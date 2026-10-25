use pal_vm::task::{TaskSystem, TASK_POOL_CAPACITY};
use pal_vm::{
    PalAnimationAxis, PalAnimationFlags, PalAnimationFrameRecord, PalInputState, PalPoint3,
    PalRect, PalSequenceAnimationDesc, PalSheetAnimationDesc, SceneTexture, SceneTextureId,
    SpriteDesc, SpriteSystem,
};

fn make_sprites() -> (SpriteSystem, pal_vm::SpriteHandle) {
    let mut sprites = SpriteSystem::new();
    sprites.insert_texture(SceneTexture::rgba8(
        SceneTextureId(10),
        1,
        96,
        24,
        vec![255; 96 * 24 * 4],
    ));
    let sprite = sprites.create(SpriteDesc {
        texture_id: SceneTextureId(10),
        texture_width: 96,
        texture_height: 24,
        cell_width: 24,
        cell_height: 24,
        position: PalPoint3::new(0, 0, 0),
        visible: true,
        ..SpriteDesc::new(SceneTextureId(10), 96, 24)
    });
    (sprites, sprite)
}

fn no_input() -> PalInputState {
    PalInputState::new()
}

// ---- Pool allocation ----

#[test]
fn task_pool_fills_to_capacity() {
    let mut tasks = TaskSystem::new();
    let mut handles = Vec::new();
    for _ in 0..TASK_POOL_CAPACITY {
        if let Some(h) = tasks.create_wait_frame(100) {
            handles.push(h);
        }
    }
    assert_eq!(handles.len(), TASK_POOL_CAPACITY);
    assert!(tasks.create_wait_frame(1).is_none(), "pool should be full");
}

#[test]
fn freed_slot_is_reusable_after_process() {
    let mut tasks = TaskSystem::new();
    let mut sprites = SpriteSystem::new();
    let input = no_input();

    // Fill pool
    let mut handles: Vec<_> = (0..TASK_POOL_CAPACITY)
        .map(|_| tasks.create_wait_frame(100).unwrap())
        .collect();
    assert!(tasks.create_wait_frame(1).is_none());

    // Free first task and process so it goes from PendingFree -> Free
    tasks.free(handles[0]);
    tasks.process(&mut sprites, &input, false, false);
    // One slot freed; can allocate again
    let new_handle = tasks.create_wait_frame(1);
    assert!(new_handle.is_some());
    handles[0] = new_handle.unwrap();
}

// ---- Pending-free lifecycle ----

#[test]
fn free_does_not_release_immediately() {
    let mut tasks = TaskSystem::new();
    let mut sprites = SpriteSystem::new();
    let input = no_input();

    let h = tasks.create_wait_frame(50).unwrap();
    assert!(tasks.is_alive(h));

    tasks.free(h);
    // Task is pending-free but still alive until process() runs
    assert!(
        tasks.is_alive(h),
        "pending-free task should still be alive before process"
    );

    tasks.process(&mut sprites, &input, false, false);
    assert!(!tasks.is_alive(h), "task must be released after process");
}

#[test]
fn wait_frame_self_frees_after_countdown() {
    let mut tasks = TaskSystem::new();
    let mut sprites = SpriteSystem::new();
    let input = no_input();

    let h = tasks.create_wait_frame(2).unwrap();

    // The first visit changes the callback without decrementing the work word.
    tasks.process(&mut sprites, &input, false, false);
    assert_eq!(tasks.task_data(h).unwrap(), &2_i32.to_le_bytes());

    // First countdown callback: remaining 2 -> 1.
    tasks.process(&mut sprites, &input, false, false);
    assert!(tasks.is_alive(h));

    // Second countdown callback: remaining 1 -> 0, FreeSelf -> PendingFree.
    tasks.process(&mut sprites, &input, false, false);
    assert!(tasks.is_alive(h), "PendingFree is still alive");

    // Deferred release follows on the next visit.
    tasks.process(&mut sprites, &input, false, false);
    assert!(!tasks.is_alive(h));
}

#[test]
fn wait_frame_forever_never_self_frees() {
    let mut tasks = TaskSystem::new();
    let mut sprites = SpriteSystem::new();
    let input = no_input();

    let h = tasks.create_wait_frame(-1).unwrap();
    for _ in 0..20 {
        tasks.process(&mut sprites, &input, false, false);
    }
    assert!(tasks.is_alive(h));
}

#[test]
fn wait_time_starts_at_first_scheduled_callback() {
    let mut tasks = TaskSystem::new();
    let mut sprites = SpriteSystem::new();
    let input = no_input();

    tasks.set_pal_time(100);
    let h = tasks.create_wait_time(250).unwrap();

    tasks.set_pal_time(349);
    tasks.process(&mut sprites, &input, false, false);
    assert!(tasks.is_alive(h));
    assert_eq!(&tasks.task_data(h).unwrap()[4..8], &349_u32.to_le_bytes());

    tasks.set_pal_time(598);
    tasks.process(&mut sprites, &input, false, false);
    assert_eq!(tasks.task_state_raw(h), 1);

    tasks.set_pal_time(599);
    tasks.process(&mut sprites, &input, false, false);
    assert!(tasks.is_alive(h), "PendingFree is still alive");

    tasks.process(&mut sprites, &input, false, false);
    assert!(!tasks.is_alive(h));
}

#[test]
fn raw_pal_task_preserves_state_message_data_and_next_process() {
    let mut tasks = TaskSystem::new();
    let handle = tasks
        .create_raw_task(0x1000, 0x2000, 8, 0, None, "RawPalTask")
        .unwrap();

    assert_eq!(
        tasks.task_state(handle),
        Some(pal_vm::task::TaskState::Active)
    );
    assert_eq!(tasks.task_state_raw(handle), 1);
    assert_eq!(tasks.task_data(handle).unwrap(), &[0; 8]);
    assert!(tasks.set_task_data(handle, vec![1, 2, 3, 4]));
    assert_eq!(tasks.task_data(handle).unwrap(), &[1, 2, 3, 4]);

    assert!(tasks.set_message(handle, 1234));
    assert_eq!(tasks.take_message(handle), 1234);
    assert_eq!(tasks.take_message(handle), 0);

    assert!(tasks.change_next(handle, 0x2000));
    assert_eq!(
        tasks.task_state(handle),
        Some(pal_vm::task::TaskState::PendingFree)
    );
}

// ---- Child depth-first order ----

#[test]
fn child_task_processes_depth_first_under_blocking_parent() {
    // PAL depth-first semantics: children are processed before the parent task itself.
    // Even under a blocking parent (WaitClick), the child animation should still advance.
    let (mut sprites, sprite) = make_sprites();
    let mut tasks = TaskSystem::new();
    let input = no_input();

    // Blocking parent: WaitClick never frees without input.
    let parent = tasks.create_wait_click().unwrap();

    // Sibling of parent (inserted in root list after parent): should be blocked.
    // We use another WaitClick to represent "never fires without input".
    let sibling = tasks.create_wait_click().unwrap();

    // Child animation under the blocking parent (delay 0: advances every process call).
    tasks.set_pal_time(0);
    let child_anim = tasks
        .create_animation_sheet(
            &mut sprites,
            PalSheetAnimationDesc {
                sprite,
                flags: PalAnimationFlags::from(PalAnimationAxis::Horizontal),
                frame_delay_ms: 0,
                running: true,
            },
            Some(parent),
        )
        .unwrap();

    // First visit initializes the child callback, still before the parent.
    tasks.process(&mut sprites, &input, false, false);

    // Next visit: children are processed first and can advance.
    tasks.process(&mut sprites, &input, false, false);

    // Child animation advanced one frame (depth-first processing confirmed).
    assert_eq!(
        tasks.get_animation_sheet(child_anim).unwrap().current_frame,
        1,
        "child animation should have advanced via depth-first processing"
    );

    // Sibling after the blocking parent must NOT have been processed.
    // Both WaitClick tasks are alive (no input given, so neither freed itself).
    assert!(tasks.is_alive(parent));
    assert!(
        tasks.is_alive(sibling),
        "sibling must not have run; it was blocked by parent"
    );
}

#[test]
fn child_animation_processes_under_blocking_parent() {
    let (mut sprites, sprite) = make_sprites();
    let mut tasks = TaskSystem::new();
    let input = no_input();

    // Blocking parent (WaitClick, never self-frees without input)
    let parent = tasks.create_wait_click().unwrap();

    // Child animation under the blocking parent
    let _child_anim = tasks
        .create_animation_sheet(
            &mut sprites,
            PalSheetAnimationDesc {
                sprite,
                flags: PalAnimationFlags::from(PalAnimationAxis::Horizontal),
                frame_delay_ms: 0,
                running: true,
            },
            Some(parent),
        )
        .unwrap();

    // Initialization uses one visit; later delay-zero visits advance one frame.
    tasks.set_pal_time(0);
    tasks.process(&mut sprites, &input, false, false);
    tasks.process(&mut sprites, &input, false, false);

    // Child was processed (frame advanced from 0 to 1).
    assert_eq!(
        sprites.get(sprite).unwrap().source_rect,
        pal_vm::PalRect::new(24, 0, 48, 24)
    );
}

// ---- Blocking task stops later siblings ----

#[test]
fn blocking_task_prevents_later_sibling_update() {
    let mut tasks = TaskSystem::new();
    let mut sprites = SpriteSystem::new();
    let input = no_input();

    // Initialization precedes the two countdown callbacks.
    let _blocker = tasks.create_wait_frame(2).unwrap();
    // Sibling after blocker: WaitFrame(1) — should NOT decrement while blocker is active
    let sibling = tasks.create_wait_frame(1).unwrap();

    tasks.process(&mut sprites, &input, false, false);

    // First countdown: blocker processes (remaining 2->1), sibling is blocked.
    tasks.process(&mut sprites, &input, false, false);
    assert!(
        tasks.is_alive(sibling),
        "sibling should still be alive (blocked)"
    );

    // Second countdown: blocker (remaining 1->0 -> PendingFree), sibling blocked.
    tasks.process(&mut sprites, &input, false, false);
    assert!(
        tasks.is_alive(sibling),
        "sibling still blocked while blocker is pending-free"
    );

    // Blocker is released; sibling now receives its initialization visit.
    tasks.process(&mut sprites, &input, false, false);
    assert_eq!(tasks.task_state_raw(sibling), 1);
    assert_eq!(tasks.task_data(sibling).unwrap(), &1_i32.to_le_bytes());

    // The next visit decrements the sibling to zero.
    tasks.process(&mut sprites, &input, false, false);
    assert!(tasks.is_alive(sibling), "sibling is now PendingFree");

    // The sibling is released on a later visit.
    tasks.process(&mut sprites, &input, false, false);
    assert!(!tasks.is_alive(sibling));
}

#[test]
fn nonblocking_animation_siblings_both_advance() {
    // Animation tasks have no blocking flag; two siblings both process every frame.
    let mut sprites = SpriteSystem::new();
    sprites.insert_texture(SceneTexture::rgba8(
        SceneTextureId(10),
        1,
        96,
        24,
        vec![255; 96 * 24 * 4],
    ));
    let sprite_a = sprites.create(SpriteDesc {
        texture_id: SceneTextureId(10),
        texture_width: 96,
        texture_height: 24,
        cell_width: 24,
        cell_height: 24,
        position: PalPoint3::new(0, 0, 0),
        ..SpriteDesc::new(SceneTextureId(10), 96, 24)
    });
    let sprite_b = sprites.create(SpriteDesc {
        texture_id: SceneTextureId(10),
        texture_width: 96,
        texture_height: 24,
        cell_width: 24,
        cell_height: 24,
        position: PalPoint3::new(0, 0, 0),
        ..SpriteDesc::new(SceneTextureId(10), 96, 24)
    });

    let mut tasks = TaskSystem::new();
    let input = no_input();

    tasks.set_pal_time(0);
    let anim_a = tasks
        .create_animation_sheet(
            &mut sprites,
            PalSheetAnimationDesc::horizontal(sprite_a, 0),
            None,
        )
        .unwrap();
    let anim_b = tasks
        .create_animation_sheet(
            &mut sprites,
            PalSheetAnimationDesc::horizontal(sprite_b, 0),
            None,
        )
        .unwrap();

    // Both initialize together, then advance on later non-blocking visits.
    tasks.process(&mut sprites, &input, false, false);
    assert_eq!(tasks.get_animation_sheet(anim_a).unwrap().current_frame, 0);
    assert_eq!(tasks.get_animation_sheet(anim_b).unwrap().current_frame, 0);

    tasks.process(&mut sprites, &input, false, false);
    assert_eq!(tasks.get_animation_sheet(anim_a).unwrap().current_frame, 1);
    assert_eq!(tasks.get_animation_sheet(anim_b).unwrap().current_frame, 1);

    tasks.process(&mut sprites, &input, false, false);
    assert_eq!(tasks.get_animation_sheet(anim_a).unwrap().current_frame, 2);
    assert_eq!(tasks.get_animation_sheet(anim_b).unwrap().current_frame, 2);
}

// ---- WaitClick responds to input ----

#[test]
fn wait_click_frees_on_any_push() {
    let mut tasks = TaskSystem::new();
    let mut sprites = SpriteSystem::new();

    let h = tasks.create_wait_click().unwrap();

    let mut input = PalInputState::new();
    // No push yet
    tasks.process(&mut sprites, &input, false, false);
    assert!(tasks.is_alive(h));

    // Simulate a mouse push
    input.begin_frame();
    input.handle_mouse_button_event(pal_vm::event::MouseButton::Left, true);
    tasks.process(&mut sprites, &input, false, false);
    // Now PendingFree
    assert!(
        tasks.is_alive(h),
        "WaitClick should be PendingFree after push"
    );

    input.begin_frame();
    tasks.process(&mut sprites, &input, false, false);
    assert!(
        !tasks.is_alive(h),
        "WaitClick must be freed after pending-free process"
    );
}

#[test]
fn wait_click_or_time_frees_on_input_before_timeout() {
    let mut tasks = TaskSystem::new();
    let mut sprites = SpriteSystem::new();
    let mut input = PalInputState::new();

    tasks.set_pal_time(100);
    let h = tasks.create_wait_click_or_time(250).unwrap();

    tasks.set_pal_time(200);
    tasks.process(&mut sprites, &input, false, false);
    assert!(tasks.is_alive(h));

    input.begin_frame();
    input.handle_mouse_button_event(pal_vm::event::MouseButton::Left, true);
    tasks.process(&mut sprites, &input, false, false);
    assert!(tasks.is_alive(h));

    input.begin_frame();
    tasks.process(&mut sprites, &input, false, false);
    assert!(!tasks.is_alive(h));
}

#[test]
fn wait_click_or_time_frees_on_timeout_without_input() {
    let mut tasks = TaskSystem::new();
    let mut sprites = SpriteSystem::new();
    let input = PalInputState::new();

    tasks.set_pal_time(100);
    let h = tasks.create_wait_click_or_time(250).unwrap();

    tasks.set_pal_time(349);
    tasks.process(&mut sprites, &input, false, false);
    assert!(tasks.is_alive(h));

    tasks.set_pal_time(350);
    tasks.process(&mut sprites, &input, false, false);
    assert!(tasks.is_alive(h));

    tasks.set_pal_time(351);
    tasks.process(&mut sprites, &input, false, false);
    assert!(!tasks.is_alive(h));
}

// ---- Animation via TaskSystem ----

#[test]
fn sheet_animation_driven_by_task_system() {
    let (mut sprites, sprite) = make_sprites();
    let mut tasks = TaskSystem::new();
    let input = no_input();

    tasks.set_pal_time(0);
    let handle = tasks
        .create_animation_sheet(
            &mut sprites,
            PalSheetAnimationDesc {
                sprite,
                flags: PalAnimationFlags::from(PalAnimationAxis::Horizontal),
                frame_delay_ms: 10,
                running: true,
            },
            None,
        )
        .unwrap();

    tasks.process(&mut sprites, &input, false, false);

    // Not enough time elapsed
    tasks.set_pal_time(5);
    tasks.process(&mut sprites, &input, false, false);
    assert_eq!(tasks.get_animation_sheet(handle).unwrap().current_frame, 0);

    // Enough time elapsed: advance one frame
    tasks.set_pal_time(10);
    tasks.process(&mut sprites, &input, false, false);
    assert_eq!(tasks.get_animation_sheet(handle).unwrap().current_frame, 1);
    assert_eq!(
        sprites.get(sprite).unwrap().source_rect,
        PalRect::new(24, 0, 48, 24)
    );
}

#[test]
fn sheet_animation_does_not_generate_draw_command() {
    // Animation tasks only modify sprite source_rect; they never produce draw commands.
    // Draw commands are produced exclusively by SpriteSystem based on sprite visibility.
    let mut sprites = SpriteSystem::new();
    sprites.insert_texture(SceneTexture::rgba8(
        SceneTextureId(10),
        1,
        96,
        24,
        vec![255; 96 * 24 * 4],
    ));
    // Create sprite with visible: false (default from SpriteDesc::new).
    let sprite = sprites.create(SpriteDesc::new(SceneTextureId(10), 96, 24));
    // Confirm: invisible sprite, no commands yet.
    assert!(sprites.commands().is_empty());

    let mut tasks = TaskSystem::new();
    let input = no_input();
    tasks.set_pal_time(0);
    let _h = tasks
        .create_animation_sheet(
            &mut sprites,
            PalSheetAnimationDesc {
                sprite,
                flags: PalAnimationFlags::from(PalAnimationAxis::Horizontal),
                frame_delay_ms: 0,
                running: true,
            },
            None,
        )
        .unwrap();

    tasks.process(&mut sprites, &input, false, false);
    tasks.process(&mut sprites, &input, false, false);
    tasks.process(&mut sprites, &input, false, false);

    // Animation ran, but sprite is still invisible — no draw commands.
    let cmds = sprites.commands();
    assert!(
        cmds.is_empty(),
        "animation task must not produce draw commands; only visible sprites do"
    );

    // Now make visible: SpriteSystem produces draw commands, animation still doesn't add extras.
    sprites.view_ctrl(sprite, true);
    let cmds = sprites.commands();
    assert_eq!(
        cmds.len(),
        1,
        "one visible sprite should produce exactly one draw command"
    );
}

#[test]
fn sequence_animation_driven_by_task_system() {
    let (mut sprites, sprite) = make_sprites();
    let mut tasks = TaskSystem::new();
    let input = no_input();

    tasks.set_pal_time(0);
    let handle = tasks
        .create_animation_sequence(
            &mut sprites,
            PalSequenceAnimationDesc {
                sprite,
                frames: vec![
                    PalAnimationFrameRecord::new(PalRect::new(0, 0, 24, 24), 5, 1),
                    PalAnimationFrameRecord::new(PalRect::new(24, 0, 48, 24), 5, -1),
                ],
                running: true,
            },
            None,
        )
        .unwrap();

    // Frame 0 is applied immediately on create
    assert_eq!(
        sprites.get(sprite).unwrap().source_rect,
        PalRect::new(0, 0, 24, 24)
    );

    tasks.process(&mut sprites, &input, false, false);

    // Not enough time
    tasks.set_pal_time(3);
    tasks.process(&mut sprites, &input, false, false);
    assert_eq!(
        tasks.get_animation_sequence(handle).unwrap().current_frame,
        0
    );

    // Advance to frame 1
    tasks.set_pal_time(5);
    tasks.process(&mut sprites, &input, false, false);
    assert_eq!(
        tasks.get_animation_sequence(handle).unwrap().current_frame,
        1
    );
    assert_eq!(
        sprites.get(sprite).unwrap().source_rect,
        PalRect::new(24, 0, 48, 24)
    );

    // Terminal frame: current_frame becomes -1
    tasks.set_pal_time(10);
    tasks.process(&mut sprites, &input, false, false);
    assert_eq!(
        tasks.get_animation_sequence(handle).unwrap().current_frame,
        -1
    );
    // Rect stays at last frame
    assert_eq!(
        sprites.get(sprite).unwrap().source_rect,
        PalRect::new(24, 0, 48, 24)
    );
}

fn wait_word(tasks: &TaskSystem, handle: pal_vm::task::TaskHandle, offset: usize) -> u32 {
    u32::from_le_bytes(
        tasks.task_data(handle).unwrap()[offset..offset + 4]
            .try_into()
            .unwrap(),
    )
}

#[test]
fn frame_wait_zero_and_negative_words_follow_native_wrapping_countdown() {
    for (initial, expected) in [(0, -1), (-1, -1), (-2, -3), (i32::MIN, i32::MAX)] {
        let mut tasks = TaskSystem::new();
        let mut sprites = SpriteSystem::new();
        let handle = tasks.create_wait_frame(initial).unwrap();
        tasks.process(&mut sprites, &no_input(), false, false);
        assert_eq!(wait_word(&tasks, handle, 0) as i32, initial);
        tasks.process(&mut sprites, &no_input(), false, false);
        assert_eq!(wait_word(&tasks, handle, 0) as i32, expected);
        assert_eq!(tasks.task_state_raw(handle), 1);
        if expected == -1 {
            for _ in 0..8 {
                tasks.process(&mut sprites, &no_input(), false, false);
            }
            assert_eq!(wait_word(&tasks, handle, 0), u32::MAX);
            assert_eq!(tasks.task_state_raw(handle), 1);
        }
    }
}

#[test]
fn frame_wait_reads_mutated_native_work_word() {
    let mut tasks = TaskSystem::new();
    let mut sprites = SpriteSystem::new();
    let handle = tasks.create_wait_frame(-1).unwrap();
    tasks.process(&mut sprites, &no_input(), false, false);
    tasks
        .task_data_mut(handle)
        .unwrap()
        .copy_from_slice(&1_i32.to_le_bytes());
    tasks.process(&mut sprites, &no_input(), false, false);
    assert_eq!(tasks.task_state_raw(handle), 2);
    assert_eq!(wait_word(&tasks, handle, 0), 0);
}

#[test]
fn time_wait_zero_requires_initialization_then_comparison_then_release() {
    let mut tasks = TaskSystem::new();
    let mut sprites = SpriteSystem::new();
    let handle = tasks.create_wait_time(0).unwrap();
    tasks.set_pal_time(400);
    tasks.process(&mut sprites, &no_input(), false, false);
    assert_eq!(tasks.task_state_raw(handle), 1);
    tasks.process(&mut sprites, &no_input(), false, false);
    assert_eq!(tasks.task_state_raw(handle), 2);
    tasks.process(&mut sprites, &no_input(), false, false);
    assert!(!tasks.is_alive(handle));
}

#[test]
fn time_wait_compares_unsigned_elapsed_across_clock_wrap() {
    let mut tasks = TaskSystem::new();
    let mut sprites = SpriteSystem::new();
    let handle = tasks.create_wait_time(32).unwrap();
    tasks.set_pal_time(0xffff_fff0);
    tasks.process(&mut sprites, &no_input(), false, false);
    tasks.set_pal_time(0x0f);
    tasks.process(&mut sprites, &no_input(), false, false);
    assert_eq!(tasks.task_state_raw(handle), 1);
    tasks.set_pal_time(0x10);
    tasks.process(&mut sprites, &no_input(), false, false);
    assert_eq!(tasks.task_state_raw(handle), 2);
}

#[test]
fn time_wait_reads_mutated_duration_and_start_words() {
    let mut tasks = TaskSystem::new();
    let mut sprites = SpriteSystem::new();
    let handle = tasks.create_wait_time(u32::MAX).unwrap();
    tasks.set_pal_time(10);
    tasks.process(&mut sprites, &no_input(), false, false);
    tasks.set_pal_time(0x8000_0010);
    tasks.process(&mut sprites, &no_input(), false, false);
    assert_eq!(
        tasks.task_state_raw(handle),
        1,
        "high-bit duration stays unsigned"
    );
    let work = tasks.task_data_mut(handle).unwrap();
    work[..4].copy_from_slice(&32_u32.to_le_bytes());
    work[4..8].copy_from_slice(&0x8000_0000_u32.to_le_bytes());
    tasks.set_pal_time(0x8000_0020);
    tasks.process(&mut sprites, &no_input(), false, false);
    assert_eq!(tasks.task_state_raw(handle), 2);
}

#[test]
fn wait_time_ex_uses_relative_sibling_insertion_and_twelve_byte_work() {
    let mut tasks = TaskSystem::new();
    let mut sprites = SpriteSystem::new();
    let parent = tasks.create_raw_task(1, 0, 0, 0, None, "Parent").unwrap();
    let relative = tasks
        .create_raw_task(1, 0, 0, 0, Some(parent), "Relative")
        .unwrap();
    let later = tasks
        .create_raw_task(1, 0, 0, 0, Some(parent), "Later")
        .unwrap();
    let wait = tasks.create_wait_time_ex(50, Some(relative)).unwrap();
    let dump = tasks.dump_entries();
    let wait_entry = dump.iter().find(|entry| entry.handle == wait).unwrap();
    let relative_entry = dump.iter().find(|entry| entry.handle == relative).unwrap();
    assert_eq!(wait_entry.parent_index, relative_entry.parent_index);
    assert_eq!(relative_entry.children_count, 0);
    assert_eq!(wait_entry.flags, 0x10001);
    assert_eq!(tasks.task_data(wait).unwrap().len(), 12);
    assert_eq!(wait_word(&tasks, wait, 0), 0);
    assert_eq!(wait_word(&tasks, wait, 4), 50);
    assert_eq!(wait_word(&tasks, wait, 8), 0);
    tasks.set_pal_time(200);
    tasks.process(&mut sprites, &no_input(), false, false);
    assert_eq!(wait_word(&tasks, wait, 8), 200);
    tasks.set_pal_time(250);
    tasks.process(&mut sprites, &no_input(), false, false);
    assert_eq!(tasks.task_state_raw(wait), 2);
    assert!(tasks.is_alive(later));
    tasks.process(&mut sprites, &no_input(), false, false);
    assert!(!tasks.is_alive(wait));
    assert!(tasks.create_wait_time_ex(10, Some(wait)).is_none());
}

#[test]
fn self_finishing_parent_marks_descendants_before_deferred_release() {
    let (mut sprites, sprite) = make_sprites();
    let mut tasks = TaskSystem::new();
    let parent = tasks.create_wait_frame(1).unwrap();
    let child = tasks
        .create_animation_sheet(
            &mut sprites,
            PalSheetAnimationDesc::horizontal(sprite, 0),
            Some(parent),
        )
        .unwrap();
    tasks.process(&mut sprites, &no_input(), false, false);
    tasks.process(&mut sprites, &no_input(), false, false);
    assert_eq!(tasks.task_state_raw(parent), 2);
    assert_eq!(tasks.task_state_raw(child), 2);
    let rect = sprites.get(sprite).unwrap().source_rect;
    tasks.process(&mut sprites, &no_input(), false, false);
    assert!(!tasks.is_alive(parent));
    assert!(!tasks.is_alive(child));
    assert_eq!(
        sprites.get(sprite).unwrap().source_rect,
        rect,
        "child destructor visit must not advance animation"
    );
}

#[test]
fn freeing_parent_selects_every_descendant_destructor() {
    let mut tasks = TaskSystem::new();
    let parent = tasks.create_raw_task(1, 2, 0, 0, None, "Parent").unwrap();
    let child = tasks
        .create_raw_task(3, 4, 0, 0, Some(parent), "Child")
        .unwrap();
    let grandchild = tasks
        .create_raw_task(5, 6, 0, 0, Some(child), "Grandchild")
        .unwrap();
    assert!(tasks.free(parent));
    for task in [parent, child, grandchild] {
        // GetState reports 2 by callback equality even if a caller overwrites
        // the state byte after Free has selected each destructor.
        assert!(tasks.change_state(task, pal_vm::task::TaskState::Active));
        assert_eq!(tasks.task_state_raw(task), 2);
    }
}

#[test]
fn sentinel_state_skips_callback_but_still_processes_children() {
    let (mut sprites, sprite) = make_sprites();
    let mut tasks = TaskSystem::new();
    let parent = tasks.create_wait_frame(1).unwrap();
    let child = tasks
        .create_animation_sheet(
            &mut sprites,
            PalSheetAnimationDesc::horizontal(sprite, 0),
            Some(parent),
        )
        .unwrap();
    tasks.change_state(parent, pal_vm::task::TaskState::ChildSentinel);
    for _ in 0..3 {
        tasks.process(&mut sprites, &no_input(), false, false);
    }
    assert_eq!(wait_word(&tasks, parent, 0), 1);
    assert_eq!(tasks.task_state_raw(parent), 3);
    assert_eq!(tasks.get_animation_sheet(child).unwrap().current_frame, 2);
}

#[test]
fn native_blocked_cleanup_does_not_visit_later_sibling_descendants() {
    let mut tasks = TaskSystem::new();
    let mut sprites = SpriteSystem::new();
    tasks.create_wait_frame(-1).unwrap();
    let parent = tasks.create_raw_task(1, 0, 0, 0, None, "Parent").unwrap();
    let child = tasks
        .create_raw_task(1, 0, 0, 0, Some(parent), "Child")
        .unwrap();
    tasks.free(parent);
    tasks.process(&mut sprites, &no_input(), false, false);
    assert!(!tasks.is_alive(parent));
    assert!(
        tasks.is_alive(child),
        "native pending-only traversal orphans this child"
    );
    assert_eq!(tasks.task_state_raw(child), 2);
}

#[test]
fn native_task_context_and_selected_work_context_are_distinct() {
    let mut tasks = TaskSystem::new();
    let mut sprites = SpriteSystem::new();
    let first = tasks.create_raw_task(1, 0, 4, 0, None, "First").unwrap();
    let second = tasks.create_raw_task(1, 0, 4, 0, None, "Second").unwrap();
    tasks.set_task_data(first, vec![1, 2, 3, 4]);
    tasks.set_task_data(second, vec![5, 6, 7, 8]);
    assert_eq!(tasks.native_free(None), 0);
    assert_eq!(tasks.native_get_task_data(None), None);
    tasks.native_select_task_data(Some(first)).unwrap();
    tasks.process(&mut sprites, &no_input(), false, false);
    tasks.native_set_message(None, 0xfedc_ba98).unwrap();
    assert_eq!(tasks.native_get_message(Some(second)), Some(0xfedc_ba98));
    assert_eq!(tasks.native_get_message(Some(second)), Some(0));
    assert_eq!(
        tasks.native_get_task_data(None),
        Some([1, 2, 3, 4].as_slice())
    );
    tasks.native_select_task_data(None).unwrap();
    assert_eq!(
        tasks.native_get_task_data(None),
        Some([5, 6, 7, 8].as_slice())
    );
    assert_eq!(
        tasks.native_get_task_data(Some(first)),
        Some([1, 2, 3, 4].as_slice())
    );
    assert_eq!(tasks.native_free(None), 1);
    assert_eq!(tasks.native_get_state(Some(second)), Some(2));
}

#[test]
fn reprocess_visits_later_siblings_and_restores_current_context() {
    let (mut sprites, sprite) = make_sprites();
    let mut tasks = TaskSystem::new();
    let blocker = tasks.create_wait_frame(-1).unwrap();
    let parent = tasks.create_raw_task(1, 0, 0, 0, None, "Parent").unwrap();
    let first = tasks
        .create_animation_sheet(
            &mut sprites,
            PalSheetAnimationDesc::horizontal(sprite, 0),
            Some(parent),
        )
        .unwrap();
    let second = tasks
        .create_animation_sheet(
            &mut sprites,
            PalSheetAnimationDesc::horizontal(sprite, 0),
            Some(parent),
        )
        .unwrap();
    let current = tasks.create_raw_task(1, 0, 0, 0, None, "Current").unwrap();
    tasks.process(&mut sprites, &no_input(), false, false);
    tasks
        .reprocess(Some(first), &mut sprites, &no_input(), false, false)
        .unwrap();
    assert_eq!(tasks.get_animation_sheet(first).unwrap().current_frame, 0);
    assert_eq!(tasks.get_animation_sheet(second).unwrap().current_frame, 0);
    tasks
        .reprocess(Some(first), &mut sprites, &no_input(), false, false)
        .unwrap();
    assert_eq!(tasks.get_animation_sheet(first).unwrap().current_frame, 1);
    assert_eq!(tasks.get_animation_sheet(second).unwrap().current_frame, 1);
    tasks.native_set_message(None, 0xfeed).unwrap();
    assert_eq!(tasks.native_get_message(Some(current)), Some(0xfeed));
    assert_eq!(tasks.native_get_state(Some(blocker)), Some(1));
}

#[test]
fn null_wait_time_ex_inserts_after_current_instead_of_at_root_end() {
    let mut tasks = TaskSystem::new();
    let mut sprites = SpriteSystem::new();
    let current = tasks.create_raw_task(1, 0, 0, 0, None, "Current").unwrap();
    tasks.process(&mut sprites, &no_input(), false, false);
    let later = tasks.create_wait_frame(1).unwrap();
    let timed = tasks.create_wait_time_ex(0, None).unwrap();
    tasks.process(&mut sprites, &no_input(), false, false);
    tasks.process(&mut sprites, &no_input(), false, false);
    assert_eq!(tasks.native_get_state(Some(timed)), Some(2));
    assert_eq!(tasks.native_get_state(Some(later)), Some(1));
    assert_eq!(wait_word(&tasks, later, 0), 1);
    assert_eq!(
        tasks
            .dump_entries()
            .iter()
            .find(|entry| entry.handle == current)
            .unwrap()
            .children_count,
        0
    );
}
