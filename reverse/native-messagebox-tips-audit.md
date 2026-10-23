# Native messagebox and Tips audit (2026-10-23)

Evidence: `reverse/Game.sqlite`, table `decompilations`, and the existing complete instruction walk in `out/extcall_report.json`.

The report contains 67,196 extcalls. None has a name containing `tips`, and none uses category 2 indexes 38 through 46 (the nine text Tips commands). This establishes absence in this decoded Koikake script, not absence in other PAL games or all releases. The Tips command/data layer is now implemented as described below, but the text-layout producer and callbacks remain missing; it is not a demonstrated blocker for this script.

The native Tips handlers are available for further implementation:

| Address | Command | Observed behavior |
| --- | --- | --- |
| `0x441A70` | tips_get_str | String extraction; inspect full destination/index contract before porting. |
| `0x441BD0` | tips_get_param | Parameter extraction; inspect full selector contract before porting. |
| `0x441DC0` | tips_reset | Clears current and first 0x182C-byte record; count becomes 0; mode word becomes 1. |
| `0x441E20` | tips_stop | Sets mode word to 256. |
| `0x441E50` | tips_search | Pops one string handle; exact strcmp search; returns last matching index or -1 through VM return slot. |
| `0x441F80` | tips_set_color | Pops two colors; first -1 disables custom colors, otherwise stores both and enables them. |
| `0x442030` | tips_get_flag | Returns the current flag byte. |
| `0x442070` | tips_init | Zeroes the complete 0x3059C-byte Tips state. |
| `0x4420B0` | tips_pause | Returns the old flag, then XORs it with 1. |

## Tips command layer follow-up

Fresh IDA inspection of category table `0x49C360` and category-2 table `0x4BB490`
confirms indexes 38–46 point to those nine functions in the order shown by the
catalog (stop is index 43). Evidence is in `target/reverse-audit/tips.json`.
The runtime implementation lives in `crates/pal-vm/src/runtime/tips.rs`.

The former stubs popped one argument for get_str/get_param/set_color/get_flag,
but their actual pop counts are **3/2/2/0**. Correcting these prevents stack
corruption even before a game uses the complete Tips presentation path.
`tips_get_str` pops `(record, destination dynamic string, selector)` and writes
one of three strings, returning the record ID. `tips_get_param` pops
`(record, selector)`: selectors 0–8 are the first nine record dwords, 9 is the
wrapped-line byte, and 10/11 are width/height plus font-effect padding 0/2/4.
Selectors 12–17 now read the stored `text_init` values: font size, body x,
body y, name x, name y, and line-pitch/mode argument, respectively (context
dwords 4/6/7/10/11/5). The exact stores in `0x441400` establish this mapping.
They return raw signed script values, without renderer projection or 1.5×
scaling; the font-size query preserves the complete dword although the native
font setter takes its low 16 bits. Later shared-font size changes and active
text-mode changes do not alter these stored values. `text_uninit` clears the
context; `text_init_visualnovelmode` uses separate context dwords 13–16.
Evidence: `reverse/evidence/runtime-audit/sub_441400.c`, `sub_4411D0.c`,
`sub_43DDB0.c`, `sub_43DEC0.c`. Unknown selectors above 17 return zero as native does. Record-reading selectors bounds-check indices instead of reproducing native
unchecked memory reads. Context selectors 12–17 do not read a record, so they
ignore its index, including negative/out-of-range values, as native does.

The record has size `0x182C`: eleven dwords, one byte, three 2047-byte strings
and tail alignment. There are 32 slots. Reset clears the current slot and slot
zero, sets count=0 and mode=1, and preserves other slots, colors and pause flag.
Stop sets mode=256 without changing pause. Disabling colors with first popped
argument -1 preserves the stored colors. Init clears all state. Search accepts
a dynamic string, compares exactly and returns the last matching slot, or -1.
Index 37 (`0x441A30`) returns `max(count-1, 0)`, not the count.

The data source is now located but **not integrated**: `0x43C7C0` handles
`<dNUMBER>...</d>` in the text layout pass. The opening tag sets record ID,
start x/y and optional colors; the closing tag stores raw enclosed text,
width/height, ending x/y and split text when the term wraps. It advances count
and sets mode=257. The native opening path wraps count 31 back to zero.
Capturing strings without the corresponding layout positions would make
`tips_get_param` misleading, so that approximation is deliberately absent.
Until the producer is connected, record queries see the initialized empty
records; this is **not a complete usable Tips UI**. Custom colors are stored,
but not yet consumed during glyph rendering. Tips state is not serialized.

Index 36 (`0x441950`) registers two point-table callbacks and clears the record
array. `0x439480` triggers one callback from interaction, and `0x43DB40`
triggers the second at text completion. These are separate from simply seeing
a tag, and are not yet implemented. The recovered functions were saved under
`target/reverse-audit/` for the next pass.

Validation: `cargo test -p pal-vm tips::tests --lib` exercises exact stack
consumption, dynamic-buffer writes, duplicate search, pause/stop/reset/init
differences, color disable preservation, effect padding and all context
selectors. The context test dispatches real `text_init`/`text_uninit` calls,
checks 1280/1920/960 logical widths, distinct coordinates, signed values,
32-bit size retention and independence from subsequent shared-font changes. These tests exercise the command layer, not native layout
or live Tips interaction.

`0x415C00` is **player_name_set_begin**, not a messagebox opcode. It pops seven arguments and calls MessageBoxA only when the supplied name is too long. Other confirmed native MessageBoxA call sites include initialization errors (`0x4053D0`), save write errors (`0x430760`), archive errors (`0x44C2F0`, `0x44C560`, `0x44C7C0`), and text/file table capacity warnings (`0x44D320`). This does not establish a script-accessible general messagebox command.

The portable FFI now connects all three existing platform callback pairs to a real fatal-frame-error notification path. Engine update errors, caught frame-update panics, and RuntimeStatus::Faulted produce a kind-0 OK dialog when a callback is registered. The pump waits for acknowledgement without running the VM, then returns terminal error status 2. It does not turn acknowledgement into script success. Without a callback it returns status 2 immediately. Request IDs are checked, duplicate/stale results are ignored, unregistering releases an outstanding wait, and Android UI-thread results are passed through a synchronized mailbox without dereferencing the engine handle. This is an error reporting implementation, not a claim that all native error sites or a script messagebox instruction have been mapped. Creation errors occur before callback registration and still return a null handle.

Validation: `cargo test -p pal-vm native_messagebox_tests --lib` covers asynchronous acknowledgement, invalid/stale IDs, repeat-step suppression, callback removal, synchronous callback reentry, missing callbacks, and null handles. Actual UIKit/Android/AppKit presentation still needs device/host verification.

## Later layout integration

The producer and callback status above is superseded by [tips-layout-port.md](tips-layout-port.md). Record production and parked-wait callbacks now work with explicit remaining native layout and scheduling differences.
