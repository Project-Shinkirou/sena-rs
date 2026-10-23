"""Bind manually reviewed native input/button contracts to release evidence.

This is a behavior inventory, not the PAL declaration generator. Incidental EAX
contents are recorded without inventing a C return type.
"""
import hashlib
import importlib.util
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
spec = importlib.util.spec_from_file_location("native_pe", ROOT / "reverse/evidence/audit_native_interfaces.py")
pe = importlib.util.module_from_spec(spec)
spec.loader.exec_module(pe)
build_paths = {
    "totsulover": "target/reverse-audit/totsulover/Pal.dll",
    "reference": "reverse/PAL.dll",
    "koikake": "testcase/dll/Pal.dll",
}
builds = {
    build: {"binary": path, "sha256": hashlib.sha256((ROOT / path).read_bytes()).hexdigest(),
            "exports": {name: row["address"] for row in pe.PE(ROOT / path).exports() for name in row["names"]}}
    for build, path in build_paths.items()
}
contracts = {}
input_map = ["crates/pal-vm/src/input.rs", "crates/pal-vm/src/runtime.rs", "crates/pal-vm/src/event.rs"]
button_map = ["crates/pal-vm/src/runtime.rs", "crates/pal-vm/src/sprite.rs", "crates/pal-vm/src/audio.rs"]
cursor_map = ["crates/pal-vm/src/app.rs", "crates/pal-vm/src/input.rs", "crates/pal-vm/src/runtime.rs"]


def arg(name, native_type="uint32_t", **extra):
    return dict(name=name, native_type=native_type, stack_slot_bits=32, **extra)


def add(name, arguments, result, behavior, edges=(), mapping=None, unresolved=(), variants=None):
    contracts[name] = {
        "name": name, "abi_status": "partial" if unresolved else "confirmed",
        "behavior_status": "reviewed", "calling_convention": "cdecl",
        "arguments": arguments, "return_contract": result, "behavior": behavior,
        "edge_cases": list(edges), "unresolved": list(unresolved),
        "implementation_mapping": mapping or input_map,
        "implementation_parity": "not_certified",
        "build_differences": variants or {},
        "abi_evidence_basis": "Reviewed stack/register loads, cleanup, return paths, pointer accesses and caller use; not inferred from the decompiler prototype alone.",
    }


context_return = "Original C return declaration unresolved: EAX contains a context pointer or other incidental register value; callers inspected do not consume it."
mask = arg("mask")
for name, field, desc in [
    ("PalInputGetKeyPush", "push", "Bits newly pressed: current & (current XOR previous)."),
    ("PalInputGetKeyOn", "on", "Stable held bits: current & previous; excludes the first sampled press."),
    ("PalInputGetKeyPull", "pull", "Bits newly released: previous & (current XOR previous)."),
    ("PalInputGetKeyOnEx", "repeat", "Last newly pressed mask after strictly more than 500 ms while any bit of that mask remains held; otherwise zero. This is a level, not a periodic pulse."),
    ("PalInputGetKeyEx", "extended", "Frame-local extended-event word: left/right/middle double-click bits1/2/4 and window-mode-change bit8. Wheel has a separate signed16 latch."),
]:
    add(name, [], "uint32_t raw input mask", f"Returns the stored {field} word. {desc}",
        ["No read-to-clear side effect.", "Requires initialized PAL context."])
for suffix in ("On", "Push", "Pull"):
    add("PalInputSetKey" + suffix, [mask], "C return type unresolved; see per-build machine result",
        f"ORs mask into only the stored key {suffix.lower()} word. Does not change physical sampling history, the other edge words, or repeat history.",
        ["No clamping or validation of bits; includes mouse bits23..25."],
        unresolved=[context_return], variants={"koikake": "EAX retains PAL context pointer.", "reference": "EAX echoes mask.", "totsulover": "EAX echoes mask."})
add("PalInputClear", [], "C return type unresolved; EAX is PAL context pointer",
    "Zeros four joystick output words and key on/push/pull/repeat outputs. Preserves physical current/previous samples, repeat source/time, mouse position/delta, wheel, extended events and cancel masks.",
    ["Held input can return on the next sampling pass without becoming a new push."], unresolved=[context_return])
add("PalInputKeyRepeatClear", [], "C return type unresolved; EAX is PAL context pointer",
    "Zeros repeat output and remembered last-push mask. Does not clear physical held state or the remembered timestamp.", unresolved=[context_return])
add("PalInputKeyCancel", [arg("key_mask"), arg("extended_mask")], "C return type unresolved; EAX is incidental and differs by build",
    "Stores persistent masks. Immediately removes key-mask bits from on/push/pull/repeat; removes extended-mask bits from KeyEx. Extended bit0x10 clears wheel. The sampling pass reapplies both masks.",
    ["Passing zero replaces the mask but cannot restore already-cleared events in the same sample.", "The second mask is not a separate mouse-button mask."], unresolved=[context_return])
for suffix, desc in [
    ("Push", "newly pressed joystick bits"), ("On", "current & previous joystick bits"),
    ("Pull", "newly released joystick bits"),
    ("OnEx", "stored extended joystick word; the inspected polling helper does not populate it"),
]:
    add("PalInputGetJoy" + suffix, [], "uint32_t raw joystick mask", "Returns " + desc + " when joystick-enabled is nonzero, otherwise zero.",
        ["No read-to-clear side effect.", "DirectInput X/Y thresholds are strictly below -800 or above800; direction bits up/down/left/right=1/2/4/8, buttons begin at bit4."])
for suffix, desc in [("X", "logical cursor x"), ("Y", "logical cursor y"), ("MoveX", "sample-to-sample logical x delta"), ("MoveY", "sample-to-sample logical y delta")]:
    add("PalInputGetMouse" + suffix, [], "int32_t signed coordinate or delta", "Returns stored " + desc + ". No coordinate recomputation or consumption occurs in the export.",
        ["Inactive/minimized/resize gate sets coordinates -1 and deltas0.", "Sampling uses single-precision scaling and truncation toward zero, not rounding; see narrative for out-of-window rules."])
add("PalInputGetWheel", [], "int16_t signed result in AX; high EAX bits unspecified",
    "Returns the signed16 wheel latch. Windows positive wheel delta becomes -1; negative becomes +1. The last queued wheel message wins, then the engine clears the latch after its frame callback.",
    ["Export itself does not consume the latch.", "Magnitude is normalized by the window procedure; multiple events are not summed.", "A zero Windows delta follows the nonnegative branch and becomes -1."])

group = arg("group", "PalButtonGroupOpaque *", nullable=True, ownership="borrowed unless Release is called")
sprite = arg("sprite", "PalSpriteOpaque *", nullable=True, ownership="borrowed; button registration never owns the sprite")
sound = lambda name: arg(name, "int32_t", meaning="native tagged PAL sound handle")
path = lambda name: arg(name, "const char *", nullable=True, ownership="borrowed for synchronous sound loading", encoding="native narrow NUL-terminated resource name")
reaction = arg("reaction", "PalButtonReaction32 *", nullable=True, ownership="caller-owned writable12-byte output; special null-group rule for Ex", layout="PalButtonReaction32")
add("PalButtonCreate", [path("hover_sound"), path("click_sound")], "Owned PalButtonGroupOpaque pointer, null on allocation failure",
    "Allocates and zeroes group; loads optional hover and click assets into sound group5. Native filename constructor incorrectly calls SoundCopy on each zeroed destination slot rather than the loaded hover source, so only the first hover slot is normally usable.",
    ["Sound-load failure does not reject the button group.", "Release destroys the sound handles and group, never registered sprites."], button_map,
    variants={"koikake": "Group allocation80 bytes; sound slots+48..64, click+68, synthetic flag+76.", "reference": "84 bytes; sound slots+52..68, click+72, synthetic flag+80.", "totsulover": "Same84-byte group as reference."})
add("PalButtonCreateEx", [sound("hover_sound"), sound("click_sound")], "Owned PalButtonGroupOpaque pointer, null on allocation failure",
    "Copies hover sound four times into group5, then stores original hover and click handles. Release will release both original handles too: ownership transfers to the group on successful allocation.",
    ["Copies may fail independently without rejecting group.", "Allocation failure does not release caller handles.", "Same allocation/layout differences as Create."], button_map)
add("PalButtonRelease", [group], "int32_t:1 released,0 null group",
    "Frees every registration node, releases five hover handles and click handle, then frees group. Registered sprites remain alive.",
    ["Caller must not retain/reuse group or nodes after release.", "No validation of dangling nonnull pointers."], button_map)
for name, extra in [("PalButtonEntry", []), ("PalButtonEntryEx", [arg("flags")])]:
    add(name, [group, sprite] + extra, "int32_t:1 inserted,0 null group/allocation failure",
        "Allocates24-byte node at list head; enabled=1,reserved=0,base_row=0. " + ("flags=1." if not extra else "Copies all32 flag bits."),
        ["Null sprite and duplicate sprites are accepted at insertion.", "Newer insertion is searched first when priorities tie.", "Flags:1 push,2 repeat-or-push when bit1 absent,4 suppress cell changes,0x10000 forwarded to SpriteHit; newer GetReaction also honors8 sound suppression."], button_map)
add("PalButtonDelete", [group, sprite], "int32_t:1 first match removed,0 missing/null group",
    "Unlinks and frees the first matching registration node only. Does not release sprite, reset pending reaction, or remove duplicate registrations.", mapping=button_map)
add("PalButtonCtrl", [group, sprite, arg("enabled")], "int32_t:1 first matching node changed,0 missing/null group/null sprite",
    "Stores raw enabled word in first matching node. Does not change sprite visibility or clear pending reaction.",
    ["Disabled front node still occludes lower-priority button nodes during hit selection."], button_map)
add("PalButtonSetPos", [group, sprite, arg("base_row", "int32_t")], "int32_t:1 matching node,0 missing/null group",
    "Stores base cell row and calls SpriteRectSetPos(sprite,0,row); this is an image cell, not screen x/y.",
    ["A null sprite registration can match and returns1 after native sprite routine reports error6.", "SpriteRectSetPos row clamping is build-specific and belongs to sprite contract."], button_map)
add("PalButtonGetPos", [group, sprite], "int32_t base row; -1 missing/null group/null sprite",
    "Returns node base_row without querying visible sprite cell.", mapping=button_map)
add("PalButtonSetReaction", [group, sprite], "int32_t always0",
    "If group exists and no pending sprite, stores requested sprite and sets synthetic flag1. Does not require sprite registration. A pending reaction rejects replacement.",
    ["A zero requested sprite still sets synthetic flag but does not enter pending path."], button_map)
add("PalButtonIsProcessing", [group], "int32_t normalized0/1",
    "True exactly when group is nonnull and pending sprite field+0x20 is nonzero.", mapping=button_map)
add("PalButtonReset", [dict(group, nullable=False), sprite], "int32_t:1 requested sprite disabled,0 missing/null requested sprite",
    "Always clears current/previous12-byte reactions, capture sprite and synthetic flag. If requested sprite exists in list, also sets its enabled word0.",
    ["Reset-only with null sprite still resets state but returns0.", "Null group is dereferenced; do not claim null-safe export."], button_map)
add("PalButtonMode", [arg("mode", "uint8_t", meaning="low8 bits of four-byte cdecl slot")], "C return type unresolved; EAX is context pointer",
    "Stores zero-extended byte in global button-mode word. Any nonzero mode uses release-style capture processing in GetReaction.",
    mapping=button_map, unresolved=[context_return])
add("PalButtonGetHitSprite", [group, arg("x", "int32_t"), arg("y", "int32_t")], "Borrowed PalSpriteOpaque pointer, null if no enabled hit",
    "Tests every node with SpriteHit(flags&0x10000). Chooses strictly smallest native sprite priority, retaining first encountered tie. Disabled winner produces null and occludes rear nodes. Then confirms enabled registration.",
    ["Null group or negative x/y returnsnull; helper is called before negative-coordinate rejection.", "No sprite ownership transfer or mutation."], button_map)
add("PalButtonHitTest", [group, arg("x", "int32_t"), arg("y", "int32_t")], "int32_t normalized0/1",
    "Returns GetHitSprite(group,x,y)!=null with identical hit/occlusion rules.", mapping=button_map)
add("PalButtonGetReaction", [group, reaction], "Borrowed completed sprite pointer; null while idle/hovering/pending",
    "Polls shared input and updates group latch. If pending, synthetic flag starts click sound once; otherwise waits until click sound is not playing, then moves pending to completed. If idle, copies current to previous, clears current, finds front hit, updates cells and starts pending click according to mode/flags. Copies {completed,hovered,pending} to optional output.",
    ["Null group returns0 and zeroes output if nonnull.", "Even with no click sound, clicked sprite is not returned until a later poll.", "Export does not clear input itself; Game.exe consumer clears input while pending.", "No native callback is invoked; VM wrapper dispatches callbacks using completed and hovered pointers.", "Pending branch bypasses hit testing and cell reset."], button_map,
    variants={"koikake": "No global release-mode API; push/repeat node flags only. Sound suppression bit8 is ignored.", "reference": "Release mode replaces node flags with2, losing flags4/8/0x10000. Push mode has no sound-suppression-bit8 checks.", "totsulover": "Release mode ORs node flags with2; GetReaction respects sound-suppression bit8, including synthetic reaction using first node flags."})
add("PalButtonGetReactionEx", [group, arg("x", "int32_t"), arg("y", "int32_t"), arg("trigger", "int32_t"), reaction], "Borrowed completed sprite pointer; null until completion",
    "Same pending/sound-completion latch but uses supplied x/y and nonzero trigger. Ignores global release mode and node push/repeat flags. Applies flag4 cell suppression; does not honor newer flag8 sound suppression.",
    ["Null group unconditionally writes12 zero bytes through reaction; output must be nonnull in that case.", "With nonnull group, output may be null.", "Unlike GetReaction, no explicit negative-coordinate rejection follows hit selection.", "Any nonzero trigger starts pending; no normalization of the caller's input source."], button_map)

add("PalCursorSetPos", [arg("x", "int32_t"), arg("y", "int32_t")], "int32_t Win32 SetCursorPos result",
    "Scales logical coordinates with float32 division/multiplication and truncation to current window/fullscreen viewport, adds screen offsets, stores physical result in input position fields and calls OS cursor warp.",
    ["No clamping to logical bounds.", "Stored coordinates temporarily contain physical screen coordinates until next input sample.", "GetMouseX/Y do not independently reproject them."], cursor_map)
add("PalCursorSetAutoHideTime", [arg("milliseconds")], "uint32_t native SetTimer timer-id result or KillTimer BOOL result",
    "Stores duration and enabled=(duration!=0); starts timer id10000 with that duration or kills it for zero. Cursor movement restarts timer; timeout hides cursor subject to client-area/visibility guards.",
    ["Setter does not immediately show or hide cursor.", "Native OS timer granularity and ShowCursor count are not portable timing guarantees."], cursor_map)
add("PalSetCursor", [arg("name", "const char *", nullable=False, ownership="borrowed for call", meaning="system resource pointer/lowword integer ID when mode1; resource filename otherwise"), arg("mode", "uint16_t", meaning="low16 bits; only comparison to1"), arg("hotspot_x"), arg("hotspot_y")], "int32_t:0 file-open failure,otherwise1",
    "Mode1 loads system cursor using LoadCursorA(NULL,name). Other modes destroy prior cursor, read resource into shared buffer, decode TGA pixels and construct cursor with supplied hotspots. Stores cursor, installs window-class cursor, posts deferred SetCursor message.",
    ["Mode1 overwrites previous handle without first destroying it.", "Decode/OS cursor allocation failure can still result in return1 with null cursor.", "Filename buffer and hotspot values are not range-checked at this boundary."], cursor_map)
add("PalReleaseCursor", [], "C return type unresolved; EAX is incidental context/handle",
    "If stored cursor exists, calls DestroyCursor and clears stored handle. Does not clear window-class cursor or post replacement message.",
    mapping=cursor_map, unresolved=[context_return])
add("?PalDeleteCursor@@YAXXZ", [], "void, established by decorated symbol YAXXZ",
    "Sets window-class GCL_HCURSOR(-12) to null; does not destroy stored cursor or clear its ownership field.", mapping=cursor_map)

add("PalIsTouch", [], "int32_t normalized0/1",
    "Returns whether dynamic RegisterTouchWindow entry exists and IsTouchWindow(engine HWND,NULL) succeeds. This is OS registration state, not physical touch availability or stored mode.",
    ["No initialized touch API returns0.", "Assumes companion IsTouchWindow entry was resolved when RegisterTouchWindow is present."])
add("PalIsTouchMode", [], "uint32_t raw stored mode", "Reads stored zero-extended mode byte; no OS query.")
add("PalTouchMode", [arg("mode", "uint8_t", meaning="low8 bits in four-byte slot")], "uint32_t:0 if dynamic registration API absent; otherwise zero-extended argument byte",
    "If API exists: nonzero newmode with oldmode0 calls UnregisterTouchWindow. Zero newmode with oldmode0 calls RegisterTouchWindow(HWND,2). Other transitions perform no registration call. Finally stores newmode.",
    ["Returning nonzero mode to zero does not re-register in the inspected native branch.", "OS success/failure is ignored; mode is still stored."])
add("PalTouchEnable", [arg("enabled", "uint8_t", meaning="low8 bits; not normalized")], "C return type unresolved; EAX context pointer",
    "Stores independent byte gate for WM_TOUCH and touch-origin right-button handling. Does not call registration functions or change stored touch mode.",
    unresolved=[context_return])
add("PalGestureCheck", [], "int32_t normalized0/1",
    "Scans five48-byte private records and returns1 if any first DWORD is between3 and7 inclusive; otherwise0. No mutation.",
    ["Does not inspect the separate48-byte buffer returned by GetGesture."],
    unresolved=["Gesture-record producer and semantic meaning of remaining record fields are not established; export read behavior is confirmed."])
add("PalGetGesture", [arg("output", "PalGestureOpaque48 *", nullable=False, ownership="caller-owned writable48-byte buffer", layout="PalGestureOpaque48")], "EAX echoes output pointer; original C return declaration unresolved",
    "Copies exactly48 bytes from private gesture buffer (newctx+0x2A0,reference+0x288) without consuming or clearing it.",
    ["Null output is dereferenced.", "Do not substitute Windows GESTUREINFO without proving field equivalence."],
    unresolved=["No consuming caller established return declaration.", "Only raw48-byte layout and first-word SetGesture write confirmed; remaining fields are opaque."])
add("PalSetGesture", [arg("value")], "C return type unresolved; EAX context pointer",
    "Writes only first DWORD of the buffer read by GetGesture. Does not update the five records scanned by GestureCheck.", unresolved=[context_return])
add("PalSetPow", [arg("value", "uint8_t", meaning="low8 bits of four-byte slot")], "AL echoes argument; remaining EAX bits and original C return declaration unresolved",
    "Writes force-power-of-two texture flag through render-context pointer: newrender+0x368,reference+0x358. Nonzero rounds positive NPOT image/create dimensions upward independently of device capability; render-target creation also makes both dimensions the maximum side. Zero permits NPOT only if capability allows it. Existing resources are not resized by setter.",
    mapping=["crates/pal-vm/src/renderer/mod.rs", "crates/pal-vm/src/runtime.rs"],
    unresolved=["Original C return declaration unresolved; do not normalize to BOOL or widen stale high EAX bits."])
contracts["PalSetPow"]["consumer_evidence"] = {
    "totsulover": {"addresses": ["0x100cb330", "0x100cb440", "0x100c9d90", "0x100a9240"], "file": "reverse/evidence/pal-platform/totsulover-consumers.json"},
    "reference": {"addresses": ["0x100c45a0", "0x100c46b0", "0x100c3260", "0x100a8310"], "file": "reverse/evidence/pal-platform/reference-consumers.json"},
}
contracts["PalGestureCheck"]["abi_status"] = "confirmed"

evidence = {}
for path in (ROOT / "reverse/evidence").glob("*.json"):
    if not (path.name.startswith("input-") or path.name == "pal-core-totsulover.json"):
        continue
    data = json.loads(path.read_text())
    if not isinstance(data, dict) or "sha256" not in data:
        continue
    for function in data.get("functions", []):
        if function.get("disassembly"):
            evidence.setdefault((data["sha256"], function["ea"].lower()), []).append(str(path.relative_to(ROOT)))
for name, row in contracts.items():
    row["builds"] = []
    for build, info in builds.items():
        if name not in info["exports"]:
            continue
        address = info["exports"][name]
        paths = evidence.get((info["sha256"], address.lower()), [])
        assert paths, (build, name, address)
        row["builds"].append(dict(build=build, binary=info["binary"], sha256=info["sha256"], address=address, evidence=paths))

vm = []
new_exe = "target/reverse-audit/totsulover/totsulover_try.exe"
for category, index, address, name, args, result, behavior in [
    (8, 12, "0x4237e0", "btn_slide_end", ["group", "index"], "destination=-1 on left pull/group-ended,else0; invalid bounds returncontrol1 without destination store", "On end sets group-ended1; while active sets task wait-control flag. Separate from8:38."),
    (8, 17, "0x423110", "btn_get_push", ["group"], "destination=stored last clicked index,or-1 invalid group", "Reads group latch; no input poll or pop-from-queue behavior."),
    (8, 23, "0x423050", "btn_get_onmouse", ["group"], "destination=stored hovered index,or-1 invalid group", "Reads group latch without recomputing hit."),
    (8, 36, "0x425d90", "btn_mode", ["mode"], "control1; no destination store", "Stores normalized task mode byte and calls native PalButtonMode."),
    (8, 38, "0x425f20", "btn_get_reaction", ["group", "index"], "destination=PalButtonGetReaction!=0;control1 valid,control0 invalidbounds", "Checks requested slot exists, but queries whole group's completed reaction, not equality to requested slot. Debug label misleadingly says btn_on_check."),
    (8, 39, "0x426080", "btn_get_toggle", ["group", "index"], "destination initialized0,then group toggle if requested slot exists; control0 invalidbounds", "Stored toggle belongs to group, not individual sprite."),
    (8, 40, "0x426180", "btn_get_info", ["group", "index", "temp_base"], "destination1 after6 outputwords,0 invalidgroup/index/base; missing sprite leaves destination unchanged", "base<=120; outputs wrapper x/y, sprite cell width/height and source rect left/top. Writes task work bank, not caller pointer."),
    (9, 35, "0x465860", "input_clear", [], "control1; no destination store", "Calls native Clear plus RepeatClear and clears task input latch."),
]:
    vm.append(dict(build="totsulover", binary=new_exe, sha256=hashlib.sha256((ROOT/new_exe).read_bytes()).hexdigest(),
                   category=category, index=index, handler=address, name=name, arguments_pop_order=args, arity=len(args),
                   abi_status="confirmed", behavior_status="reviewed", return_contract=result, behavior=behavior,
                   registration_evidence="reverse/evidence/totsulover-command-map.json",
                   evidence=["reverse/evidence/input-totsulover-exe-wrappers.json", "reverse/evidence/input-totsulover-exe-extra.json"]))

result = {
    "schema_version": 1, "scope": "Every exported input/button/cursor/touch/gesture function in the three release PAL builds, plus PalSetPow. Native exports and VM adapters remain separate.",
    "primary_build": "totsulover",
    "builds": {build: {k:v for k,v in info.items() if k != "exports"} for build, info in builds.items()},
    "functions": list(contracts.values()), "vm_wrappers": vm,
    "shared_layouts": [
        {"name": "PalButtonReaction32", "size": 12, "alignment": 4, "fields": [
            {"name": "completed_sprite", "offset": 0, "native_type": "PalSpriteOpaque *"},
            {"name": "hovered_sprite", "offset": 4, "native_type": "PalSpriteOpaque *"},
            {"name": "pending_sprite", "offset": 8, "native_type": "PalSpriteOpaque *"}],
         "ownership": "Caller-owned output; all three words are borrowed native pointers, not portable arena handles."},
        {"name": "PalGestureOpaque48", "size": 48, "alignment": "No alignment check; new build uses unaligned SSE loads/stores", "fields": [
            {"name": "value", "offset": 0, "native_type": "uint32_t"},
            {"name": "opaque", "offset": 4, "native_type": "uint8_t[44]"}],
         "ownership": "Caller-owned writable output; no transferred pointers can be asserted for opaque bytes."},
    ],
    "private_layouts": {
        "button_node": {"size": 24, "fields": {"0": "borrowed sprite", "4": "enabled word", "8": "initialized0, consumer unresolved", "12": "base cell row", "16": "flags", "20": "next owned node"}},
        "button_group": {"size": {"koikake": 80, "reference": 84, "totsulover": 84}, "common_fields": {"20": "head", "24": "completed", "28": "hovered", "32": "pending", "36": "previous completed", "40": "previous hovered", "44": "previous pending"}, "new_fields": {"48": "release capture", "52": "five owned hover sound handles", "72": "owned click sound", "80": "synthetic reaction flag"}},
    },
    "unresolved": [
        "Behavior review covers export branches and documented consumers, not OS-device certification or all thread interleavings.",
        "Several void-like setters have build-dependent incidental EAX values. They are not confirmed C scalar returns.",
        "Gesture opaque record fields/producer remain unresolved; PalSetPow consumers are covered by the graphics audit evidence.",
        "Input joystick delivery, touch hardware, custom cursor integration, click-sound delay and exact mouse projection are not certified in the portable implementation.",
        "Current runtime differs on wheel sign/magnitude, repeat scheduling, disabled-button occlusion and delayed native reactions; see input-buttons-contract.md.",
    ],
}
prefixes = ("PalInput", "PalButton", "PalGesture", "PalTouch", "PalCursor", "PalSetCursor", "PalReleaseCursor", "?PalDeleteCursor", "PalGetGesture", "PalSetGesture", "PalIsTouch", "PalSetPow")
expected = {name for info in builds.values() for name in info["exports"] if name.startswith(prefixes)}
assert expected == set(contracts), (expected - set(contracts), set(contracts) - expected)
destination = ROOT / "reverse/contracts/input_buttons.json"
destination.write_text(json.dumps(result, indent=2) + "\n")
print(f"Wrote {len(contracts)} native exports, {sum(len(r['builds']) for r in contracts.values())} export/build instances and {len(vm)} VM wrappers")
