use pal_vm::{
    PalGesture, PalRandomState, PalRectI, PalSystemPathKind, PalSystemState, PalWindowRequest,
};

#[test]
fn pal_random_seed_ring_matches_set_get_and_wrap_semantics() {
    let mut random = PalRandomState::new(3);
    assert_eq!(random.set_seed(&[10, 20, 30, 40]), 2);
    let mut out = [0; 4];
    assert_eq!(random.get_seed(&mut out), 3);
    assert_eq!(out, [10, 20, 30, 0]);
    assert_eq!(random.random_ex(), 10);
    assert_eq!(random.random_ex(), 20);
    assert_eq!(random.random_ex(), 30);
    assert_eq!(random.random_ex(), 10);
    assert_eq!(random.cursor(), 1);
}

#[test]
fn pal_system_language_window_touch_and_gesture_state_are_field_like() {
    let mut state = PalSystemState::new();
    assert_eq!(state.language(), 1);
    assert_eq!(state.set_language(2), 1);
    assert_eq!(state.language(), 2);

    assert_eq!(state.set_window_change_enabled(0), 1);
    assert_eq!(state.window_change_enabled(), 0);
    assert_eq!(state.change_window_mode(1), 1);
    assert!(state.take_window_requests().is_empty());
    assert_eq!(state.window_mode(), 0);
    assert_eq!(state.change_aspect_mode(3), 1);
    assert_eq!(state.aspect_mode(), 3);

    state.set_touch_enabled(true);
    assert!(state.touch_enabled());
    assert!(state.is_touch());
    assert_eq!(state.set_touch_mode(1), 1);
    assert_eq!(state.touch_mode(), 1);

    assert!(!state.gesture_check());
    assert!(state.set_gesture(
        2,
        PalGesture {
            kind: 5,
            x: 12,
            y: 34
        }
    ));
    assert!(state.gesture_check());
    assert_eq!(state.gestures()[2].kind, 5);
}

#[test]
fn pal_system_window_size_position_and_cursor_conversion_follow_pal_modes() {
    let mut state = PalSystemState::new();
    state.set_logical_size(1280, 720);
    state.set_window_pos(100, 50);
    state.set_window_size(640, 360);
    state.take_window_requests();
    assert_eq!(state.window_pos(), (100, 50));
    assert_eq!(state.window_size(), (640, 360));
    assert_eq!(state.cursor_device_pos(640, 360), (420, 230));

    state.set_fullscreen_content_rect(PalRectI::new(10, 20, 1290, 740));
    state.change_window_mode(1);
    state.take_window_requests();
    assert_eq!(state.window_pos(), (10, 20));
    assert_eq!(state.window_size(), (1280, 720));
    assert_eq!(state.cursor_device_pos(640, 360), (650, 380));
}

#[test]
fn pal_system_paths_are_cross_platform_path_lists() {
    let mut state = PalSystemState::new();
    assert!(state.push_system_path(PalSystemPathKind::Normal, "/game", "sys"));
    assert!(state.push_system_path(
        PalSystemPathKind::ChineseSimplified,
        "/game",
        "/override/cn"
    ));
    assert_eq!(
        state.system_paths(PalSystemPathKind::Normal)[0],
        std::path::PathBuf::from("/game/sys")
    );
    assert_eq!(
        state.system_paths(PalSystemPathKind::ChineseSimplified)[0],
        std::path::PathBuf::from("/override/cn")
    );
    assert!(state.clear_system_paths());
    assert!(state.system_paths(PalSystemPathKind::Normal).is_empty());
}

#[test]
fn pal_window_ops_record_platform_neutral_requests() {
    let mut state = PalSystemState::new();
    state.set_window_pos(12, 34);
    state.set_window_size(800, 600);
    state.change_window_mode(1);
    state.change_aspect_mode(3);
    assert_eq!(
        state.take_window_requests(),
        vec![
            PalWindowRequest::SetPos { x: 12, y: 34 },
            PalWindowRequest::ChangeSize {
                width: 800,
                height: 450
            },
            PalWindowRequest::ChangeMode { mode: 1 },
            PalWindowRequest::ChangeAspect { mode: 3 },
        ]
    );
    assert!(state.take_window_requests().is_empty());
}

#[test]
fn host_geometry_updates_do_not_create_resize_feedback() {
    let mut state = PalSystemState::new();
    state.observe_window_geometry(20, 40, 960, 540, false);
    assert_eq!(state.window_size(), (960, 540));
    assert_eq!(state.window_pos(), (20, 40));
    assert!(state.take_window_requests().is_empty());
    state.set_window_size(800, 700);
    assert_eq!(
        state.take_window_requests(),
        vec![PalWindowRequest::ChangeSize {
            width: 800,
            height: 450
        }]
    );
    // The compositor may constrain the requested client dimensions.
    state.observe_window_geometry(20, 40, 790, 444, false);
    assert_eq!(state.window_size(), (790, 444));
    assert!(state.take_window_requests().is_empty());
}

#[test]
fn mode_enable_is_checked_when_host_consumes_and_fullscreen_ignores_size() {
    let mut state = PalSystemState::new();
    state.observe_window_geometry(20, 40, 960, 540, false);
    state.change_window_mode(1);
    assert_eq!(state.window_mode(), 0, "mode changes are asynchronous");
    state.set_window_change_enabled(0);
    assert!(state.take_window_requests().is_empty());
    assert_eq!(state.window_mode(), 0);
    state.set_window_change_enabled(1);
    state.change_window_mode(1);
    assert_eq!(
        state.take_window_requests(),
        vec![PalWindowRequest::ChangeMode { mode: 1 }]
    );
    state.observe_window_geometry(0, 0, 1920, 1080, true);
    state.set_window_size(640, 480);
    assert!(state.take_window_requests().is_empty());
    assert_eq!(state.window_size(), (1920, 1080));
    state.change_window_mode(0);
    state.take_window_requests();
    assert_eq!(
        state.window_size(),
        (960, 540),
        "fullscreen preserves windowed geometry"
    );
}

#[test]
fn pal_random_uses_the_recovered_windows_crt_sequence_and_range() {
    // Totsulover _rand at 0x100D5F6F multiplies by 214013, adds 2531011,
    // then returns bits 16..30. Reset fills the seed ring from that stream.
    let mut random = PalRandomState::new(3);
    let mut seeds = [0; 3];
    assert_eq!(random.get_seed(&mut seeds), 3);
    assert_eq!(seeds, [41, 18467, 6334]);
    assert_eq!(random.random(false), 26500);
    assert_eq!(random.random(false), 19169);
    for _ in 0..10_000 {
        assert!((0..=32767).contains(&random.random(false)));
    }
}
