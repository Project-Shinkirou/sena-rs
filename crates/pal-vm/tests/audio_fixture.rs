use pal_vm::{AudioConfig, AudioHandle, AudioSystem, PalSoundGroup, PalSoundStatus, PalVolume};

#[test]
fn audio_handle_preserves_original_prefixes() {
    for group in PalSoundGroup::ALL {
        let handle = AudioHandle::new(group, 3);
        assert_eq!(AudioHandle::from_raw(handle.raw()), Some(handle));
    }
}

#[test]
fn pal_volume_preserves_native_word_and_explicit_clamping() {
    assert_eq!(PalVolume::from_raw(-1).raw(), -1);
    assert_eq!(PalVolume::from_raw(10001).raw(), 10001);
    assert!(PalVolume::from_raw(5000).to_decibels() < 0.0);
}

#[test]
fn audio_global_and_unloaded_channel_state_match_pal_failure_shape() {
    let mut audio = AudioSystem::new(AudioConfig { enabled: false }).unwrap();
    let handle = AudioHandle::new(PalSoundGroup::GROUP1, 0);
    assert_eq!(audio.sound_status(handle).unwrap(), PalSoundStatus::Free);
    assert!(audio.set_start_end(handle, 10, 20).is_err());
    assert!(audio.set_channel_pan(handle, -1000).is_err());
    assert!(audio.set_channel_frequency(handle, 44_100).is_err());

    audio.set_primary_volume(PalVolume::from_raw(7500)).unwrap();
    assert_eq!(audio.primary_volume().raw(), 7500);
    audio
        .set_group_volume(PalSoundGroup::GROUP1, PalVolume::from_raw(6400))
        .unwrap();
    assert_eq!(audio.group_volume(PalSoundGroup::GROUP1).raw(), 6400);
    assert_eq!(audio.free_channel_count(PalSoundGroup::GROUP1), 16);
    assert_eq!(audio.sound_param(0, 99, true).unwrap(), 0);
}
