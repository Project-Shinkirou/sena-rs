# Save-slot queries and the most recent save

Verified against `reverse/Game.sqlite` decompilations on 2026-10-23. These
addresses belong to the executable represented by that database; addresses in
older Koikake notes differ.

| Native function | Recovered behavior |
| --- | --- |
| `sub_432620` (`is_save`) | Only argument `-1` scans numbered `save%03d.dat` files for slots 0 through 999. Every other argument names one numbered file. `continue.dat` is never part of this query. |
| `sub_44D320` (initialization) | Initializes task-data dword 6, offset 24, to `-1`. |
| `sub_4312F0` (`get_new_savefile`) | Returns task-data offset 24 unchanged, including `-1`; it does not find the newest file by timestamp. |
| `sub_433D10` (`save`) | Once a savepoint is armed and the slot is in 0 through 999, stores the slot at task-data offset 24 when the second argument is nonzero. |
| `sub_431130` (`delete_file`) | Returns `DeleteFileA`'s result, then clears the remembered slot to `-1` if it matches the requested slot, even if deletion failed. Only slot `-1` names `continue.dat`. |

Koikake's title script in `docs/dis.txt` calls `is_save(998)` at `0x57CE4`
and `is_save(-1)` at `0x57D74`. The latter controls the DATA LOAD button:
a normal numbered save is sufficient, independently of slot 998. The runtime
already implemented the wildcard scan before this audit; the old continuation
note describing the scan as missing was stale. This audit corrected the broader
negative-value wildcard, the initial remembered-slot value, clamping of the
returned sentinel, and clearing the marker after a failed deletion.

`crates/pal-vm/tests/save_semantics_fixture.rs` executes constructed Sv20
instructions through the public runtime. It checks the title's two queries,
the inclusive scan bounds and exclusion of continue files, compatibility with
legacy portable snapshots, exact negative-slot behavior, and the most-recent
slot lifecycle. All test saves live in temporary directories. These tests
verify VM behavior; they do not claim a new full-game visual title-screen test.

Run with `cargo test -p pal-vm --test save_semantics_fixture`.
