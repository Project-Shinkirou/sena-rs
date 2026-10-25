use pal_vm::MSpriteSystem;

const MPEG2: &[u8] = include_bytes!("fixtures/timing_mpeg2_bframes.mpg");

#[test]
fn mpeg2_b_frames_are_presented_in_order_through_end_of_stream() {
    let mut system = MSpriteSystem::new();
    let movie = system.load_movie("mpeg2", MPEG2.to_vec()).unwrap();
    assert_eq!((movie.width, movie.height), (32, 32));
    system.play(movie.handle, 0);
    system.start_movie("mpeg2", 0, movie.handle);
    for frame in 1..10 {
        assert!(system.advance(39).is_empty(), "early frame {frame}");
        assert_eq!(system.advance(1).len(), 1, "missing frame {frame}");
        assert_eq!(system.movie().unwrap().elapsed_ms, frame * 40);
    }
    assert!(system.is_movie());
    assert!(system.advance(39).is_empty());
    assert!(system.is_movie());
    assert!(system.advance(1).is_empty());
    assert!(!system.is_movie());
}

#[test]
fn mpeg2_loop_preroll_retains_preceding_frame() {
    let mut system = MSpriteSystem::new();
    let movie = system.load_movie("mpeg2", MPEG2.to_vec()).unwrap();
    system.play(movie.handle, 1);
    system.start_movie("mpeg2", 0, movie.handle);
    system.set_loop_point(movie.handle, 50, 150);
    // The initial pass reaches the full stream end before loop points apply.
    assert_eq!(system.advance(150).len(), 1);
    assert_eq!(system.movie().unwrap().elapsed_ms, 150);
    assert_eq!(system.advance(250).len(), 1);
    assert_eq!(system.movie().unwrap().elapsed_ms, 50);
    assert!(system.advance(29).is_empty());
    assert_eq!(system.advance(1).len(), 1);
    assert!(system.is_movie());
}

#[test]
fn mpeg2_fractional_cadence_does_not_truncate_the_final_picture() {
    let mut system = MSpriteSystem::new();
    let loaded = system
        .load_movie(
            "fractional",
            include_bytes!("fixtures/timing_mpeg2_23976.mpg").to_vec(),
        )
        .unwrap();
    system.play(loaded.handle, 0);
    system.start_movie("fractional", 0, loaded.handle);
    let mut previous = 0;
    for index in 1..8 {
        let timestamp = index * 1001 / 24;
        assert!(
            system.advance(timestamp - previous - 1).is_empty(),
            "early picture {index}"
        );
        assert_eq!(system.advance(1).len(), 1, "missing picture {index}");
        previous = timestamp;
    }
    assert!(system.advance(332 - previous).is_empty());
    assert!(system.is_movie());
    assert!(system.advance(1).is_empty());
    assert!(!system.is_movie());
}
