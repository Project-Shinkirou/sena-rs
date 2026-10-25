use pal_vm::{
    DrawCommand, PalColor, PalPoint2, PalPoint3, PalRect, PalRenderMode, PalVec3, SceneTexture,
    SceneTextureId, SpriteDesc, SpriteSurfaceId, SpriteSystem,
};

fn make_system() -> SpriteSystem {
    let mut sprites = SpriteSystem::new();
    sprites.insert_texture(SceneTexture::rgba8(
        SceneTextureId(10),
        1,
        64,
        32,
        vec![255; 64 * 32 * 4],
    ));
    sprites.insert_texture(SceneTexture::rgba8(
        SceneTextureId(11),
        1,
        64,
        32,
        vec![255; 64 * 32 * 4],
    ));
    sprites
}

#[test]
fn rgba_sprite_create_and_replace_updates_draw_surface() {
    let mut sprites = SpriteSystem::new();
    let handle = sprites
        .create_rgba_sprite(
            2,
            1,
            vec![255, 0, 0, 255, 0, 255, 0, 255],
            PalVec3::new(10, 20, 30),
            30,
            "text:first",
        )
        .expect("rgba sprite should be created");
    let sprite = sprites.get(handle).unwrap();
    assert_eq!(sprite.cell_size.width, 2);
    assert_eq!(sprite.cell_size.height, 1);
    assert_eq!(sprite.source_name, "text:first");
    assert_eq!(sprites.commands().len(), 1);

    assert!(sprites.replace_sprite_surface(
        handle,
        1,
        2,
        vec![0, 0, 255, 255, 255, 255, 255, 128],
        "text:next"
    ));
    let sprite = sprites.get(handle).unwrap();
    assert_eq!(sprite.texture_size.width, 1);
    assert_eq!(sprite.texture_size.height, 2);
    assert_eq!(sprite.source_rect, PalRect::new(0, 0, 1, 2));
    assert_eq!(sprite.source_name, "text:next");
}

#[test]
fn image_overlay_composites_into_existing_sprite_surface() {
    let mut sprites = SpriteSystem::new();
    let handle = sprites
        .create_rgba_sprite(
            3,
            1,
            vec![0, 0, 128, 255].repeat(3),
            PalVec3::new(0, 0, 0),
            0,
            "popup",
        )
        .unwrap();
    let before = sprites
        .surface(sprites.get(handle).unwrap().surface)
        .unwrap()
        .generation;
    assert!(sprites.composite_rgba_to_sprite(
        handle,
        1,
        0,
        2,
        1,
        &[255, 0, 0, 255, 0, 255, 0, 0],
        0,
        0,
        2,
        1,
    ));
    let surface = sprites
        .surface(sprites.get(handle).unwrap().surface)
        .unwrap();
    assert!(surface.generation > before);
    assert_eq!(
        &*surface.pixels,
        &[0, 0, 128, 255, 255, 0, 0, 255, 0, 0, 128, 255]
    );
}

#[test]
fn release_removes_render_node_and_unreferenced_surface() {
    let mut sprites = make_system();
    let first = sprites.create(SpriteDesc {
        texture_id: SceneTextureId(10),
        texture_width: 64,
        texture_height: 32,
        visible: true,
        ..SpriteDesc::new(SceneTextureId(10), 64, 32)
    });
    let second = sprites.create(SpriteDesc {
        texture_id: SceneTextureId(10),
        texture_width: 64,
        texture_height: 32,
        visible: true,
        ..SpriteDesc::new(SceneTextureId(10), 64, 32)
    });

    assert_eq!(sprites.len(), 2);
    assert_eq!(sprites.render_node_count(), 2);
    assert!(sprites.surface(SpriteSurfaceId(10)).is_some());

    assert!(sprites.release(first));
    assert_eq!(sprites.len(), 1);
    assert_eq!(sprites.render_node_count(), 1);
    assert!(sprites.surface(SpriteSurfaceId(10)).is_some());

    assert!(sprites.release(second));
    assert_eq!(sprites.len(), 0);
    assert_eq!(sprites.render_node_count(), 0);
    assert!(sprites.surface(SpriteSurfaceId(10)).is_none());
}

#[test]
fn sprite_rect_reset_uses_texture_size_but_rect_set_pos_uses_cell_size() {
    let mut sprites = make_system();
    let sprite = sprites.create(SpriteDesc {
        texture_id: SceneTextureId(10),
        texture_width: 64,
        texture_height: 32,
        cell_width: 16,
        cell_height: 16,
        visible: true,
        ..SpriteDesc::new(SceneTextureId(10), 64, 32)
    });

    assert_eq!(
        sprites.get(sprite).unwrap().source_rect,
        PalRect::new(0, 0, 16, 16)
    );
    assert!(sprites.rect_set_pos(sprite, 3, 1));
    assert_eq!(
        sprites.get(sprite).unwrap().source_rect,
        PalRect::new(48, 16, 64, 32)
    );
    assert!(sprites.set_rect(sprite, None));
    assert_eq!(
        sprites.get(sprite).unwrap().source_rect,
        PalRect::new(0, 0, 64, 32)
    );
}

#[test]
fn pal_cell_indices_keep_native_word_width_and_wrapping_products() {
    let mut sprites = make_system();
    let handle = sprites.create(SpriteDesc {
        cell_width: 16,
        cell_height: 16,
        ..SpriteDesc::new(SceneTextureId(10), 64, 32)
    });
    // Both audited DLLs use 32-bit IMUL followed by unsigned bounds checks.
    // Truncating to u16, clamping negatives, or saturating multiplication
    // changes at least one of these native results.
    for (x, y, expected) in [
        (65_536, -1, PalRect::new(48, 16, 64, 32)),
        (-1, 65_536, PalRect::new(48, 16, 64, 32)),
        (0x1000_0001, 0x1000_0001, PalRect::new(16, 16, 32, 32)),
        (i32::MIN, i32::MIN, PalRect::new(0, 0, 16, 16)),
    ] {
        assert!(sprites.rect_set_pos(handle, x, y));
        assert_eq!(sprites.get(handle).unwrap().source_rect, expected);
    }
}

#[test]
fn pal_cell_indices_preserve_native_oversized_cell_subtraction() {
    let mut sprites = make_system();
    let handle = sprites.create(SpriteDesc {
        cell_width: 80,
        cell_height: 48,
        ..SpriteDesc::new(SceneTextureId(10), 64, 32)
    });
    assert!(sprites.rect_set_pos(handle, 1, 1));
    assert_eq!(
        sprites.get(handle).unwrap().source_rect,
        PalRect::new(-16, -16, 64, 32)
    );
}

#[test]
fn registered_cells_preserve_signed_indices_and_ignore_one_pixel_padding() {
    let mut sprites = make_system();
    let handle = sprites.create(SpriteDesc {
        cell_width: 16,
        cell_height: 16,
        ..SpriteDesc::new(SceneTextureId(10), 65, 34)
    });
    sprites.set_registered_profile(true);
    for (x, y, expected) in [
        (-1, -2, PalRect::new(-16, -32, 0, -16)),
        (4, 2, PalRect::new(48, 18, 64, 34)),
        (i32::MAX, i32::MAX, PalRect::new(48, 18, 64, 34)),
        (i32::MIN, i32::MIN, PalRect::new(0, 0, 16, 16)),
    ] {
        assert!(sprites.rect_set_pos(handle, x, y));
        assert_eq!(sprites.get(handle).unwrap().source_rect, expected);
    }
    // Changing back also updates sprites created before profile selection.
    sprites.set_registered_profile(false);
    assert!(sprites.rect_set_pos(handle, -1, -2));
    assert_eq!(
        sprites.get(handle).unwrap().source_rect,
        PalRect::new(49, 18, 65, 34)
    );
}

#[test]
fn center_offset_setter_stores_negative_value_and_does_not_move_2d_draw_position() {
    let mut sprites = make_system();
    let sprite = sprites.create(SpriteDesc {
        texture_id: SceneTextureId(10),
        texture_width: 64,
        texture_height: 32,
        cell_width: 16,
        cell_height: 16,
        position: PalPoint3::new(10, 20, 0),
        offset: PalPoint2::new(3, 4),
        visible: true,
        ..SpriteDesc::new(SceneTextureId(10), 64, 32)
    });

    assert!(sprites.set_center_offset(sprite, 8, 9));
    let pal_sprite = sprites.get(sprite).unwrap();
    assert_eq!(pal_sprite.center_offset, PalPoint2::new(-8, -9));

    let commands = sprites.commands();
    let pal_vm::DrawCommand::Sprite(draw) = &commands[0] else {
        panic!("expected sprite draw")
    };
    assert_eq!(draw.dst.x, 13.0);
    assert_eq!(draw.dst.y, 24.0);
    assert_eq!(draw.center_offset, [-8.0, -9.0]);
}

#[test]
fn render_nodes_sort_by_effective_priority() {
    let mut sprites = make_system();
    let low = sprites.create(SpriteDesc {
        texture_id: SceneTextureId(10),
        texture_width: 64,
        texture_height: 32,
        visible: true,
        base_priority: 20,
        ..SpriteDesc::new(SceneTextureId(10), 64, 32)
    });
    let high = sprites.create(SpriteDesc {
        texture_id: SceneTextureId(11),
        texture_width: 64,
        texture_height: 32,
        visible: true,
        base_priority: 10,
        position: PalPoint3::new(0, 0, 5),
        ..SpriteDesc::new(SceneTextureId(11), 64, 32)
    });

    let commands = sprites.commands();
    let pal_vm::DrawCommand::Sprite(first) = &commands[0] else {
        panic!("expected sprite draw")
    };
    let pal_vm::DrawCommand::Sprite(second) = &commands[1] else {
        panic!("expected sprite draw")
    };
    assert_eq!(first.texture_id, SceneTextureId(11));
    assert_eq!(first.priority, 15);
    assert_eq!(second.texture_id, SceneTextureId(10));
    assert_eq!(second.priority, 20);

    assert!(sprites.set_pos(high, 0, 0, 30));
    let commands = sprites.commands();
    let pal_vm::DrawCommand::Sprite(first) = &commands[0] else {
        panic!("expected sprite draw")
    };
    assert_eq!(first.texture_id, SceneTextureId(10));
    assert_eq!(sprites.get(low).unwrap().effective_priority(), 20);
}

#[test]
fn equal_priority_draws_newer_render_nodes_over_older_nodes() {
    let mut sprites = make_system();
    let foreground = sprites.create(SpriteDesc {
        texture_id: SceneTextureId(10),
        texture_width: 64,
        texture_height: 32,
        visible: true,
        base_priority: 0,
        ..SpriteDesc::new(SceneTextureId(10), 64, 32)
    });
    let fullscreen_background = sprites.create(SpriteDesc {
        texture_id: SceneTextureId(11),
        texture_width: 1280,
        texture_height: 720,
        visible: true,
        base_priority: 0,
        ..SpriteDesc::new(SceneTextureId(11), 1280, 720)
    });

    let commands = sprites.commands();
    let pal_vm::DrawCommand::Sprite(first) = &commands[0] else {
        panic!("expected sprite draw")
    };
    let pal_vm::DrawCommand::Sprite(second) = &commands[1] else {
        panic!("expected sprite draw")
    };
    assert_eq!(first.texture_id, SceneTextureId(10));
    assert_eq!(second.texture_id, SceneTextureId(11));
    assert_eq!(
        sprites
            .get(fullscreen_background)
            .unwrap()
            .effective_priority(),
        sprites.get(foreground).unwrap().effective_priority()
    );
}

#[test]
fn transition_or_lock_suppresses_normal_sprite_draw() {
    let mut sprites = make_system();
    let sprite = sprites.create(SpriteDesc {
        texture_id: SceneTextureId(10),
        texture_width: 64,
        texture_height: 32,
        visible: true,
        color: PalColor::from_argb(0x80FF_0000),
        render_mode: PalRenderMode::new(3),
        ..SpriteDesc::new(SceneTextureId(10), 64, 32)
    });
    assert_eq!(sprites.commands().len(), 1);

    sprites.get_mut(sprite).unwrap().transition_block = 1;
    assert!(sprites.commands().is_empty());
    sprites.get_mut(sprite).unwrap().transition_block = 0;

    assert!(sprites.lock(sprite).is_some());
    assert!(sprites.commands().is_empty());
    assert!(sprites.unlock(sprite));
    assert_eq!(sprites.commands().len(), 1);
}

#[test]
fn transparent_sprite_keeps_a_node_for_persistent_native_blend_state() {
    let mut sprites = make_system();
    let sprite = sprites.create(SpriteDesc {
        texture_id: SceneTextureId(10),
        texture_width: 64,
        texture_height: 32,
        visible: true,
        color: PalColor::from_argb(0x00FF_FFFF),
        ..SpriteDesc::new(SceneTextureId(10), 64, 32)
    });

    assert_eq!(sprites.commands().len(), 1);
    sprites.get_mut(sprite).expect("sprite should exist").color = PalColor::from_argb(0x01FF_FFFF);
    assert_eq!(sprites.commands().len(), 1);
}

#[test]
fn sprite_scale_preserves_visible_source_aspect() {
    let mut sprites = make_system();
    let sprite = sprites.create(SpriteDesc {
        texture_id: SceneTextureId(10),
        texture_width: 64,
        texture_height: 32,
        visible: true,
        position: PalVec3::new(10, 20, 0),
        ..SpriteDesc::new(SceneTextureId(10), 64, 32)
    });
    assert!(sprites.set_center_offset(sprite, 32, 16));
    assert!(sprites.set_scale(sprite, 2.0));

    let DrawCommand::Sprite(draw) = &sprites.commands()[0] else {
        panic!("scaled sprite should draw");
    };
    // PAL.dll scales around the source rect center, so doubling a 64x32 sprite
    // shifts the submitted top-left position by half of the 64x32 growth.
    assert!((draw.dst.x - (-22.0)).abs() < 0.001);
    assert!((draw.dst.y - 4.0).abs() < 0.001);
    assert!((draw.dst.w - 128.0).abs() < 0.001);
    assert!((draw.dst.h - 64.0).abs() < 0.001);
    assert_eq!(draw.center_offset, [-32.0, -16.0]);
}

#[test]
fn render_modes_keep_native_words_and_scope_extended_operations_by_build() {
    let mut sprites = make_system();
    let sprite = sprites.create(SpriteDesc {
        visible: true,
        ..SpriteDesc::new(SceneTextureId(10), 64, 32)
    });
    sprites.set_render_mode(sprite, pal_vm::sprite::PalRenderMode::new(5));
    assert_eq!(sprites.get(sprite).unwrap().render_mode.raw(), 5);
    let DrawCommand::Sprite(draw) = &sprites.commands()[0] else {
        panic!("sprite draw");
    };
    assert_eq!(draw.render_mode, 1);
    sprites.set_registered_profile(true);
    let DrawCommand::Sprite(draw) = &sprites.commands()[0] else {
        panic!("sprite draw");
    };
    assert_eq!(draw.render_mode, 5);
    sprites.set_alpha(sprite, 0);
    assert_eq!(
        sprites.commands().len(),
        1,
        "multiplication ignores source alpha"
    );
    sprites.set_render_mode(sprite, pal_vm::sprite::PalRenderMode::new(1));
    assert_eq!(
        sprites.commands().len(),
        1,
        "the render walk resolves persistent blend state before alpha optimization"
    );
}

#[test]
fn newer_sprite_scale_preserves_independent_axes_and_uniform_reset() {
    let mut sprites = make_system();
    let sprite = sprites.create(SpriteDesc {
        visible: true,
        position: PalVec3::new(10, 20, 0),
        ..SpriteDesc::new(SceneTextureId(10), 64, 32)
    });
    assert!(sprites.set_scale_xy(sprite, 2.0, 0.5));
    let DrawCommand::Sprite(draw) = &sprites.commands()[0] else {
        panic!("anisotropic sprite should draw");
    };
    assert_eq!(
        (draw.dst.x, draw.dst.y, draw.dst.w, draw.dst.h),
        (-22.0, 28.0, 128.0, 16.0)
    );
    let info = sprites.get(sprite).unwrap().info();
    assert_eq!(info.scale_y, Some(0.5));
    assert!(sprites.set_scale(sprite, 1.0));
    assert_eq!(sprites.get(sprite).unwrap().scale_y, None);
    let DrawCommand::Sprite(draw) = &sprites.commands()[0] else {
        panic!("uniform sprite should draw");
    };
    assert_eq!(
        (draw.dst.x, draw.dst.y, draw.dst.w, draw.dst.h),
        (10.0, 20.0, 64.0, 32.0)
    );
}

#[test]
fn offset_rect_and_change_priority_match_pal_sprite_external_semantics() {
    let mut sprites = make_system();
    let sprite = sprites.create(SpriteDesc {
        texture_id: SceneTextureId(10),
        texture_width: 64,
        texture_height: 32,
        visible: true,
        base_priority: 10,
        position: PalPoint3::new(0, 0, 7),
        ..SpriteDesc::new(SceneTextureId(10), 64, 32)
    });

    assert_eq!(sprites.get_width(sprite), Some(64));
    assert_eq!(sprites.get_height(sprite), Some(32));
    assert!(sprites.set_offset_rect(sprite, 8, 4));
    assert_eq!(sprites.get_width(sprite), Some(8));
    assert_eq!(sprites.get_height(sprite), Some(4));

    assert!(sprites.change_priority(sprite, 3));
    let pal_vm::DrawCommand::Sprite(draw) = &sprites.commands()[0] else {
        panic!("expected sprite draw")
    };
    assert_eq!(draw.priority, 3);
    assert_eq!(
        sprites
            .render_nodes()
            .find(|node| node.sprite == sprite)
            .unwrap()
            .priority,
        3
    );
    assert!(sprites.set_offset_rect(sprite, 0, u32::MAX));
    assert_eq!(sprites.get_width(sprite), Some(0));
    assert_eq!(sprites.get_height(sprite), Some(u32::MAX));
    assert!(sprites.set_offset_pos(sprite, i32::MAX, i32::MIN));
    assert!(sprites.move_offset_pos(sprite, 1, -1));
    assert_eq!(sprites.get(sprite).unwrap().offset.x, i32::MIN);
    assert_eq!(sprites.get(sprite).unwrap().offset.y, i32::MAX);
    assert!(sprites.set_pos_float(sprite, 0.0, 0.0, f32::INFINITY));
    assert_eq!(
        sprites
            .render_nodes()
            .find(|node| node.sprite == sprite)
            .unwrap()
            .priority,
        i32::MIN + 3
    );
}

#[test]
fn sprite_mix_ex_truncates_retained_alpha_and_clears_transparent_pixels() {
    let mut sprites = SpriteSystem::new();
    let dst = sprites
        .create_rgba_sprite(
            2,
            1,
            vec![4, 5, 6, 0, 20, 40, 60, 70],
            PalVec3::default(),
            0,
            "dst",
        )
        .unwrap();
    let src = sprites
        .create_rgba_sprite(
            2,
            1,
            vec![1, 2, 3, 0, 200, 100, 0, 100],
            PalVec3::default(),
            0,
            "src",
        )
        .unwrap();
    assert!(sprites.mix_sprite_to_sprite_ex(dst, 0, 0, 2, 1, src, 0, 0));
    let pixels = &sprites
        .surface(sprites.get(dst).unwrap().surface)
        .unwrap()
        .pixels;
    assert_eq!(&pixels[..], &[0, 0, 0, 0, 146, 82, 17, 142]);
    assert!(!sprites.mix_sprite_to_sprite_ex(dst, 0, 0, -1, 1, src, 0, 0));
}

#[test]
fn sprite_surface_copy_mix_and_mask_mutate_pixels() {
    let mut sprites = SpriteSystem::new();
    sprites.insert_texture(SceneTexture::rgba8(
        SceneTextureId(20),
        1,
        2,
        2,
        vec![
            10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160,
        ],
    ));
    sprites.insert_texture(SceneTexture::rgba8(
        SceneTextureId(21),
        1,
        2,
        2,
        vec![
            200, 210, 220, 230, 200, 210, 220, 230, 200, 210, 220, 230, 200, 210, 220, 230,
        ],
    ));
    sprites.insert_texture(SceneTexture::rgba8(
        SceneTextureId(22),
        1,
        2,
        2,
        vec![
            0, 0, 128, 255, 0, 0, 128, 255, 0, 0, 128, 255, 0, 0, 128, 255,
        ],
    ));

    let src = sprites.create(SpriteDesc::new(SceneTextureId(20), 2, 2));
    let dst = sprites.create(SpriteDesc::new(SceneTextureId(21), 2, 2));
    let mask = sprites.create(SpriteDesc::new(SceneTextureId(22), 2, 2));

    assert!(sprites.copy_sprite_to_sprite_rgb(dst, 0, 0, src, 0, 0, 1, 1));
    let dst_surface = sprites.surface(SpriteSurfaceId(21)).unwrap();
    assert_eq!(&dst_surface.pixels[0..4], &[10, 20, 30, 40]);

    assert!(sprites.mix_sprite_to_sprite(dst, 1, 0, src, 1, 0, 1, 1));
    let dst_surface = sprites.surface(SpriteSurfaceId(21)).unwrap();
    assert_eq!(&dst_surface.pixels[4..8], &[152, 162, 172, 230]);

    assert!(sprites.mask_alpha(dst, 0, 0, 1, 1, mask, 0, 0));
    let dst_surface = sprites.surface(SpriteSurfaceId(21)).unwrap();
    assert_eq!(dst_surface.pixels[3], 20);
}

#[test]
fn set_buffer_and_paint_update_sprite_surface_generation() {
    let mut sprites = make_system();
    let sprite = sprites.create(SpriteDesc::new(SceneTextureId(10), 64, 32));
    let before = sprites.surface(SpriteSurfaceId(10)).unwrap().generation;

    assert!(sprites.set_buffer(sprite, 1, 1, 1, 1, &[1, 2, 3, 4]));
    let surface = sprites.surface(SpriteSurfaceId(10)).unwrap();
    let idx = ((64 + 1) * 4) as usize;
    assert_eq!(&surface.pixels[idx..idx + 4], &[1, 2, 3, 4]);
    assert!(surface.generation > before);

    assert!(sprites.paint(sprite, 0, 0, PalColor::from_argb(0x8040_2010)));
    let surface = sprites.surface(SpriteSurfaceId(10)).unwrap();
    assert_eq!(&surface.pixels[0..4], &[0x40, 0x20, 0x10, 0x80]);
}

#[test]
fn sprite_transition_handle_marks_and_cancels_participating_sprites() {
    let mut sprites = make_system();
    let from = sprites.create(SpriteDesc {
        texture_id: SceneTextureId(10),
        texture_width: 64,
        texture_height: 32,
        visible: true,
        ..SpriteDesc::new(SceneTextureId(10), 64, 32)
    });
    let to = sprites.create(SpriteDesc {
        texture_id: SceneTextureId(11),
        texture_width: 64,
        texture_height: 32,
        visible: true,
        ..SpriteDesc::new(SceneTextureId(11), 64, 32)
    });
    let transition = sprites.create_transition_handle();

    assert_eq!(sprites.transition_state(transition), 0);
    assert!(sprites.set_transition(transition, 1, Some(to), Some(from), 1, 1000, 500));
    assert_eq!(sprites.transition_state(transition), 1);
    assert_eq!(sprites.get(from).unwrap().transition_block, 1);
    assert_eq!(sprites.get(to).unwrap().transition_block, 1);
    assert!(sprites.commands().is_empty());
    assert_eq!(sprites.transition_commands().len(), 2);

    sprites.advance_transitions(1250, false);
    let commands = sprites.transition_commands();
    assert_eq!(commands.len(), 2);
    let DrawCommand::Sprite(to_draw) = &commands[1] else {
        panic!("transition target must be drawn as sprite");
    };
    assert_eq!(to_draw.color[3], 127.0 / 255.0);

    sprites.advance_transitions(1500, false);
    assert_eq!(sprites.transition_state(transition), 2);
    assert!(sprites.get(from).is_some());
    assert!(sprites.commands().is_empty());
    assert!(sprites.cancel_transition(transition));
    assert_eq!(sprites.transition_state(transition), 0);
    assert!(sprites.get(from).is_none());
    assert_eq!(sprites.get(to).unwrap().transition_block, 0);
    assert_eq!(sprites.commands().len(), 1);
    assert!(!sprites.cancel_transition(transition));
    assert!(sprites.release_transition_handle(transition));
    assert_eq!(sprites.transition_state(transition), 3);
}

#[test]
fn screen_copy_sprite_is_released_only_when_transition_is_cancelled() {
    let mut sprites = SpriteSystem::new();
    let backbuffer = sprites
        .create_rgba_sprite(
            4,
            4,
            vec![0; 4 * 4 * 4],
            PalVec3::default(),
            0,
            "screen-copy:backbuffer",
        )
        .expect("backbuffer sprite");
    assert!(sprites.view_ctrl(backbuffer, true));
    let transition = sprites.create_transition_handle();

    assert!(sprites.set_transition(transition, 1, None, Some(backbuffer), 1, 0, 16));
    assert_eq!(sprites.transition_commands().len(), 1);
    sprites.advance_transitions(16, false);

    assert_eq!(sprites.transition_state(transition), 2);
    assert!(sprites.get(backbuffer).unwrap().visible);
    assert!(sprites.commands().is_empty());
    assert!(sprites.cancel_transition(transition));
    assert!(sprites.get(backbuffer).is_none());
}

#[test]
fn sprite_transition_uses_wrapping_absolute_clock_and_zero_duration() {
    let mut sprites = make_system();
    let first = sprites.create(SpriteDesc::new(SceneTextureId(10), 64, 32));
    let transition = sprites.create_transition_handle();
    assert!(sprites.set_transition(transition, 1, Some(first), None, 1, u32::MAX - 15, 32));
    sprites.advance_transitions(0, false);
    assert_eq!(sprites.transition_state(transition), 1);
    sprites.advance_transitions(16, false);
    assert_eq!(sprites.transition_state(transition), 2);
    assert!(sprites.set_transition(transition, 1, Some(first), None, 1, 100, 0));
    sprites.advance_transitions(100, false);
    assert_eq!(sprites.transition_state(transition), 2);
    assert!(sprites.get(first).is_some());
    assert!(sprites.release_transition_handle(transition));
    assert!(sprites.get(first).is_some());
}

#[test]
fn unsupported_transition_and_both_null_preserve_native_state() {
    let mut sprites = make_system();
    let first = sprites.create(SpriteDesc::new(SceneTextureId(10), 64, 32));
    let transition = sprites.create_transition_handle();
    assert!(sprites.set_transition(transition, 1, Some(first), None, -1, 0, 1));
    sprites.advance_transitions(100, true);
    assert_eq!(sprites.transition_state(transition), 1);
    assert!(sprites.transition_commands().is_empty());
    assert_eq!(sprites.get(first).unwrap().transition_block, u32::MAX);
    assert!(!sprites.set_transition(transition, 1, None, None, 1, 0, 0));
    assert_eq!(sprites.transition_state(transition), 1);
    sprites.advance_transitions(100, true);
    assert_eq!(sprites.transition_state(transition), 1);
    assert_eq!(sprites.get(first).unwrap().transition_block, 0);
    assert!(sprites.cancel_transition(transition));
}

#[test]
fn sprite_transition_skip_keeps_second_owned_until_cancellation() {
    let mut sprites = make_system();
    let first = sprites.create(SpriteDesc::new(SceneTextureId(10), 64, 32));
    let second = sprites.create(SpriteDesc::new(SceneTextureId(11), 64, 32));
    let transition = sprites.create_transition_handle();
    assert!(sprites.set_transition(transition, 1, Some(first), Some(second), 1, 100, 1000));
    sprites.advance_transitions(101, true);
    assert_eq!(sprites.transition_state(transition), 2);
    assert!(sprites.get(second).is_some());
    assert!(sprites.release_transition_handle(transition));
    assert!(sprites.get(first).is_some());
    assert!(sprites.get(second).is_none());
}

#[test]
fn sprite_fx_effect_state_tracks_set_update_and_release() {
    let mut sprites = make_system();
    let sprite = sprites.create(SpriteDesc::new(SceneTextureId(10), 64, 32));

    assert_eq!(sprites.sprite_fx_effect_state(sprite), 0);
    assert!(!sprites.set_sprite_fx_effect(sprite, 4, 1, 100));
    assert!(sprites.set_sprite_fx_effect(sprite, 0, 1, 100));
    assert_eq!(sprites.sprite_fx_effect_state(sprite), 1);
    assert!(!sprites.set_sprite_fx_effect(sprite, 0, 1, 100));
    assert!(sprites.set_sprite_fx_effect(sprite, 0, 2, 200));
    assert_eq!(sprites.sprite_fx_effect_state(sprite), 2);
    sprites.tick_fx_effects(199);
    assert_eq!(sprites.sprite_fx_effect_state(sprite), 2);
    sprites.tick_fx_effects(200);
    assert_eq!(sprites.sprite_fx_effect_state(sprite), 0);
    assert!(sprites.release_sprite_fx_effect(sprite));
    assert_eq!(sprites.sprite_fx_effect_state(sprite), 0);
}

#[test]
fn named_ani_motion_lanes_share_one_motion_entry_and_clear_on_completion() {
    let mut sprites = make_system();
    let sprite = sprites.create(SpriteDesc {
        texture_id: SceneTextureId(10),
        texture_width: 64,
        texture_height: 32,
        visible: true,
        ..SpriteDesc::new(SceneTextureId(10), 64, 32)
    });

    assert!(sprites.tween_pos_by(sprite, 30.0, 12.0, 3.0, 100));
    assert!(sprites.tween_scale_to(sprite, 2.0, 100));
    assert!(sprites.tween_alpha_to(sprite, 64, 100));
    assert_eq!(sprites.motion_entry_count(), 1);

    sprites.advance_motion_entries(50);
    let mid = sprites.get(sprite).unwrap();
    assert_eq!(mid.position, PalVec3::from_f32(15.0, 6.0, 1.5));
    assert_eq!(mid.scale, 1.5);
    assert_eq!(mid.color.alpha(), 160);
    assert_eq!(sprites.motion_entry_count(), 1);

    sprites.advance_motion_entries(50);
    let done = sprites.get(sprite).unwrap();
    assert_eq!(done.position, PalVec3::from_f32(30.0, 12.0, 3.0));
    assert_eq!(done.scale, 2.0);
    assert_eq!(done.color.alpha(), 64);
    assert_eq!(sprites.motion_entry_count(), 0);
}

#[test]
fn fx_non_start_actions_ignore_type_and_keep_the_owner() {
    let mut sprites = make_system();
    let sprite = sprites.create(SpriteDesc::new(SceneTextureId(10), 64, 32));
    assert!(sprites.set_sprite_fx_effect(sprite, u32::MAX, 0, i32::MIN));
    let owner = sprites.get(sprite).unwrap().fx_effect;
    assert!(owner.is_some());
    assert!(sprites.set_sprite_fx_effect(sprite, u32::MAX, u32::MAX, i32::MAX));
    assert_eq!(sprites.get(sprite).unwrap().fx_effect, owner);
    assert_eq!(sprites.sprite_fx_effect_state(sprite), 0);
    assert!(sprites.set_sprite_fx_effect(sprite, 0, 1, 100));
    assert!(sprites.set_sprite_fx_effect(sprite, u32::MAX, 99, 0));
    assert_eq!(sprites.sprite_fx_effect_state(sprite), 1);
    assert!(sprites.set_sprite_fx_effect(sprite, u32::MAX, 0, 0));
    assert_eq!(sprites.get(sprite).unwrap().fx_effect, owner);
    assert_eq!(sprites.sprite_fx_effect_state(sprite), 0);
    assert!(sprites.release_sprite_fx_effect(sprite));
    assert!(sprites.get(sprite).unwrap().fx_effect.is_none());
    assert!(!sprites.release_sprite_fx_effect(sprite));
}

#[test]
fn fx_stop_before_start_sets_state_and_preserves_full_duration_bits() {
    let mut sprites = make_system();
    let sprite = sprites.create(SpriteDesc::new(SceneTextureId(10), 64, 32));
    assert!(sprites.set_sprite_fx_effect(sprite, u32::MAX, 2, -1));
    assert_eq!(sprites.sprite_fx_effect_state(sprite), 2);
    assert!(!sprites.set_sprite_fx_effect(sprite, 0, 1, 0));
    sprites.tick_fx_effects(u32::MAX - 1);
    assert_eq!(sprites.sprite_fx_effect_state(sprite), 2);
    sprites.tick_fx_effects(u32::MAX);
    assert_eq!(sprites.sprite_fx_effect_state(sprite), 0);
    assert!(sprites.set_sprite_fx_effect(sprite, 0, 1, 0));
}

#[test]
fn global_fx_requires_release_after_stop_or_clear_before_another_start() {
    use pal_vm::fx::RasterSource;
    let mut sprites = make_system();
    let source = RasterSource {
        period_ms: 100,
        undulate_x: 0.5,
        undulate_y: 0.25,
    };
    assert!(sprites.start_global_raster(42, source, 0));
    sprites.tick_fx_effects(10);
    assert_eq!(sprites.global_raster_stage().unwrap().priority, 42);
    sprites.stop_global_raster(20);
    sprites.tick_fx_effects(30);
    assert_eq!(sprites.global_raster_state(), 0);
    assert!(sprites.global_raster_stage().is_none());
    assert!(!sprites.start_global_raster(43, source, 0));
    sprites.release_global_raster();
    assert!(sprites.start_global_raster(44, source, 0));
    sprites.clear_global_raster();
    assert_eq!(sprites.global_raster_state(), 0);
    assert!(!sprites.start_global_raster(45, source, 0));
    sprites.release_global_raster();
    assert!(sprites.start_global_raster(46, source, 0));
}

#[test]
fn global_fx_stop_without_render_attachment_does_not_advance() {
    let mut sprites = make_system();
    sprites.stop_global_raster(10);
    sprites.tick_fx_effects(20);
    assert_eq!(sprites.global_raster_state(), 2);
    assert!(sprites.global_raster_stage().is_none());
    assert!(!sprites.start_global_raster(1, Default::default(), 0));
    sprites.clear_global_raster();
    assert!(!sprites.start_global_raster(1, Default::default(), 0));
    sprites.release_global_raster();
    assert!(sprites.start_global_raster(1, Default::default(), 0));
}
