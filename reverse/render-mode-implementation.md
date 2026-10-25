# Native render modes and target-list implementation

The native ABI declarations are in `abi/graphics32.json`. Exact per-build evidence
and remaining boundaries are in `contracts/graphics.json` and
`pal-graphics-behavior.md`.

## Mode state

Koikake and reference D3D accept modes 1 through 4; Totsulover extends the switch
through 11. The portable adapter keeps the stored native word and scopes backend
operations by profile. The newer mode-11 callback leaves alpha blending disabled
and RGB texture selection enabled. Ordinary render walks preserve those states
across nodes and frames; target-list walks reset before each callback and afterward.
`SpriteDraw` carries the two independent state flags, and `SenaSceneDraw` transports
them as separate 32-bit host API fields. This host interface is not the native PAL
DLL ABI.

The CPU compositor and wgpu normal/raster pipelines implement the recovered D3D
RGB equations. Switch uses deko3d factors and operations. Vita creates GXM blend
variants from the linked vita2d precompiled texture programs and selects the
appropriate variant per draw. Neither Vita build nor runtime requires a shader
compiler module for these programs. Vita raster effects still use the existing
CPU fallback; this work does not certify device output.

Before the user's instruction to stop adding regression tests, the implementation
passed four mode/state pixel checks, 30 sprite fixtures, a scene-FFI transport
check, and 176 real wgpu readbacks across 11 modes, four independent state-flag
combinations and four raster/rotation cases. Maximum mean RGB byte error was
0.013264973958333334. Both native console packages built with this host-scene ABI.
Subsequent work uses compilation and actual game runs only.

## Target lists and script dispatch

`SpriteSystem::target_render_list_add/clear/draw` implements the observed borrowed
sprite collection. Duplicate and null entries remain ordered; hidden sprites draw
in temporary-target mode. Transition-owned sprites remain suppressed. A temporary
RGBA target clears to transparent black and uses the native D3D blend factors for
alpha as well as RGB; ordinary presentation still forces output alpha opaque.
`render_copy` preserves all channels and returns success for valid destinations
when capture is unavailable or undersized, following the native export result.
Larger portable destinations center logical pixels with transparent padding.

The newer Game.exe registers category 3 indexes 82, 83 and 84 at
`0x454810/0x454821/0x454832`, pointing to `0x44F650/0x44F6E0/0x44F960`.
The binary SHA-256 is
`4778fe70629a7c0ee6cb3caa30ce572cc45440ac392ea998b406ca45ec304483`.
They pop `(parent)`, `(parent, source, lane)` and `(parent, lane)` respectively;
none writes a VM destination. `runtime/children.rs` routes these wrappers to the
collection, records mode 17/18 flags using native OR semantics, cancels source
transitions and commits source wrapper transforms before capture. The normal
five-argument `sp_set_child` remains the separate attachment operation; it shares
and clears the corresponding lane bookkeeping.

The attachment setter and consumer are now captured for all three builds:
Koikake `0x41AA50 -> 0x41D870 -> 0x435070`, reference
`0x424A20 -> 0x4240F0 -> 0x448900`, and newer
`0x447860 -> 0x476F80 -> 0x47A310`. Each walk visits parent slots 0 through 63
and eight child lanes. The child's shared auxiliary bank stores
`parent_base_scale * child_base_scale - parent_base_scale`; later parent scale
changes add that saved delta, rather than multiplying by a captured child factor.
Newer X and Y scales are independent. Colors add each captured child byte minus
255 to the parent's effective color and clamp. Rotation differences are retained,
Z inherits the parent without accumulating previous-frame Z, and center
compensation truncates the scaled center difference before integer subtraction.
PAL sprite image offsets remain separate from wrapper positions.

Portable snapshot v13 appends child attachments, shared child auxiliary banks,
render-lane records, ordered
stable target references, the last RGBA capture and independent draw flags after
the v12 action payload. Existing v1 through v12 read order is unchanged. Restoring
the captured pixels avoids blending an already-captured destination a second time.

The native Add and Clear bodies leave incidental EAX values; their source return
types remain unresolved. They are typed internal adapters, not guessed native C
function declarations. The current render tree models sprite objects, so foreign
function-pointer callbacks are outside its arena interface. Locked sprites are
skipped rather than blocking a host thread indefinitely. Native driver-lock errors,
physical D3D padding and every native DIB path are not reproduced by the shared
CPU offscreen renderer. Original-engine save-image child records are distinct from
the portable v13 tail. The complete native image-wrapper animation bank is not
represented by the child adapter: arbitrary repeated reattachment interleaved with
base-field writes, every legacy integer-angle versus newer float-angle mode, and
all independent auxiliary animation lanes still require a full wrapper-state
representation. These are separate from the confirmed attachment equations.

## Additional corrected graphics boundaries

`channel_copy_ex` maps native BGRA channel numbers to RGBA storage and implements
clipping against both images, byte copy, saturating addition and rounded product.
Aliased surfaces update in native row-major order, retaining overlap feedback;
invalid overflow coordinates are rejected safely.

`SpriteSystem::hit` uses the selected unscaled rectangle, wrapping position math
and native float-to-integer conversion. It ignores rotation, scale and modulation
alpha. Any nonzero alpha-test word checks the selected texture texel. Native
missing/out-of-range pixel reads return opaque `0xFFFFFFFF`, so those geometrically
valid hits remain accepted. Button-specific flag conversion belongs to its caller.

## Actual-run validation

The isolated Totsulover resource root reached the CRYSTALiA logo after 360 frames
and the trial title splash after 1,800 frames at a fixed 16 ms step. The final
release binary also completed 2,400 frames after the child corrections, with
seven title-scene draw commands and exit status 0. Its log surfaced two separate
history command gaps: 14:13 `history_face_enable` at PC `0xABEB8` and 14:24
`history_secondary_icon_enable` at PC `0xAA320`; those were reported to the history
implementation work. Only the isolated root owns save output; original game
files were not changed. These runs establish boot behavior, not coverage of every
attachment mode or target-list branch. Native Switch NRO and Vita VPK builds also
completed after the final child corrections and the concurrent v14 save update;
device execution remains unverified. Artifact hashes and exact logs are recorded
in `evidence/pal-graphics/portable-final-validation.json`.
