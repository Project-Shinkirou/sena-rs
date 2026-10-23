"""Write analyst-reviewed audio/video contracts and bind them to PE evidence.

This inventory is not an ABI generator. Unresolved return semantics and private
backend limitations remain explicit and must not become generated declarations.
"""
import hashlib
import importlib.util
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
spec = importlib.util.spec_from_file_location("native_pe", ROOT / "reverse/evidence/audit_native_interfaces.py")
pe = importlib.util.module_from_spec(spec)
spec.loader.exec_module(pe)

builds = {
    "totsulover": "target/reverse-audit/totsulover/Pal.dll",
    "reference": "reverse/PAL.dll",
    "koikake": "testcase/dll/Pal.dll",
}
inventory = {}
for build, path in builds.items():
    data = (ROOT / path).read_bytes()
    inventory[build] = {
        "binary": path, "sha256": hashlib.sha256(data).hexdigest(),
        "exports": {name: row["address"] for row in pe.PE(ROOT / path).exports() for name in row["names"]},
    }

# Arguments are listed in native left-to-right order. Every integer is a native
# 32-bit word unless its width is explicitly stated otherwise.
contracts = {}

def add(name, args, result, behavior, edges, mapping, status="confirmed", notes=None):
    contracts[name] = {
        "name": name, "abi_status": status, "behavior_status": "reviewed",
        "calling_convention": "cdecl", "arguments": args, "return_contract": result,
        "behavior": behavior, "edge_cases": edges,
        "implementation_mapping": list(mapping),
        "implementation_parity": "not certified; mapping is not proof of equivalent behavior",
        "notes": notes or [],
    }

def arg(name, kind="int32_t", **more):
    return dict(name=name, native_type=kind, stack_slot_bits=32, **more)

handle = arg("handle", "int32_t", meaning="Native tagged sound word; no arena-handle substitution at ABI boundary")
sprite = arg("sprite", "PalSpriteOpaque *", nullable=True, ownership="borrowed; does not transfer ownership")
filename = arg("filename", "const char *", nullable=False, ownership="borrowed for call; path content copied or resource opened synchronously", encoding="native narrow NUL-terminated resource path")
group = arg("group", "int32_t")
volume = arg("volume", "int32_t")
range_arg = arg("range", "const PalSoundLoadRange32 *", nullable=True, ownership="borrowed synchronously; not retained", layout="PalSoundLoadRange32")
sound_map = ["crates/pal-vm/src/audio.rs", "crates/pal-vm/src/runtime/registered_audio.rs"]
movie_map = ["crates/pal-vm/src/msprite.rs", "crates/pal-vm/src/msprite/audio.rs", "crates/pal-vm/src/audio/movie.rs", "crates/pal-vm/src/runtime/movies.rs", "crates/pal-vm/src/engine.rs"]
invalid_handles = "Native resolver performs unchecked indexing for malformed tagged nonzero handles; reviewed success/error cases assume a handle from this engine."
null_sprite = "Null sprite returns zero and sets Win32 error 6."

add("PalMSpriteCheck", [sprite], "32-bit borrowed movie-worker pointer, or null; not normalized BOOL", "Reads sprite+0x14.", [null_sprite, "Existing nonmovie/info sprite returns null."], movie_map)
add("PalMSpriteGetState", [sprite], "uint32_t raw state word; null/nonmovie returns 0", "Reads worker+0x50.", [null_sprite, "Initial0; running1; stopped2; natural completion6. High bit preserved."], movie_map)
add("PalMSpriteIsLoop", [sprite], "uint32_t: worker state & 0x80000000", "Reads state high bit, not the loop-mode field.", [null_sprite, "No worker returns0; SetLoop alone does not change this result."], movie_map)
add("PalMSpriteLoad", [arg("render", "PalRenderOpaque *", nullable=False, ownership="borrowed"), filename, arg("priority")], "Owned PalSpriteOpaque pointer; null on failure; released through sprite lifecycle", "Builds MPEG graph with optional audio branch, allocates texture, inserts sprite into render tree; initially not running.", ["Missing render/allocation failure sets error6; graph failure frees sprite.", "No native audio-silent assumption."], movie_map)
add("PalMSpriteInfoCreate", [arg("filename", "const char *", nullable=True, ownership="borrowed for call"), arg("width"), arg("height"), arg("priority")], "Owned PalSpriteOpaque information object, null on failure", "Null filename uses supplied dimensions; otherwise probes movie dimensions then destroys worker and clears sprite+0x14.", ["Information object is not playable; dimensions remain signed words at ABI."], movie_map)
for name, flag in [("PalMSpriteLock",1),("PalMSpriteUnlock",0)]:
    add(name,[sprite],"int32_t:1 if worker exists,0 otherwise",f"Stores {flag} to worker+0x48 and renderer+0x188; no graph pause/stop/seek.",[null_sprite,"No observed lock-flag consumer in newer sample-copy path; do not freeze audio/clock."],movie_map)
add("PalMSpritePause",[sprite],"int32_t:nonnull sprite=>1,null=>0","No-op in all three inspected releases.",[null_sprite,"Does not inspect worker or pause audio/video."],movie_map)
add("PalMSpritePlay",[sprite,arg("loop_mode")],"int32_t success; newer version reports start failure; older versions return1 for any nonnull sprite","Stores raw loop mode and runs graph without initial seek.",[null_sprite,"Newer version retries after stop on failed start; old waiting can be unbounded.","Sets state(old&0x80000000)|1 after start."],movie_map)
add("PalMSpriteStop",[sprite],"int32_t:nonnull sprite=>1,null=>0; helper failure is ignored","Stops running graph and sets state lowbits2; does not synthesize natural-completion bit4.",[null_sprite,"Already nonrunning graph can retain previous state."],movie_map)
add("PalMSpriteSetLoop",[sprite,arg("loop_mode")],"int32_t:worker=>1,otherwise0","Stores raw nonboolean loop mode at worker+0x4C.",[null_sprite,"Any nonzero mode repeats at completion; no range clamp."],movie_map)
add("PalMSpriteSetLoopPoint",[sprite,arg("start_ms"),arg("end_ms")],"int32_t:worker=>1,otherwise0","Always stores start; only nonzero end overwrites existing end; setter performs no seek.",[null_sprite,"Negative inputs preserved; end0 retains duration/previous end.","First pass reaches original graph end; later loop seek uses milliseconds*10000."],movie_map)
add("PalMSpriteSetVolume",[sprite,arg("percent")],"int32_t:audio interface=>1,otherwise0; ignores COM result","q=master*trunc(percent*group4/100)/10000; convert q to DirectShow hundredths dB.",[null_sprite,"No argument clamp; zero q maps-10000; absent audio returns0."],movie_map)
add("PalMSpriteSetVolumeEx",[sprite,arg("percent")],"int32_t:audio interface=>1,otherwise0; ignores COM result","q=100*percent*master/10000; bypasses group4; convert q to DirectShow hundredths dB.",[null_sprite,"VM wrapper clamps0..100 but native export does not."],movie_map)
add("PalSetVideoVolumeType",[arg("mode","uint8_t",meaning="Only low2 bits used; occupies four-byte cdecl slot")],"int32_t changed low-bit mask (old XOR new)&3","Replaces low2 bits of video-volume selector; no immediate COM volume call.",["Other bits retained."],movie_map)
for name in ["?PalVideoSaveState@@YAXXZ","?PalVideoRestoreState@@YAXXZ"]:
    add(name,[],"void (C++ decorated symbol confirms void result)","Copies three private global words for the video state machine; does not save decoder position.",[],movie_map)
add("PalVideoPlay",[filename,arg("playback_flags","uint16_t",meaning="Low16 bits copied; four-byte stack slot")],"int32_t:0 rejected/nonzero graph-initializer result;1 playback requested","Synchronously initializes WMV/MPEG graph, applies initial gain, invokes Run without checking its result, stores IsMovie=1, posts window notifications and returns. Does not block until EOF.",["Requires initialized PAL context; path copied into private state.","The second argument is a cancellation-mask selector, not a layer or sprite slot.","No explicit existing-movie guard is present at this export boundary."],movie_map,notes=["No further codec investigation is performed in this follow-up."])
contracts["PalVideoPlay"]["state_machine"] = {
    "scope": "Exact lifecycle helpers traced in newer PAL; asynchronous public start also verified in reference and Koikake exports.",
    "initialization": "Clear remembered64-bit current/stop positions; copy flags16 and filename; graph initializer requests seeking/audio/control/event/sink interfaces, configures renderer window and notification0x8064, applies selected group/master gain, sets phase1 and duration1000ms.",
    "running": "Render/input thread keeps sampling input and processing graph cancellation. Generic PAL task traversal remains active; Game.exe normal frame callback gates its scene/script updates on IsMovie.",
    "cancellation": "flags0 mask0x01000001 (Escape/right mouse); nonzero flags mask0x01800007 (adds Space/Return/left mouse). Simultaneous physical Alt+Enter is excluded. Tests push mask, not wheel, held state or arbitrary click.",
    "fade_default": "Newer init0x100A34F0 stores context[283]=1; reference init0x100A2C20 stores context[277]=1. No config override observed in those init bodies.",
    "fade": "When context fade gate and custom renderer exist, enter phase2 using engine clock; for elapsed<duration, percent=100-trunc(float32(elapsed)/duration*100), apply live selected group/master gain times percent and renderer alpha=percent/100. Default duration1000ms. Otherwise phase3 aborts immediately.",
    "abort_delivery": "After fade/immediate abort, Clear input, send media event2 and sleep100ms. IsMovie remains1 until the window event consumer destroys graph.",
    "events": "Window message0x8064 drains and frees media-event parameters. Events1(complete) and2(userabort) destroy graph and set IsMovie0. Event3(errorabort) destroys with IsMovie1 and attempts recreation; it is not an ordinary clean finish in this native code.",
    "cleanup": "Stop graph, release filters/interfaces/custom renderer/resource handles, restore cursor state, set IsMovie to cleanup argument, restore render-thread normal priority. Does not delete or replace ordinary script sprite slots.",
    "device_reset": "Save three fade globals; destroy graph with IsMovie1; rebuild/retry, restore remembered100ns positions if nonzero, Run and restore fade globals.",
    "evidence": ["reverse/evidence/movie-audio-totsulover-async-lifecycle.json", "reverse/evidence/movie-audio-totsulover-task-gate.json", "reverse/evidence/movie-audio-totsulover-frame-gate.json", "reverse/evidence/movie-audio-video-clock.json", "reverse/evidence/movie-audio-video-lifecycle.json", "reverse/evidence/pal-platform/totsulover-helpers.json", "reverse/evidence/pal-platform/reference-helpers.json"],
}
add("IsMovie",[],"int32_t raw engine movie-state word","Returns context+0x14, not a normalized boolean.",[],movie_map)

add("PalSoundLoad",[filename,group,range_arg],"int32_t tagged sound handle;0 failure","Allocates free group record; loads Ogg when suffix matches, otherwise WAV path; group3 may stream and use optional byte-range descriptor.",["Groups4/5/6/3/2 select matching banks; other values use bank1 but preserve supplied group field.","No free slot=>0; malformed input/resource failure=>0.",invalid_handles],sound_map)
add("PalSoundLoadEx",[filename,group,range_arg,arg("gain","float",width_bits=32)],"int32_t tagged handle;0 failure","Restricted to group4; same load path, then multiplies signed16-bit PCM by gain with truncation/saturation.",["Group!=4=>0; gain<=0 or1 skips scaling; even decoded byte size required for scaling path.","Decoder-buffer lock failure/second-region behavior needs runtime comparison."],sound_map)
add("PalSoundLoadVerEpeg",[filename],"int32_t group0 tagged handle;0 open/allocation failure","Allocates one of two group0 records, reads proprietary EPEG header/table and derives streaming byte boundaries.",["Division by header count is unchecked; resource-header validity is a caller/data precondition."],sound_map,status="confirmed",notes=["Proprietary EPEG header semantics beyond observed field arithmetic remain unresolved; portable implementation mapping is missing."])
add("PalSoundCopy",[handle,group],"uint32_t new tagged handle,0 no slot/null source","Copies private sound record and duplicates DirectSound buffer; source resource pointer cleared in copy.",["Group field is copied from source, not rewritten to destination group.","DuplicateSoundBuffer failure logs but still returns allocated handle.",invalid_handles],sound_map)
add("PalSoundGetChannelCount",[group],"int32_t number of free slots","Counts free entries in requested group; invalid group defaults to group0.",["Newer excludes both loaded and reserved slots; older releases only check loaded bit."],sound_map)
add("PalSoundGetNowChannel",[],"int32_t first playing group3 channel0/1,else-1","Checks playing word of the two group3 records.",[],sound_map)
add("PalSoundGetState",[handle],"int32_t fade state0/1/2;null0;unloaded-1","Returns fade-state field; not playing flag or queued command.",[invalid_handles],sound_map)
add("PalSoundGetStatus",[arg("channel_index")],"int32_t raw group3 command word","Reads command from group3 record by unchecked index.",["Only indices0..1 are valid; no bounds check."],sound_map)
add("PalSoundPlayIs",[handle],"int32_t playing value;0 null/unloaded","Group3 returns stored playing word; other groups query DirectSound status bit1.",["Backend query failure handling is not normalized.",invalid_handles],sound_map)
add("PalSoundGetParam",[arg("channel_index"),arg("selector")],"int32_t selected group3 field/value","Unchecked group3 index; selector map in shared layouts; newer supports0..23,reference supports0..4.",["Unknown selector=>0; cursor/elapsed unavailable=>-1; cursor HRESULT failure=>-2."],sound_map)
add("PalSoundGetParamByType",[group,arg("channel_index","uint32_t"),arg("selector")],"int32_t selected field;0 invalid group/index","Bounds-checks all seven banks then shares newer selector helper.",["Zero is ambiguous between invalid input and legitimate field value."],sound_map)
add("PalSoundGetPrimaryVolume",[],"int32_t configured raw master word","Reads native master scalar.",[],sound_map)
add("PalSoundSetPrimaryVolume",[volume],"int32_t1","Stores master without clamp, then reapplies all seven group volumes.",["Each group setter clamps its group level; existing movie-sprite COM interfaces are not iterated."],sound_map)
add("PalSoundGetVolume",[arg("output","int32_t *",nullable=True,ownership="caller-owned output; one word"),arg("group","uint32_t")],"int32_t configured volume; -1 invalid group/null output","Stores and returns selected group level.",["Groups0..6 only."],sound_map)
add("PalSoundSetVolume",[volume,arg("group","uint32_t")],"int32_t1 valid group,0 invalid","Clamps group level0..10000, stores it, and updates loaded nonfading DirectSound records; some streaming command states defer update.",["Groups0..6 only; retries backend volume for up to3s engine clock.","Does not loop through MSprite graphs."],sound_map)
add("PalSoundGetVolumeChannel",[handle],"int32_t configured channel level;null0;unloaded-1","Reads channel scalar, not group/master/fade-adjusted gain.",[invalid_handles],sound_map)
add("PalSoundSetVolumeChannel",[handle,volume],"int32_t1 loaded,0 null/unloaded","Stores raw level without clamp; if not fading, recomputes group*channel gain and master dB then updates buffer.",["During fade, worker uses new level on subsequent tick.",invalid_handles],sound_map)
add("?PalSoundSetVolumeRatio@@YAHJM@Z",[handle,arg("gain","float",width_bits=32)],"int32_t1 loaded,0 null/unloaded","Stores float gain at private record+0x84 only; does not immediately rescale PCM.",[invalid_handles],sound_map)
for name in ["PalSoundSetFreq","PalSoundSetFraquencyChannel"]:
    add(name,[handle,arg("frequency_hz")],"int32_t1 loaded,0 null/unloaded; ignores DirectSound HRESULT","Calls buffer SetFrequency directly.",["No clamp or stored rate normalization.",invalid_handles],sound_map)
add("PalSoundGetFreq",[handle],"int32_t frequencyHz; -1 null/unloaded/query failure","Calls DirectSound GetFrequency.",[invalid_handles],sound_map)
add("PalSoundSetPanChannel",[handle,arg("pan")],"int32_t1 loaded,0 null/unloaded; ignores DirectSound HRESULT","Forwards signed native pan word directly to DirectSound.",["No native clamp in wrapper; backend scale is hundredths dB, not normalized stereo balance.",invalid_handles],sound_map)
add("PalSoundSetStartEnd",[handle,arg("loop_start_bytes"),arg("loop_end_bytes")],"int32_t1 loaded,0 null/unloaded","Stores raw loop byte offsets; no seek or validation.",["Different units from MSprite millisecond loop points.",invalid_handles],sound_map)
add("PalSoundPlay",[handle,arg("loop_mode")],"int32_t1 accepted,0 null/unloaded","Group0/3 queue streaming start/fade command6 with10ms duration; other groups stop prior playback, rewind buffer, start, restore gain, and mark playing.",["Raw loop word retained; worker state and backend flags may differ by bank.",invalid_handles],sound_map)
add("PalSoundPlayFade",[handle,arg("loop_mode"),arg("duration_ms")],"int32_t1 accepted,0 null/unloaded","Queues command6; stores duration,loop,targetgain; waits for worker acceptance.",["Elapsed/duration are evaluated as unsigned words by worker; negative durations are not clamped."],sound_map)
add("PalSoundStop",[handle],"int32_t1 when loaded+playing,0 otherwise","Queues command3 and waits; streaming group3 closes source and clears loaded state; other groups retain buffer.",[invalid_handles],sound_map)
add("PalSoundStopFade",[handle,arg("duration_ms")],"int32_t1 accepted/alreadyfadingout,0 null/unloaded/notplaying","Queues command7 then worker fadeout2 and stop3; alreadyfadeout+nonzeroduration returns1 without restarting.",["Zero duration can force immediate stop; negative values retain raw unsigned-duration behavior."],sound_map)
add("PalSoundPause",[handle,arg("pause"),arg("duration_ms")],"int32_t1 latch changed while loaded+playing,0 otherwise","Queues9pause/10resume; nonzero duration schedules fadeout/fadein before command handling.",["Same pause value returns0; no buffer-position rewind on resume."],sound_map)
add("PalSoundRelease",[handle],"int32_t1 accepted,0 null/unloaded","Clears group3 reservation, queuesrelease4, waits for unloaded; ordinary buffers destroyed and private record zeroed.",["Worker waits bounded by100000 Sleep(1) iterations in newer build.",invalid_handles],sound_map)
add("PalSoundReleaseEx",[group],"EAX not consistently defined for empty groups; do not treat as success BOOL","Releases every loaded record in selected bank; group3 additionally clears both reservation flags.",["Invalid group defaults to group0; observed EAX can be incoming argument/context/lastrelease result."],sound_map,status="partial",notes=["Stack arity/order confirmed; C return type remains unresolved. Requires caller/prototype evidence before ABI declaration."])
add("PalSoundSetReserved",[group,handle,arg("reserved")],"No stable EAX result; declared return contract unresolved","For group3 and handles0x20000000/1 only, stores reservation word.",["Other inputs cause no state write; EAX is incidental."],sound_map,status="partial")
add("PalSoundLoadSetReserved",[arg("unused_word"),group],"uint32_t reserved group3 handle or0","Argument1 unused; group must3; finds free/unreserved slot and reserves it without loading.",["Only two group3 slots; full=>0."],sound_map)
add("PalGetSoundBufferFormat",[handle,arg("output","PalWaveFormatEx32 *",nullable=False,ownership="caller-owned writable18-byte output",layout="PalWaveFormatEx32")],"int32_t1 querysuccess,0 invalidresolver/backendfailure","Gets DirectSound WAVEFORMATEX and copies exactly18 bytes.",["Output is not null-checked; valid loaded DirectSound handle required."],sound_map)
add("PalSoundGetPlayCursorToBuffer",[handle,arg("output","void *",nullable=False,ownership="caller-owned output, capacity at leastrequested bytes"),arg("bytes","uint32_t")],"int32_t1 unlocksuccess,0 validation/backendfailure","Reads buffer caps/cursor, locks from play cursor, copies reported locked regions, unlocks.",["Zero bytes/null output/nullhandle=>0.","Lock request is min(requested,buffer_end-cursor); do not assume it always fills requested bytes across wrap."],sound_map)
add("PalGetVoiceSpeed",[],"x87 ST0 holding value loaded from32-bit float global","Returns stored voice-speed setting; no playback consumer found.",[],sound_map,status="partial",notes=["Machine return location/value precision confirmed; original C float-vs-double declaration is unresolved without authoritative type/caller evidence."])
add("PalSetVoiceSpeed",[arg("speed","float",width_bits=32)],"x87 ST0 previous32-bit float value","Clamps/stores0..200; does not update voices or frequency in reviewed call graph.",["NaN fails nonnegative comparison and becomes0; no consumer references found outside getter/setter."],sound_map,status="partial",notes=["C return scalar float-vs-double remains unresolved."])

evidence_files = list((ROOT / "reverse/evidence").glob("*.json"))
evidence_by_build_address = {}
for path in evidence_files:
    if not path.name.startswith(("audio-", "msprite-", "movie-audio-", "mp4-")):
        continue
    try:
        data = json.loads(path.read_text())
    except (ValueError, OSError):
        continue
    if not isinstance(data, dict) or "sha256" not in data:
        continue
    for function in data.get("functions", []):
        evidence_by_build_address.setdefault((data["sha256"], function["ea"].lower()), []).append(str(path.relative_to(ROOT)))

for name, row in contracts.items():
    row["builds"] = []
    for build, info in inventory.items():
        address = info["exports"].get(name)
        if address is None:
            continue
        row["builds"].append({"build": build, "binary": info["binary"], "sha256": info["sha256"],
                              "address": address, "evidence": evidence_by_build_address.get((info["sha256"], address.lower()), [])})
    row["abi_evidence_basis"] = "Inspected stack argument loads, caller/callee cleanup, return instructions, and pointed-to memory accesses; IDA prototypes alone are not confirmation."

movie_rows = []
reference_exe_sha = hashlib.sha256((ROOT / "reverse/Game.exe").read_bytes()).hexdigest()
movie_prefix = [
    (0,"movie_play",["filename","flags_low16"],"continue; no destination store", "Filename is first pop; flags are cancellation-input mode, never a sprite layer."),
    (1,"msp_set_loop_sp_ep",["filename","slot","x","y","z","loop_mode","start_ms","end_ms"],"build-specific; see notes", "Delegates first five operands to generic sp_set_ex, then sets loop points and plays."),
    (2,"msp_cls",["slot"],"continue; no destination store", "Delegates one pop to generic sp_cls; slot-1 clears all128 generic sprite slots; other valid slots clear only that slot."),
    (3,"msp_wait",["slot"],"continue; no destination store", "One pop on first entry; zero pops while polling stored pointer. Sets loop0. Completes on statebit4 or any nonzero schedulingstate; no Stop call. Otherwise rewinds PC12 when schedulingstate&0x30==0."),
    (4,"msp_lock",["slot"],"continue; no destination store", "Movie type check then lock flag call; invalid slot/noimage does nothing."),
    (5,"msp_unlock",["slot"],"continue; no destination store", "Movie type check then unlock flag call; invalid slot/noimage does nothing."),
    (6,"msp_play",["slot","loop_mode"],"continue; no destination store", "Sets wrapper replay flag0x400000 and stored loop argument; forwards loop to native Play."),
    (7,"msp_stop",["slot"],"continue; no destination store", "Movie type check then native Stop; ignores its result; no wrapper replayflag reset observed."),
]
for build, addresses in [
    ("totsulover",["0x45b940","0x45b5d0","0x45b5a0","0x45b490","0x45b220","0x45b190","0x45b0a0","0x45b010"]),
    ("reference",["0x42e650","0x42e380","0x42e350","0x42e240","0x42e1b0","0x42e120","0x42e030","0x42dfa0"]),
    ("koikake",["0x421bb0","0x421a20","0x4219e0","0x421890","0x4217f0","0x421750","0x421670","0x4215d0"]),
]:
    for index,name,args,result,note in movie_prefix:
        if index==1:
            result = ("newer: destination=PALPlayresult; control0 on failure,1 on success; skip overrides success" if build=="totsulover" else "older: no destination store; control0 missing sprite,otherwise1")
        if build == "koikake" and index == 3:
            note = note.replace("schedulingstate&0x30", "schedulingstate&0x10")
        exe_path = {"totsulover":"target/reverse-audit/totsulover/totsulover_try.exe", "reference":"reverse/Game.exe", "koikake":"testcase/koikake.exe"}[build]
        movie_rows.append({"category":11,"index":index,"handler":addresses[index],"name":name,"build":build,
            "arity":len(args),"arguments_pop_order":args,"return_contract":result,"notes":[note],
            "abi_status":"confirmed","behavior_status":"reviewed",
            "binary":exe_path,
            "sha256":hashlib.sha256((ROOT/exe_path).read_bytes()).hexdigest(),
            "evidence":[f"reverse/evidence/movie-audio-{build}-wrappers.json",f"reverse/evidence/movie-audio-{build}-delegation.json",f"reverse/evidence/movie-audio-{build}-sprite-delegates.json"]})
for index, address, arity, name, result in [
    (8,"0x45bb00",1,"movie_set_volume_type","continue; no destination store"),
    (9,"0x45bb50",1,"movie_set_volume","continue; no destination store"),
    (10,"0x45bbd0",0,"movie_get_volume","destination=stored percent"),
    (11,"0x45bc20",1,"movie_mute","continue; no destination store"),
    (12,"0x45b2b0",1,"msp_get_state","destination=rawstate/nonmovie0/invalid-1"),
    (13,"0x45b390",2,"msp_set_volume","destination=movie1/nonmovie0/invalid-1"),
]:
    movie_rows.append({"category":11,"index":index,"handler":address,"name":name,"arity":arity,
                       "build":"totsulover", "arguments_pop_order":{8:["volume_type"],9:["percent"],10:[],11:["muted"],12:["slot"],13:["slot","percent"]}[index],
                       "return_contract":result,"abi_status":"confirmed","behavior_status":"reviewed",
                       "binary":"target/reverse-audit/totsulover/totsulover_try.exe",
                       "sha256":"4778fe70629a7c0ee6cb3caa30ce572cc45440ac392ea998b406ca45ec304483",
                       "registration_evidence":"reverse/evidence/totsulover-command-map.json",
                       "evidence":["reverse/evidence/movie-audio-totsulover-wrappers.json","reverse/evidence/movie-audio-totsulover-wrappers-extra.json"]})

movie_rows.extend(json.loads((ROOT / "reverse/evidence/bgm-reviewed-wrappers.json").read_text())["rows"])

# Runtime corrections remain distinct from reviewed native behavior.
for name in ("PalSoundPlay", "PalSoundPlayFade", "PalSoundStop", "PalSoundStopFade", "PalSoundPause", "PalSoundGetState"):
    contracts[name]["implementation_mapping"].append("crates/pal-vm/src/audio/native_sound.rs")
    contracts[name]["implementation_parity"] = "Partial: amplitude envelopes and transport lifetime corrected; production fades use the shared wrapping engine millisecond clock. Native per-tick integer gain quantization, worker cadence and command acknowledgement timeout behavior remain uncertified."
contracts["PalSoundPause"]["notes"].append("Worker0x100B07C0 handles fade state before command10. A paused buffer with nonzero resume duration stays stopped during the gain ramp, then resumes without rewind.")
for name in ("PalSoundGetParam", "PalSoundGetParamByType", "PalSoundGetStatus"):
    contracts[name]["implementation_parity"] = "Partial: scalar selectors and worker command words follow recovered field meanings. Pending 7/3/9/10 commands acknowledge on portable audio processing; native thread scheduling/timeout cadence differs. Unavailable streaming cursors, unknown source formats and resource addresses return explicit portable errors; no arena address is substituted."
contracts["PalSoundSetStartEnd"]["implementation_parity"] = "Partial: retained source PCM metadata drives initial and loop byte-range seeking. Compressed WAVE alignment, out-of-bounds native seeks, and live nonloop end-boundary extension remain unsupported."
contracts["PalSoundCopy"]["implementation_parity"] = "Partial: buffer and scalar/loop/source-group state copied with independent transport; DirectSound failures and transient worker latch copying are not emulated."
contracts["PalSoundLoadSetReserved"]["implementation_mapping"] = ["crates/pal-vm/src/audio.rs::AudioSystem::load_set_reserved", "crates/pal-vm/src/runtime.rs::load_bgm_native"]
contracts["PalSoundSetReserved"]["implementation_mapping"] = ["crates/pal-vm/src/audio.rs::AudioSystem::set_reserved", "crates/pal-vm/src/runtime.rs::ext_bgm_stop"]
contracts["?PalSoundSetVolumeRatio@@YAHJM@Z"]["implementation_mapping"] = ["crates/pal-vm/src/audio.rs::AudioSystem::set_volume_ratio"]
contracts["?PalSoundSetVolumeRatio@@YAHJM@Z"]["implementation_parity"] = "Raw float field retained on loaded records and copied independently; setter does not rescale PCM. LoadEx stores its original gain after the verified one-time amplification."
contracts["PalGetSoundBufferFormat"]["implementation_mapping"] = ["crates/pal-vm/src/audio.rs::AudioSystem::sound_buffer_format", "crates/pal-vm/src/audio/source_format.rs", "crates/pal-vm/src/abi/mod.rs::PalWaveFormatEx32"]
contracts["PalGetSoundBufferFormat"]["implementation_parity"] = "Reviewed PCM buffer formats implemented: fixed 44100 Hz stereo16 stream banks and original ordinary WAV/Ogg PCM metadata. Mixed, compressed and unsupported format outputs return explicit errors."
for name in ("PalSoundLoadSetReserved", "PalSoundSetReserved"):
    contracts[name]["implementation_parity"] = "Reviewed reservation side effects implemented: raw word, allocation exclusion, selector17/18, copy and release. VM scheduling suppression uses per-build masks; reserved PAL records do not store a deferred filename. Native setter EAX remains unresolved."

out = {
    "schema_version":1,"scope":"Native PAL audio/video exports; VM wrappers recorded separately. No codec expansion in this follow-up.",
    "primary_build":"totsulover","builds":{key:{k:v for k,v in value.items() if k!="exports"} for key,value in inventory.items()},
    "functions":list(contracts.values()),"vm_wrappers":movie_rows,
    "shared_layouts":[
        {"name":"PalSoundLoadRange32","size":16,"alignment":4,"fields":[
            {"name":"enabled","offset":0,"native_type":"uint32_t"},
            {"name":"initial_start_bytes","offset":4,"native_type":"int32_t"},
            {"name":"loop_start_bytes","offset":8,"native_type":"int32_t"},
            {"name":"loop_end_bytes","offset":12,"native_type":"int32_t"}],
         "ownership":"Borrowed synchronously by loaders; optional null; native group3 paths dereference only whenenabled.",
         "evidence":["reverse/evidence/audio-totsulover-helpers.json","reverse/evidence/audio-totsulover-streaming.json"]},
        {"name":"PalWaveFormatEx32","size":18,"alignment":2,"fields":[
            {"name":"format_tag","offset":0,"native_type":"uint16_t"},
            {"name":"channels","offset":2,"native_type":"uint16_t"},
            {"name":"samples_per_second","offset":4,"native_type":"uint32_t"},
            {"name":"average_bytes_per_second","offset":8,"native_type":"uint32_t"},
            {"name":"block_align","offset":12,"native_type":"uint16_t"},
            {"name":"bits_per_sample","offset":14,"native_type":"uint16_t"},
            {"name":"extra_size","offset":16,"native_type":"uint16_t"}],
         "ownership":"Caller-owned writable output; exactly18 bytes copied.","evidence":["reverse/evidence/movie-audio-renderer-vtables.json"]}],
    "private_layout_notes":{
        "sound_record_bytes":{"koikake":76,"reference":88,"totsulover":140},
        "group_capacity":[2,16,8,2,16,64,16],
        "group_handle_tag":["0x10000000","0x30000000","0x70000000","0x20000000","0x40000000","0x50000000","0x60000000"],
        "totsulover_get_param_offsets":{"0":36,"1":40,"2":44,"3":48,"4":52,"7":20,"8":8,"9":16,"10":24,"11":28,"12":32,"13":56,"14":60,"15":64,"16":72,"17":12,"19":4,"20":68,"21":76,"22":128,"23":124},
        "totsulover_get_param_computed":{"5":"elapsed engineclock sincefade/start, -1 unloaded/notplaying","6":"DirectSound playcursor, -1 unavailable,-2 queryfailure","18":"reserved||loaded||playing normalized0/1"},
        "reference_get_param":"Only selectors0..4 supported; returns record offsets32,36,40,44,48."},
    "unresolved":[
        "No hardware listening/device-timing comparison or decoder fidelity certification.",
        "PalSoundReleaseEx and PalSoundSetReserved stable C return types unresolved.",
        "Voice-speed exports return x87 values sourced fromfloat; original C return scalar declaration unresolved.",
        "EPEG stream format outside recovered header arithmetic remains incomplete.",
        "Malformed tagged handles are natively unchecked; adapter safety policy is separate from semantic parity.",
        "DirectSound copy/lock failures, out-of-range gain retry timing, source PCM byte metadata, reserved records, and all native race schedules are not runtime-certified."],
}
expected={name for name in inventory["totsulover"]["exports"] if any(token in name for token in ["PalSound","PalWave","PalVoice","PalMSprite","PalVideo","PalSetVideo","VoiceSpeed","SoundBufferFormat"]) or name=="IsMovie"}
missing=sorted(expected-set(contracts))
assert not missing, missing
destination=ROOT/"reverse/contracts/audio_video.json"
destination.parent.mkdir(parents=True,exist_ok=True)
destination.write_text(json.dumps(out,indent=2)+"\n")
print(f"Wrote {len(contracts)} native exports and {len(movie_rows)} reviewed VM wrapper records")
