"""Generate platform/lifecycle contracts from the explicit reviewed native table."""

import json
from collections import Counter
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
DIRECTORY = Path(__file__).resolve().parent
BUILDS = {
    "totsulover": "744219b7c2b2ba80c5bb0440ac556201652ffbcc01d2de1211c4b576ad21db7e",
    "reference": "e676972ddaab2d45fc82210bb89143923f10d1053492a89e50571dbcec54840a",
    "koikake": "2a43bc4408aff086c18dc73ccd2d35be4f8a56cf8b3e82da7b75f80ffe61625e",
}

# Explicit machine arguments are in cdecl stack order. Unknown return types stay
# unresolved even where EAX contains an incidental pointer, coordinate or clock.
TABLE = r"""
PalDebugDump||void|The decorated export proves a void, zero-argument function. Its body is a single RET and has no side effects.
PalDebugSetEBP|frame_pointer:ptr32<i32>|u32|Stores the borrowed pointer word in the debug EBP slot and returns 0. Does not read the pointed-to memory. Decorated signature proves unsigned-long return and pointer-to-long input.
PalDebugSetESP|stack_pointer:ptr32<i32>|u32|Stores the borrowed pointer word in the debug ESP slot and returns 0. Does not read the pointed-to memory. Decorated signature proves unsigned-long return and pointer-to-long input.
PalDebugSetRegistryCount|count:i32|u32|Stores the complete count word in a global debug register-count slot and returns 0. Decorated signature proves signed-int input and unsigned-long result.
PalDebugWindowToggleControl|window_id:u32|i32|Finds a debug window by numeric ID, XORs its visible word with 1, shows or hides its dialog, updates the menu check and returns 1. Missing ID returns 0. An old value of exactly 1 selects hiding; every other old value selects showing.
DebugSetActionList|text:ptr32<const char>|unresolved|Copies at most 32 bytes into the global action-list text buffer, then forces byte 31 to zero. Source is required; the native strncpy return pointer is incidental and does not establish a public result contract.
GetMemoryPool|out_capacity:ptr32<u32>|ptr32<u8>|Returns the borrowed engine scratch-pool pointer. Optional output receives its configured capacity in bytes. Pool remains engine-owned and must not be freed by the caller; a null capacity pointer is accepted.
GetMinWindowSize|out_width:ptr32<i32>,out_height:ptr32<i32>|unresolved|Divides configured logical width/height by four using a logical shift, builds a zero-origin rectangle, calls AdjustWindowRectEx with the configured style and writes the resulting outer dimensions. Both output pointers are required. EAX happens to contain height, not an established success result.
IsActive||u32|Returns 0 when the minimized/inactive gate is nonzero; otherwise returns the complete stored activation word. Engine context is required and unchecked.
IsSystemEnd||u32|Returns the process-global shutdown latch. Shutdown sets it to 1; the reviewed PalInit path does not clear this latch.
Pal3DIs||u32|Returns renderer_flags & 8, exactly 0 or 8, not normalized 0/1. Engine context is required.
PalAttachProcess|callback:ptr32<ProcessCallback>,argument:u32|i32|Dispatches message 0x806A to the main window; that handler calls the cdecl callback with one complete argument word and returns 0. Main-window dispatch does not null-check the callback. Delivery mechanism differs by profile.
PalAttachWorkProcess|callback:ptr32<ProcessCallback>,argument:u32|i32|Retries PostThreadMessageA(work_thread,0x806A,callback,argument) with Sleep(1) until accepted, without timeout. Returns the successful BOOL. Work thread later calls nonnull cdecl callback(argument); null callback is an accepted no-op. The argument is not copied or owned beyond its single word.
PalBeginUpdate|window_id:i32|i32|When debug mode is enabled, finds the debug dialog ID and its list control 1001, then calls LockWindowUpdate(control). Missing ID/control returns 0; otherwise returns 1 without checking LockWindowUpdate's BOOL. With debug disabled it is a successful no-op. This locks a debug UI list, not the sprite renderer.
PalEndUpdate||i32|Calls LockWindowUpdate(NULL) and returns 1 regardless of the Win32 BOOL. It has no render-list lock effect.
PalChangeAspectMode|mode:u32|i32|Requires a live running engine. Attempts to queue render-thread message 0x8068 with the mode, retrying for up to 3000 wrapping milliseconds while pumping caller-thread messages and discarding WM_QUIT. Returns 1 once queued, not once applied; returns 0 if not running or posting times out. Downstream stores the mode and directly indexes a four-entry table without bounds validation.
PalChangeWindowSize|width:u32,height:u32|i32|Requires a live running engine. Nonzero window mode returns success without resizing. Otherwise queues render-thread message 0x8067, retrying for up to 3000 milliseconds with a message pump. Downstream recomputes client height as trunc(width * logical_height/logical_width) when a change is needed; the height argument participates in the no-change check but does not override aspect ratio. Returns queue acceptance, not render-device reset success.
PalCheckAspectMode||u32|Returns the complete current aspect-mode word from the copied system parameters, without boolean normalization.
PalCheckWindowMode||u32|Returns the complete current window-mode word. Zero is ordinary windowed; nonzero selects the fullscreen path in the reviewed consumers.
PalDebugIs||u32|Returns the complete debug-enable word. Totsulover additionally returns 0 if the engine context is null; old profiles dereference it unconditionally.
PalDebugPrintf|window_id:i32,label:ptr32<const char>,format:ptr32<const char>,...|i32|Formats cdecl varargs into a fixed local byte buffer and appends to debug list control 1001, scrolling to the last row and optionally writing text plus CRLF to that dialog's log. Mode 2 inserts label as column zero and formatted text as column one. Suppressed debug output returns 1; missing dialog/control returns 0. The post-format length warning does not prevent native buffer overflow. Per-profile suppression differs.
PalDebugSetData|window_id:i32,data_address:ptr32<ptr32<u32>>,count:i32|i32|Finds a debug dialog, copies the pointer word read from *data_address to its data slot and stores count, then inserts numeric row labels 0..count-1. Borrows the underlying data without copying its elements. Returns 0 both on success and missing ID. Pointer is only dereferenced after a matching ID is found.
PalDebugSetSaveThumbnail|word:u32|i32|Stores the complete word in a process-global debug thumbnail slot and returns 0. The setter does not dereference it; downstream semantic type is not established by this body.
PalDebugSetStringNo|string_number:i32|i32|Stores the complete word in the global debug string-number slot and returns 0.
PalDebugWindowCreate|window_id:i32,title:ptr32<const char>,mode_word:u32|i32|Rejects duplicate ID, allocates/zeroes a 36-byte owned dialog record, stores ID and low byte of mode, creates dialog resource 105, sets title, and inserts it at the head of the debug list. Mode byte 1 creates/truncates <base>/_PAL_DEBUG_REPORT_/<title>.txt; mode byte 2 selects two-column printing. Reads geometry from setting.ini using title as section. Returns 1 on creation, 0 on duplicate/allocation/dialog failure; releases the record on dialog failure. Title is required and native formatting is unbounded.
PalDebugWindowDelete|window_id:i32|i32|Finds the debug record, removes its menu item, closes its nonzero log handle, destroys the dialog, unlinks and frees the record. Returns 1 on found, 0 on missing. It does not free the external data pointer registered by DebugSetData.
PalDebugWindowDeleteLog|window_id:i32|i32|Finds list control 1001 and repeatedly deletes its next item until none remains. Returns 1 when the control exists, even when already empty; missing ID/control returns 0. It does not truncate the on-disk log.
PalDebugWindowGetState||u32|Returns the full debug state word if the parent debug window exists, else 0. Engine context itself is unchecked.
PalDebugWindowSetState|state:u32|i32|Stores the complete debug state word only if the parent debug window exists. Returns 1 then, otherwise 0; no flag masking.
PalDefaultCreate|parameters:ptr32<SystemParameters32>|unresolved|Zeroes exactly 108 bytes, sets static title/class string pointers and profile-specific default dimensions, centers the window on the desktop, and writes the documented defaults in SystemParameters32. Required output pointer is unchecked. Return register is incidental: centered Y in newer/reference and the record pointer in Koikake.
PalDisableChangeWindowMode|disabled:u32|unresolved|Stores the complete process-global disable word. The window procedure uses it to suppress Alt+Enter and bypass the close-confirmation prompt. It is distinct from WindowChangeEnable and does not directly reject programmatic window-mode requests. EAX echoes the input but source-level return type remains unresolved.
PalEnd||unresolved|If the shutdown latch is zero, synchronously sends main-window message 0x8069. That handler tears down debug UI/cursor, signals/stops and closes worker/render threads, restores IME and destroys the window. Already-ended calls perform no operation and leave EAX undefined; no stable success return is established.
PalErrorLogPrintf|format:ptr32<const char>,...|i32|With debug disabled returns 1. Otherwise requires a nonzero non-INVALID_HANDLE_VALUE error-log handle, formats once into a fixed temporary buffer to determine length, allocates length+1, formats again, writes bytes plus CRLF and frees allocation. Returns 0 for unavailable handle/allocation and 1 after attempted writes; WriteFile failures are ignored. This is cdecl varargs, not an inferred fixed two-argument function.
PalErrorLogPrintfBasic|format:ptr32<const char>,...|i32|With debug disabled returns 1. Otherwise formats cdecl varargs into a fixed local buffer, writes bytes plus CRLF to the error log and returns 1 regardless of WriteFile success. Missing/invalid handle returns 0. Native formatting has no safe length bound.
PalErrorLogPuts||i32|Reads Win32 GetLastError, FormatMessageA into a 1024-byte stack buffer, emits it to OutputDebugStringA and passes that message as the format string to PalErrorLogPrintf. Takes no string parameter. FormatMessage failure is not checked.
PalFrameRateSet|frames_per_second:u32|unresolved|Writes the frame-rate word directly. The render loop divides integer 1000 by this value, so zero is not a supported unlimited-rate sentinel and can divide by zero. 0xFFFFFFFF requests the Sleep(1) branch. Source-level setter return is unresolved; EAX contains the engine pointer.
PalGetAllocCount||u32|Returns the engine's complete allocation counter word without a null check.
PalGetChangeWindowModeTime||u32|Returns a process-global millisecond timestamp. It is independent from the cached PAL frame clock.
PalGetCmdLine||ptr32<const char>|Returns the borrowed command-line pointer stored in SystemParameters32, without copying it. Null content is possible; engine context must exist.
PalGetCore||ptr32<Renderer>|Returns the borrowed GPU core pointer when renderer_flags bit 8 is set, otherwise the software core pointer. It does not return the whole PAL engine context.
PalGetDesktopRect|out_rect:ptr32<Rect32>|unresolved|When desktop override is enabled, writes [0,0,override_width,override_height]. Otherwise delegates to GetWindowRect(GetDesktopWindow()). Required output pointer is unchecked; return is either height or the OS BOOL and has no uniform public contract.
PalGetFilePath||ptr32<const char>|Returns the borrowed engine base-directory string. Old builds expose the inline context buffer; newer returns a separate process-global buffer. This is not a path resolver and takes no filename argument.
PalGetInstanceHandle||u32|Returns the borrowed 32-bit HINSTANCE word copied from system parameters.
PalGetSystemParameters|out_parameters:ptr32<SystemParameters32>|void|Copies exactly 108 bytes from the engine's current parameter block into required caller storage. Copied pointers remain borrowed; this does not deep-copy title, class or command-line strings or callbacks.
PalGetWindowHandle||u32|Returns the borrowed 32-bit main HWND word. Engine context is required.
PalGetWindowSize|out_width:ptr32<u32>,out_height:ptr32<u32>|i32|Each output is independently optional. Both null returns 0. In ordinary windowed mode writes stored client dimensions; in nonzero window mode writes current presentation rectangle extents. Returns 1 when at least one output is supplied.
PalGetlpPool||ptr32<u8>|Returns the same borrowed process-global scratch-pool pointer as GetMemoryPool, with no size output.
PalInit|parameters:ptr32<SystemParameters32>|i32|Rejects null parameters or zero client width/height, registers the window class, recenters an out-of-desktop position by mutating the caller's record, creates the main window and initializes the engine from a 108-byte shallow parameter copy. Requires a nonnull startup callback. Allocates renderer, scratch pool, task/input/audio/resource state; requested GPU failure can fall back to software and clear renderer flag 8. Honors hidden-start bit 0 at parameters+88. Returns 1 on successful initialization, 0 on failure; failures are not a transactional rollback. Newer initialization runs from WM_CREATE, old builds invoke the helper after CreateWindowExA.
PalIsDesktopRectFix||u32|Returns the entire process-global desktop-override enable word without normalization.
PalIsWorkThreadId||i32|Compares GetCurrentThreadId with the ID recorded by the work-thread entry and returns normalized 0/1.
PalRun||u32|Creates work and render threads with startup events, starts the render-thread startup callback, marks the engine running and pumps the main-window message loop. Returns WM_QUIT wParam when received; thread-creation failures report a native dialog and return 0xFFFFFFFF. Exit through the running flag without a prior message can expose an uninitialized/stale MSG.wParam. Not a one-frame update call.
PalSetChangeWindowModeTime||unresolved|Calls timeGetTime and stores the result in the mode-change timestamp. It has no timestamp argument. EAX contains the clock value, but a source-level return type is not established.
PalSetDesktopRect|enabled:u32,width:i32,height:i32|unresolved|Stores all three complete words as desktop override configuration. Width/height are used when enabled is nonzero. There is no dimension validation or immediate resize; EAX happens to echo height.
PalUninit||i32|Returns 0 for null engine. Otherwise clears the running flag, releases tasks, fonts, resources, input/audio/renderer allocations and scratch memory, closes the error log, ends the timer period and frees engine context. This is resource teardown; worker/render thread stopping belongs to PalEnd. Returns 1 after teardown. Global-context clearing differs by profile.
PalUpdateTime||u32|Samples timeGetTime, stores the complete wrapping u32 value in the cached PAL clock and returns that value.
PalWait|ticks:i32|ptr32<Task>|Creates a wait task with four-byte work storage and returns the owned scheduler task pointer or null. First scheduled callback installs the countdown callback. Each later tick leaves -1 unchanged; otherwise decrements with wrapping arithmetic and frees the task when the result reaches zero. Input 0 becomes -1 and consequently waits indefinitely. This function queues a wait; it does not block the calling OS thread.
PalWaitTime|duration_ms:u32|ptr32<Task>|Creates a wait task with eight-byte work storage. First scheduled callback records cached PAL time, later callbacks free the task when wrapping unsigned elapsed >= duration. Clock capture occurs at first scheduling, not at the exported call. Returns scheduler task pointer or null.
PalWaitTimeEx|duration_ms:u32,parent:ptr32<Task>|ptr32<Task>|Creates a wait task with 12-byte work storage and passes the second argument as PalTaskCreate's parent. Stores duration at work+4 and first scheduled clock at work+8, then compares wrapping unsigned elapsed >= duration. Second argument is not an input mask or callback. Returns scheduler task pointer or null.
PalWindowChangeEnable|enabled:u32|i32|Stores the full enable word consumed by the render thread's mode-change message handler and returns 1. It is not consulted by the public queueing function, so accepted requests may be ignored later.
PalWindowChangeEnableIs||u32|Returns the complete stored window-change enable word, not normalized bool.
PalWindowChangeMode|mode:u32|i32|Requires a live running engine, queues render-thread message 0x8065 and retries for at most 3000 milliseconds while pumping caller messages. Returns queue acceptance. Consumer obeys WindowChangeEnable, locks rendering, suspends work, forwards to main window and resets frame timing afterward. Same-mode and fixed-mode-flag requests are downstream no-ops; backend reset failure can restore the prior mode.
PalWindowGetPos|out_x:ptr32<i32>,out_y:ptr32<i32>|unresolved|Writes each optional coordinate independently: stored window origin in mode 0, presentation-rectangle origin otherwise. Both null is accepted. EAX contains whichever value/context register happened to be used and is not a stable success result.
PalWindowGetPosCenter|out_point:ptr32<Point32>,width:i32,height:i32|i32|Null point returns 0. Otherwise writes [(desktop.right-width)>>1,(desktop.bottom-height)>>1] using arithmetic signed shifts and returns 1. Ignores desktop left/top. Newer build honors the desktop override; old builds query the real desktop.
PalWindowSetPos|x:i32,y:i32|i32|Stores both complete coordinate words before calling SetWindowPos(HWND,0,x,y,0,0,SWP_NOSIZE). Returns the OS BOOL without rolling back stored coordinates on failure. It does not skip fullscreen mode.
PaltimeGetTime||u32|Returns the cached PAL frame clock without sampling the OS clock. It advances only when the engine loop or PalUpdateTime updates it.
PalSaveScreenShot||i32|This newer release export is a constant-zero stub and performs no capture or file write. Its empty behavior does not establish source-level argument arity.
"""

SPECS = {}
for line in TABLE.strip().splitlines():
    name, arguments, result, behavior = line.split("|", 3)
    SPECS[name] = (arguments, result, behavior)

def arguments(spec):
    result = []
    for index, argument in enumerate(spec.split(",") if spec else []):
        if argument == "...":
            break
        name, value_type = argument.split(":", 1)
        row = {"name": name, "type": value_type, "width_bytes": 4,
               "stack_offset_from_entry_esp": 4 + index * 4}
        if value_type.startswith("ptr32"):
            row["ownership"] = "borrowed unless behavior explicitly assigns ownership"
            row["nullability"] = "see individual behavior; no implicit null acceptance"
        result.append(row)
    return result


def canonical(name):
    return name[1:].split("@@", 1)[0] if name.startswith("?") else name


HELPERS = {
    "totsulover": {
        "window": "0x100a0910", "init": "0x100a34f0", "render_thread": "0x100a2780",
        "work_thread": "0x100a26f0", "shutdown": "0x100a0660", "mode": "0x100c18f0",
        "wait": "0x100c1340", "wait_time": "0x100c1540", "wait_time_ex": "0x100c1490",
        "debug_toggle": "0x100a6050", "debug_delete": "0x100a5b10", "formatter": "0x100a5ad0",
    },
    "reference": {
        "window": "0x100a05b0", "init": "0x100a2c20", "render_thread": "0x100a20f0",
        "work_thread": "0x100a2060", "shutdown": "0x100a0300", "rate": "0x100ab030",
        "wait": "0x100badf0", "wait_time": "0x100bafa0", "wait_time_ex": "0x100baec0",
        "formatter": "0x100a57c0",
    },
    "koikake": {
        "window": "0x100adca0", "init": "0x100af000", "render_thread": "0x100aea80",
        "work_thread": "0x100ae9e0", "shutdown": "0x100ad9f0", "mode": "0x100cc7c0",
        "size": "0x100cc980", "aspect": "0x100ccb60", "rate": "0x100b6e10",
        "wait": "0x100cc1a0", "wait_time": "0x100cc250", "wait_time_ex": "0x100cc200",
        "debug_delete": "0x100b1580", "debug_list": "0x100a14d0",
    },
}

MAPPING = {
    "PalWait": "crates/pal-vm/src/task.rs / runtime.rs waiting; native scheduler pointer adapter is separate",
    "PalWaitTime": "crates/pal-vm/src/task.rs / runtime.rs timing; first-callback clock semantics need explicit adapter",
    "PalWaitTimeEx": "crates/pal-vm/src/task.rs; parent_task is a native pointer, not an input-mask field",
    "PaltimeGetTime": "crates/pal-vm/src/engine.rs::FrameClock and runtime frame time",
    "PalUpdateTime": "crates/pal-vm/src/engine.rs::FrameClock and runtime frame time",
    "PalInit": "crates/pal-vm/src/engine.rs / config.rs; platform launchers and host C API are separate interfaces",
    "PalRun": "crates/pal-vm/src/engine.rs and platform event loops; no native blocking-run ABI adapter certified",
    "PalUninit": "Rust ownership teardown and host C API destroy; native teardown is a distinct contract",
    "PalDefaultCreate": "crates/pal-vm/src/config.rs; native defaults vary by DLL profile",
}

functions = []
for build, digest in BUILDS.items():
    for path in DIRECTORY.glob(build + "-*.json"):
        if path.name.endswith("-inventory.json"):
            continue
        evidence = json.loads(path.read_text())
        evidence.pop("input", None)
        evidence.update({"binary": "Pal.dll", "build": build, "sha256": digest})
        path.write_text(json.dumps(evidence, indent=2) + "\n")
    path = DIRECTORY / (build + "-exports.json")
    exports = json.loads(path.read_text())["functions"]
    inventory = json.loads((DIRECTORY / (build + "-inventory.json")).read_text())
    by_address = {row["ea"]: row for row in exports}
    for entry in inventory:
        key = canonical(entry["name"])
        spec, result, behavior = SPECS[key]
        issues = []
        if result == "unresolved":
            issues.append("Source-level return type remains unresolved; do not generate a guessed callable declaration.")
        if key == "PalSaveScreenShot":
            issues.append("Constant-zero stub has no argument reads and cannot prove public argument arity.")
        if key == "PalAttachProcess":
            behavior += " Totsulover uses synchronous SendMessageA and returns its LRESULT (handler returns 0)." if build == "totsulover" else " This profile retries asynchronous PostMessageA with Sleep(1) until accepted, without timeout, and returns the successful BOOL."
        if key == "PalDefaultCreate":
            behavior += " Default client and logical size is " + {"koikake": "960x600", "reference": "1920x1080", "totsulover": "1280x720"}[build] + "; game configuration can replace it before PalInit."
        if key == "PalInit":
            behavior += " Context allocation is " + {"koikake": "0x2A90", "reference": "0x31D0", "totsulover": "0x4B30"}[build] + " bytes; scratch pool is " + ("0x2000001" if build == "koikake" else "0x8000001") + " bytes."
        if key == "GetMemoryPool":
            behavior += " Normal initialized capacity is " + ("0x2000001" if build == "koikake" else "0x8000001") + " bytes."
        if key == "PalGetSystemParameters":
            behavior += " Source block is context+" + {"koikake": "0x270", "reference": "0x3C8", "totsulover": "0x3E0"}[build] + "."
        if key == "PalUninit":
            behavior += " Koikake leaves the global engine pointer dangling after free; repeated Uninit is unsafe." if build == "koikake" else " This profile clears the global engine pointer after free."
        if key == "PalRun":
            behavior += " Main-thread idle uses Sleep(1) and ANSI messages." if build == "totsulover" else (" Main-thread idle uses WaitMessage and Unicode message retrieval/dispatch." if build == "reference" else " Main-thread idle uses WaitMessage and ANSI messages.")
        if key in {"PalErrorLogPrintf", "PalErrorLogPrintfBasic", "PalBeginUpdate"}:
            behavior += " A null engine is a successful no-op in this newer build." if build == "totsulover" else " Engine context is dereferenced without a null check in this build."
        if key == "PalDebugPrintf":
            behavior += {"koikake": " Global debug-state bit 0 suppresses every window, including ID 0. Local buffer is 2048 bytes.", "reference": " ID 0 bypasses debug-state bit 0; other IDs are suppressed. Hidden dialog visibility does not suppress logging. Local buffer is 4100 bytes.", "totsulover": " ID 0 bypasses debug-state bit 0; hidden dialog records suppress output. ID 10 retains at most 2000 displayed rows by removing the oldest. Null engine returns 1. Local buffer is 2116 bytes."}[build]
        if key == "PalDebugWindowCreate":
            behavior += " Newer geometry defaults to 300x230 and honors persisted view=0 by hiding the dialog and unchecking its menu item." if build == "totsulover" else " This profile defaults geometry to 100x100 and does not read a persisted view key here."
        if key == "PalFrameRateSet" and build == "totsulover":
            behavior += " Zero additionally opens the diagnostic message 'PalFrameSkip zero div' before the still-unsafe division."
        row = {
            "name": entry["name"], "canonical_name": key, "build": build,
            "binary_sha256": digest, "address": entry["ea"],
            "calling_convention": "cdecl", "arguments": arguments(spec),
            "variadic": spec.endswith("..."), "return_type": result,
            "abi_status": "unresolved" if issues else "reviewed_machine_contract",
            "behavior_status": "reviewed_export_boundary", "behavior": behavior,
            "evidence": [str(path.relative_to(ROOT))],
            "portable_mapping": MAPPING.get(key, "Host/platform integration; no reviewed native export adapter certified by this audit"),
            "unresolved": issues,
            "review_basis": ["export stack loads and disassembly", "explicit control-flow, memory and return paths", "downstream helper/callback evidence where behavior leaves the export"],
        }
        assert by_address[entry["ea"]].get("disassembly"), entry
        if entry["name"].startswith("?"):
            row["review_basis"].append("MSVC decorated public signature")
        helper_keys = []
        if key in {"PalInit", "PalDefaultCreate", "GetMemoryPool", "PalGetFilePath", "PalGetSystemParameters"}: helper_keys += ["init"]
        if key in {"PalRun", "PalFrameRateSet"}: helper_keys += ["render_thread", "work_thread"]
        if key == "PalAttachProcess": helper_keys += ["window"]
        if key == "PalAttachWorkProcess": helper_keys += ["work_thread"]
        if key == "PalEnd": helper_keys += ["window", "shutdown"]
        if key in {"PalWindowChangeMode", "PalChangeWindowSize", "PalChangeAspectMode", "PalWindowChangeEnable"}: helper_keys += ["window", "render_thread"]
        if key == "PalWait": helper_keys += ["wait"]
        if key == "PalWaitTime": helper_keys += ["wait_time"]
        if key == "PalWaitTimeEx": helper_keys += ["wait_time_ex"]
        if key == "PalDebugWindowToggleControl": helper_keys += ["debug_toggle"]
        if key == "PalDebugWindowDelete" and "debug_delete" in HELPERS[build]: helper_keys += ["debug_delete"]
        if key in {"PalErrorLogPrintf", "PalErrorLogPrintfBasic", "PalDebugPrintf"} and "formatter" in HELPERS[build]: helper_keys += ["formatter"]
        if helper_keys:
            row["reviewed_helper_addresses"] = [HELPERS[build][name] for name in helper_keys]
            for suffix in ("helpers", "wait", "consumers"):
                candidate = DIRECTORY / f"{build}-{suffix}.json"
                values = json.loads(candidate.read_text()).get("functions", [])
                if any(value.get("ea") in row["reviewed_helper_addresses"] for value in values):
                    row["evidence"].append(str(candidate.relative_to(ROOT)))
        functions.append(row)

PARAMETERS = [
    (0, "instance", "u32", 0), (4, "window_mode", "u32", 0), (8, "debug_enabled", "u32", 0),
    (12, "window_title", "ptr32<const char>", "WindowTitle"),
    (16, "window_class", "ptr32<const char>", "CLASS"),
    (20, "icon_resource", "u32", 0), (24, "menu_resource", "u32", 0),
    (28, "command_line", "ptr32<const char>", 0), (32, "window_style", "u32", 47120384),
    (36, "frame_rate", "u32", 60), (40, "window_x", "i32", "centered"),
    (44, "window_y", "i32", "centered"), (48, "client_width", "u32", "profile-specific"),
    (52, "client_height", "u32", "profile-specific"), (56, "logical_width", "u32", "profile-specific"),
    (60, "logical_height", "u32", "profile-specific"), (64, "texture_capacity", "u32", 256),
    (68, "capacity_68_unresolved", "u32", 512), (72, "random_table_length", "u32", 2048),
    (76, "startup_callback", "ptr32<StartupCallback>", 0),
    (80, "shutdown_callback", "ptr32<ShutdownCallback>", 0),
    (84, "renderer_parameter_84", "u32", 0), (88, "window_flags", "u32", 0),
    (92, "renderer_flags", "u32", 17435), (96, "frame_pacing_mode", "u32", 2),
    (100, "aspect_mode", "u32", 1), (104, "word_104_unresolved", "u32", 1),
]

payload = {
    "schema_version": 1, "review_date": "2026-10-23", "builds": BUILDS,
    "scope": "Native PAL lifecycle, windows, clocks/waits, context accessors, debug/logging and release screenshot stub across three profiles. Input/button/touch/cursor exports are owned by a separate audit.",
    "coverage": dict(Counter(row["build"] for row in functions)),
    "status_definition": {
        "reviewed_export_boundary": "Export body and named major helper/callback boundaries are reviewed; downstream Windows/D3D driver implementation and untraced private internals are not certified.",
        "reviewed_machine_contract": "Machine argument widths/order and return path were reviewed independently of IDA-inferred public prototypes.",
        "unresolved": "Listed arity or return-type ambiguity must remain explicit in generated callable ABI declarations.",
    },
    "functions": functions,
    "shared_layouts": [{
        "name": "PalSystemParameters32", "size": 108, "alignment": 4, "build": "all",
        "fields": [{"offset": offset, "name": name, "type": kind, "width_bytes": 4, "default": default}
                   for offset, name, kind, default in PARAMETERS],
        "ownership": "Caller owns the 108-byte record; PalInit copies its words. All copied strings, resources and callback addresses remain borrowed.",
        "unresolved": ["Semantic name of offset 68 (default 512)", "Complete interpretation of renderer parameter offset 84", "Semantic name of offset 104 (default 1)"],
    }],
    "callbacks": [
        {"name": "ProcessCallback", "calling_convention": "cdecl", "arguments": ["u32 argument"], "return_type": "void", "notes": "Window-thread callback is unchecked; work-thread callback skips null. Return value is ignored, so void expresses the portable consumption contract rather than recovering every caller's source type."},
        {"name": "StartupCallback", "calling_convention": "cdecl", "arguments": [], "return_type": "i32", "notes": "Called once from the render thread; zero triggers PalEnd instead of the frame loop."},
        {"name": "ShutdownCallback", "calling_convention": "cdecl", "arguments": ["ptr32<Opaque> first_root_child_work"], "return_type": "unresolved", "notes": "Optional callback receives the first root task child's work pointer. Return is ignored. Root/child validity is assumed."},
    ],
    "boundary_examples": [
        {"function": "Pal3DIs", "input": "renderer_flags=8", "expected": "Returns 8, not 1."},
        {"function": "PalWait", "input": "ticks=0", "expected": "First callback installs countdown; next changes 0 to -1; later countdown callbacks preserve -1 indefinitely."},
        {"function": "PalWaitTime", "input": "start=0xFFFFFFFC,current=1,duration=5", "expected": "Unsigned wrapping elapsed is 5 and completes the wait."},
        {"function": "PalWindowChangeMode", "input": "running engine, WindowChangeEnable=0, queue accepts request", "expected": "Export returns 1 although render-thread consumer ignores the request."},
        {"function": "PalChangeWindowSize", "input": "logical=1280x720, request=800x900, current size differs", "expected": "Applied client size is 800x450; requested height does not override logical aspect ratio."},
        {"function": "PalDebugSetData", "input": "matching dialog, valid pointer and count", "expected": "Stores borrowed data and creates rows, then returns 0."},
        {"function": "PaltimeGetTime", "input": "OS time advances without PalUpdateTime/frame update", "expected": "Repeated queries retain the same cached u32 time."},
    ],
}
output = ROOT / "reverse/contracts/platform_lifecycle.json"
output.write_text(json.dumps(payload, indent=2) + "\n")
print(json.dumps(payload["coverage"]))
