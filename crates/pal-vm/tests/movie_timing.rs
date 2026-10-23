use pal_vm::MSpriteSystem;

// Synthetic 32x32, five-frame, 25fps movies generated from FFmpeg testsrc2.
// Runtime playback uses the embedded decoders; FFmpeg is not needed by tests.
const MPEG: &[u8] = include_bytes!("fixtures/timing_25fps.mpg");
const WMV: &[u8] = include_bytes!("fixtures/timing_25fps.wmv");

fn check_timing(bytes: &[u8]) {
    let mut system = MSpriteSystem::new();
    let loaded = system.load_movie("timing", bytes.to_vec()).unwrap();
    assert_eq!((loaded.width, loaded.height), (32, 32));
    system.play(loaded.handle, 0);
    system.start_movie("timing", 0, loaded.handle);
    assert!(system.advance(0).is_empty());
    assert!(system.advance(16).is_empty());
    assert!(system.advance(16).is_empty());
    assert_eq!(system.advance(8).len(), 1);
    assert!(system.advance(39).is_empty());
    assert_eq!(system.advance(1).len(), 1);
    assert_eq!(system.movie().unwrap().elapsed_ms, 80);
    assert_eq!(system.advance(80).len(), 1);
    assert!(system.is_movie());
    assert!(system.advance(39).is_empty());
    assert!(system.is_movie());
    assert!(system.advance(1).is_empty());
    assert!(!system.is_movie());
}

#[test]
fn mpeg_movie_uses_elapsed_time_instead_of_render_frame_count() {
    check_timing(MPEG);
}

#[test]
fn wmv_movie_uses_elapsed_time_instead_of_render_frame_count() {
    check_timing(WMV);
}

fn check_loop(bytes: &[u8]) {
    let mut system = MSpriteSystem::new();
    let loaded = system.load_movie("timing", bytes.to_vec()).unwrap();
    system.play(loaded.handle, 1);
    system.start_movie("timing", 0, loaded.handle);
    assert_eq!(system.advance(240).len(), 1);
    assert_eq!(system.movie().unwrap().elapsed_ms, 40);
    assert!(system.is_movie());
    assert!(system.advance(39).is_empty());
    assert_eq!(system.advance(1).len(), 1);
    system.set_loop_point(loaded.handle, 50, 100);
    // Updating stored loop points does not replace the currently running
    // graph segment. The new range is installed at its next EOF seek.
    assert!(system.advance(20).is_empty());
    assert_eq!(system.movie().unwrap().elapsed_ms, 100);
    assert_eq!(system.advance(100).len(), 1);
    assert_eq!(system.movie().unwrap().elapsed_ms, 50);
    // A restart between frames keeps the previous frame until 80ms is due.
    assert!(system.advance(29).is_empty());
    assert_eq!(system.advance(1).len(), 1);
}

#[test]
fn mpeg_movie_restarts_with_only_the_elapsed_remainder() {
    check_loop(MPEG);
}

#[test]
fn wmv_movie_restarts_with_only_the_elapsed_remainder() {
    check_loop(WMV);
}

fn check_single_picture(bytes: &[u8]) {
    let mut system = MSpriteSystem::new();
    let loaded = system.load_movie("single", bytes.to_vec()).unwrap();
    system.play(loaded.handle, 0);
    system.start_movie("single", 0, loaded.handle);
    assert!(system.advance(1).is_empty());
    assert!(
        system.is_movie(),
        "a one-picture movie must not finish after 1 ms"
    );
    assert!(system.advance(38).is_empty());
    assert!(system.is_movie());
    assert!(system.advance(1).is_empty());
    assert!(!system.is_movie());
    let mut system = MSpriteSystem::new();
    let loaded = system.load_movie("single-loop", bytes.to_vec()).unwrap();
    system.play(loaded.handle, 1);
    system.start_movie("single-loop", 0, loaded.handle);
    assert_eq!(system.advance(1_019).len(), 1);
    assert_eq!(system.movie().unwrap().elapsed_ms, 19);
    assert!(system.is_movie());
}

#[test]
fn mpeg_single_picture_retains_sequence_frame_duration() {
    check_single_picture(include_bytes!("fixtures/timing_mpeg2_single.mpg"));
}

#[test]
fn wmv_single_picture_retains_container_duration() {
    check_single_picture(include_bytes!("fixtures/timing_wmv_single.wmv"));
}

#[test]
fn asf_audio_keeps_remaining_media_objects_when_a_frame_is_returned() {
    let bytes = include_bytes!("fixtures/timing_audio.wma");
    let mut decoder = wmv_decoder::AsfWmaDecoder::open(std::io::Cursor::new(bytes)).unwrap();
    let mut timestamps = Vec::new();
    while let Some(frame) = decoder.next_frame().unwrap() {
        timestamps.push(frame.pts_ms);
    }
    timestamps.dedup(); // The decoder may also flush a buffered tail at EOF.
    assert_eq!(timestamps, [0, 46, 92, 139, 185]);
}
