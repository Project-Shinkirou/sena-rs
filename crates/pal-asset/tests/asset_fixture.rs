use std::fs;
use std::path::{Path, PathBuf};
use std::time::{SystemTime, UNIX_EPOCH};

use pal_asset::{make_pac_key, parse_archive_dat_paths, parse_archive_dat_profile, Nls, PacArchive, ResourceManager};

#[test]
fn archive_dat_parser_matches_pal_delimiters() {
    let bytes = b" data | bg\r\n| fgimage\t |";
    let paths = parse_archive_dat_paths(bytes, Nls::ShiftJis).unwrap();
    assert_eq!(paths, vec!["data", "bg", "fgimage"]);
}

#[test]
fn archive_profiles_are_selected_without_merging_or_splitting_dbcs_trail_bytes() {
    let bytes = b"base|debug,\r\nbase|trial,base|\x83\x7c\0ignored";
    assert_eq!(parse_archive_dat_paths(bytes, Nls::ShiftJis).unwrap(), ["base", "debug"]);
    assert_eq!(parse_archive_dat_profile(bytes, Nls::ShiftJis, 1).unwrap(), ["base", "trial"]);
    assert_eq!(parse_archive_dat_profile(bytes, Nls::ShiftJis, 2).unwrap(), ["base", "\u{30dd}"]);
    assert!(parse_archive_dat_profile(bytes, Nls::ShiftJis, 3).is_err());
}

#[test]
fn switching_archive_profiles_replaces_search_paths_and_cached_archives() {
    let dir = temp_dir("pal_asset_profiles");
    let archive_key = make_pac_key("ARCHIVE.DAT", Nls::ShiftJis).unwrap();
    let image_key = make_pac_key("shared.pgd", Nls::ShiftJis).unwrap();
    fs::write(dir.join("data.pac"), make_one_entry_pac(archive_key, b"retail,trial")).unwrap();
    fs::write(dir.join("retail.pac"), make_one_entry_pac(image_key, b"retail image")).unwrap();
    fs::write(dir.join("trial.pac"), make_one_entry_pac(image_key, b"trial image")).unwrap();
    let mut manager = ResourceManager::bootstrap(&dir, Nls::ShiftJis).unwrap();
    assert_eq!(manager.open("shared.pgd").unwrap().bytes, b"retail image");
    manager.select_archive_profile(1).unwrap();
    assert_eq!(manager.paths(), ["data", "trial"]);
    assert_eq!(manager.open("shared.pgd").unwrap().bytes, b"trial image");
    assert!(manager.select_archive_profile(7).is_err());
    assert_eq!(manager.paths(), ["data"]);
    assert!(manager.open("shared.pgd").is_err());
    fs::remove_dir_all(dir).unwrap();
}

#[test]
fn archive_list_can_bootstrap_from_csv_archive() {
    let dir = temp_dir("pal_asset_csv_archive");
    let key = make_pac_key("ARCHIVE.DAT", Nls::ShiftJis).unwrap();
    fs::write(dir.join("csv.pac"), make_one_entry_pac(key, b"csv|system")).unwrap();
    let manager = ResourceManager::bootstrap(&dir, Nls::ShiftJis).unwrap();
    assert_eq!(manager.paths(), ["data", "csv", "system"]);
    fs::remove_dir_all(dir).unwrap();
}

#[test]
fn pac_archive_reads_bucketed_entry() {
    let dir = temp_dir("pal_asset_pac");
    let pac_path = dir.join("data.pac");
    let key = make_pac_key("script.src", Nls::ShiftJis).unwrap();
    fs::write(&pac_path, make_one_entry_pac(key, b"Sv20fixture")).unwrap();

    let pac = PacArchive::from_file(&pac_path).unwrap();
    let data = pac.read_key(&key).unwrap().unwrap();
    assert_eq!(data, b"Sv20fixture");

    fs::remove_dir_all(dir).unwrap();
}

#[test]
fn resource_manager_bootstraps_archive_dat_and_opens_from_pac() {
    let dir = temp_dir("pal_asset_resource");
    fs::create_dir_all(dir.join("data")).unwrap();
    fs::write(dir.join("data").join("archive.dat"), b"data|bg|").unwrap();

    let key = make_pac_key("script.src", Nls::ShiftJis).unwrap();
    fs::write(dir.join("data.pac"), make_one_entry_pac(key, b"Sv20script")).unwrap();

    let mut manager = ResourceManager::bootstrap(&dir, Nls::ShiftJis).unwrap();
    let asset = manager.open("script.src").unwrap();
    assert_eq!(asset.bytes, b"Sv20script");
    assert_eq!(manager.paths(), &["data".to_string(), "bg".to_string()]);

    fs::remove_dir_all(dir).unwrap();
}

#[test]
fn resource_manager_bootstraps_archive_dat_stored_in_data_pac() {
    let dir = temp_dir("pal_asset_archive_in_pac");
    let archive_key = make_pac_key("ARCHIVE.DAT", Nls::ShiftJis).unwrap();
    let script_key = make_pac_key("script.src", Nls::ShiftJis).unwrap();
    fs::write(
        dir.join("data.pac"),
        make_two_entry_pac(
            archive_key,
            b"movie|bgm|mask|em|ev|ev2|etc|bk|se|system|face|voice|st|st2",
            script_key,
            b"Sv20script",
        ),
    )
    .unwrap();

    let mut manager = ResourceManager::bootstrap(&dir, Nls::ShiftJis).unwrap();
    assert_eq!(
        manager.paths(),
        &[
            "data", "movie", "bgm", "mask", "em", "ev", "ev2", "etc", "bk", "se", "system",
            "face", "voice", "st", "st2",
        ]
        .map(str::to_string)
    );
    let asset = manager.open("script.src").unwrap();
    assert_eq!(asset.bytes, b"Sv20script");
    assert!(matches!(asset.source, pal_asset::AssetSource::Pac { .. }));

    fs::remove_dir_all(dir).unwrap();
}

/// The localized FHD releases name their translated archives in a second list,
/// `archive_cn.dat` / `archive_tc.dat`.  Those archives must be searched before
/// the base ones so a translated screen shadows the original one.
#[test]
fn resource_manager_applies_language_archive_list_ahead_of_base() {
    let dir = temp_dir("pal_asset_language_list");
    let archive_key = make_pac_key("ARCHIVE.DAT", Nls::ShiftJis).unwrap();
    let language_key = make_pac_key("ARCHIVE_CN.DAT", Nls::ShiftJis).unwrap();
    let script_key = make_pac_key("script.src", Nls::ShiftJis).unwrap();
    fs::write(
        dir.join("data.pac"),
        make_entries_pac(&[
            (archive_key, b"data|system|etc"),
            (language_key, b"system_cn|etc_cn|movie_cn|update_cn"),
            (script_key, b"Sv20script"),
        ]),
    )
    .unwrap();

    let mut manager =
        ResourceManager::bootstrap_with_language(&dir, Nls::ShiftJis, Some("archive_cn.dat"))
            .unwrap();
    assert_eq!(
        manager.paths(),
        &[
            "data", "system_cn", "etc_cn", "movie_cn", "update_cn", "system", "etc",
        ]
        .map(str::to_string)
    );
    let asset = manager.open("script.src").unwrap();
    assert_eq!(asset.bytes, b"Sv20script");

    fs::remove_dir_all(dir).unwrap();
}

/// A single-language install has no second list, and that stays non-fatal.
#[test]
fn resource_manager_ignores_missing_language_archive_list() {
    let dir = temp_dir("pal_asset_no_language_list");
    fs::create_dir_all(dir.join("data")).unwrap();
    fs::write(dir.join("data").join("archive.dat"), b"data|bg|").unwrap();

    let plain = ResourceManager::bootstrap(&dir, Nls::ShiftJis).unwrap();
    let missing =
        ResourceManager::bootstrap_with_language(&dir, Nls::ShiftJis, Some("archive_cn.dat"))
            .unwrap();
    assert_eq!(missing.paths(), plain.paths());

    fs::remove_dir_all(dir).unwrap();
}

#[test]
fn registered_paths_use_lifo_precedence_and_preserve_language_overrides() {
    let dir = temp_dir("pal_asset_lifo");
    for path in ["data", "base", "trial", "localized"] {
        fs::create_dir_all(dir.join(path)).unwrap();
    }
    fs::write(dir.join("data/archive.dat"), b"base,base|trial").unwrap();
    fs::write(dir.join("data/archive_cn.dat"), b"localized").unwrap();
    for path in ["data", "base", "trial", "localized"] {
        fs::write(dir.join(path).join("shared.txt"), path.as_bytes()).unwrap();
    }
    let mut manager = ResourceManager::bootstrap_with_path_order(&dir, Nls::ShiftJis, None, true).unwrap();
    assert_eq!(manager.open("shared.txt").unwrap().bytes, b"base");
    manager.select_archive_profile(1).unwrap();
    assert_eq!(manager.open("shared.txt").unwrap().bytes, b"trial");
    manager.select_archive_profile(0).unwrap();
    assert_eq!(manager.open("shared.txt").unwrap().bytes, b"base");
    let mut localized = ResourceManager::bootstrap_with_path_order(&dir, Nls::ShiftJis, Some("archive_cn.dat"), true).unwrap();
    localized.select_archive_profile(1).unwrap();
    assert_eq!(localized.open("shared.txt").unwrap().bytes, b"localized");
    fs::remove_dir_all(dir).unwrap();
}

fn make_one_entry_pac(key: [u8; 32], payload: &[u8]) -> Vec<u8> {
    make_entries_pac(&[(key, payload)])
}

fn make_two_entry_pac(
    key_a: [u8; 32],
    payload_a: &[u8],
    key_b: [u8; 32],
    payload_b: &[u8],
) -> Vec<u8> {
    make_entries_pac(&[(key_a, payload_a), (key_b, payload_b)])
}

fn make_entries_pac(entries: &[([u8; 32], &[u8])]) -> Vec<u8> {
    const TABLE_OFF: usize = 0x0C;
    const TABLE_SIZE: usize = 255 * 8;
    const RECORD_BASE: usize = TABLE_OFF + TABLE_SIZE;
    const RECORD_SIZE: usize = 40;

    let mut by_bucket: Vec<Vec<([u8; 32], &[u8])>> = vec![Vec::new(); 255];
    for entry in entries {
        by_bucket[entry.0[0] as usize].push(*entry);
    }

    let record_count: usize = entries.len();
    let data_base = RECORD_BASE + record_count * RECORD_SIZE;
    let payload_len: usize = entries.iter().map(|(_, payload)| payload.len()).sum();
    let mut bytes = vec![0u8; data_base + payload_len];

    let mut record_index = 0u32;
    let mut data_cursor = data_base;
    for (bucket, bucket_entries) in by_bucket.iter().enumerate() {
        if bucket_entries.is_empty() {
            continue;
        }
        let bucket_off = TABLE_OFF + bucket * 8;
        bytes[bucket_off..bucket_off + 4].copy_from_slice(&record_index.to_le_bytes());
        bytes[bucket_off + 4..bucket_off + 8]
            .copy_from_slice(&(bucket_entries.len() as u32).to_le_bytes());
        for (key, payload) in bucket_entries {
            let record_off = RECORD_BASE + record_index as usize * RECORD_SIZE;
            bytes[record_off..record_off + 32].copy_from_slice(key);
            bytes[record_off + 32..record_off + 36]
                .copy_from_slice(&(payload.len() as u32).to_le_bytes());
            bytes[record_off + 36..record_off + 40]
                .copy_from_slice(&(data_cursor as u32).to_le_bytes());
            bytes[data_cursor..data_cursor + payload.len()].copy_from_slice(payload);
            data_cursor += payload.len();
            record_index += 1;
        }
    }
    bytes
}

fn temp_dir(prefix: &str) -> PathBuf {
    let nonce = SystemTime::now()
        .duration_since(UNIX_EPOCH)
        .unwrap()
        .as_nanos();
    let path = std::env::temp_dir().join(format!("{}_{}", prefix, nonce));
    fs::create_dir_all(&path).unwrap();
    path
}
