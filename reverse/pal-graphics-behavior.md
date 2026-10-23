# PAL graphics export behavior audit

Review date: 2026-10-23. This document covers the native graphics exports in the
three release profiles, including sprite ownership, render lists, image operations,
scene effects and sprite FX. Movie/MSprite, fonts, animation objects and the EXE's
VM wrappers are separate interfaces and are outside this document's export count.

The exact per-export contracts, argument stack offsets, native widths, return
contracts, pointer obligations, evidence paths and implementation mappings are in
[`contracts/graphics.json`](contracts/graphics.json). This audit does not certify
that a portable implementation reproduces every recorded behavior. In particular,
an export body being reviewed is different from a public C declaration being fully
resolved, and both are different from a portable adapter being implemented.

## Scope and evidence

| Profile | PAL SHA-256 | Export instances covered |
| --- | --- | ---: |
| Totsulover | `744219b7c2b2ba80c5bb0440ac556201652ffbcc01d2de1211c4b576ad21db7e` | 113 |
| Reference | `e676972ddaab2d45fc82210bb89143923f10d1053492a89e50571dbcec54840a` | 76 |
| Koikake | `2a43bc4408aff086c18dc73ccd2d35be4f8a56cf8b3e82da7b75f80ffe61625e` | 67 |

The 256 records preserve exact export names, including decorated C++ names. The
selection includes every `PalSprite*`, `PalRender*`, `PalEffect*` and `PalRaster*`
export present in these inventories, plus `PalGetRender`, target-render lists,
global FX, copies, mixes, pixel operations and the standalone blur/filter exports.
There is no `PalRaster*` export in these release inventories: raster work is reached
through the FX interface. The reference-only position getters are included.

Raw export bodies are in
[`evidence/pal-graphics/totsulover-exports.json`](evidence/pal-graphics/totsulover-exports.json),
[`reference-exports.json`](evidence/pal-graphics/reference-exports.json) and
[`koikake-exports.json`](evidence/pal-graphics/koikake-exports.json). Each record has
an address, disassembly, cross-references and decompiler output. Additional helper
and draw bodies are in the same directory. IDA worked on disposable copied
databases under `target/reverse-audit/pal-graphics`; game installations were not
modified.

The hand-reviewed contract table is maintained in
[`build_contracts.py`](evidence/pal-graphics/build_contracts.py). It explicitly
describes the machine contract rather than extracting IDA's inferred signatures.
Several native functions leave incidental register values, and several empty
stubs do not establish argument arity. Those records remain `abi_status=unresolved`
even though their control flow is known. They must not become guessed callable
declarations in the central ABI manifest.

## Calling conventions and shared records

These exports use 32-bit x86 cdecl stack arguments. Pointers and ordinary words are
four bytes. Floating-point position/scale inputs are IEEE-754 `float`, not integer
coordinates. `PalMotionBlurFilter` is an exception in width: its angle occupies an
eight-byte `double` stack argument. The C++ decorated wall-spring export proves a
sixth, unused float argument that Hex-Rays omits. The decorated RGBStr export
proves ten `long` arguments; arguments one and six are interpreted as addresses.

Three public sprite-info records must remain distinct:

| Record | Extent | Important differences |
| --- | ---: | --- |
| Koikake | 76 bytes | Uniform scale; no filename or advanced tail |
| Reference | 340 bytes | Extra word at 76; 260-byte name at 80 |
| Totsulover | 372 bytes | Independent scales; layer; name at 84; physical dimensions and advanced tail |

The common first 60 bytes contain visibility, ARGB color, three float positions,
integer offsets, texture/cell sizes, integer center offsets, base priority and
scale X. `GetInfo` and `SetInfo` are field copies, not serialization of the private
sprite object. `SetInfo` does not recalculate source rectangles or render-list
ordering. Newer `GetInfo` leaves physical dimensions untouched if no texture is
attached; newer `SetInfo` ignores those output-only dimensions. The bounded name
copy writes at most 259 bytes; `GetInfo` does not guarantee that the caller's final
name byte becomes zero. The reference extra word's semantic name remains unknown.

The private sprite allocations are 136 bytes in Koikake, 400 in the reference and
424 in Totsulover. A portable Rust object or arena key is not this ABI. Public
record layouts and the 32-byte transition, 12-byte mask-image and 48-byte render
node/header records are described field by field in the JSON contract.

## Render-list ordering, locking and callbacks

`PalRenderInsert` sorts signed priorities in descending order, placing new equal
priorities after existing equal entries. `PalRenderInsertNext` inserts **before**
the first node with the supplied object pointer and appends if none matches.
Object and callback pointers are borrowed; destroying a list frees its nodes but
does not destroy their sprites or clear the global active pointer.

A draw callback receives `(core_pointer, object_pointer)` as two cdecl words. A
zero result removes its node; nonzero preserves it. The target-render API collects
borrowed object pointers, permits duplicates and null entries, and searches the
active list in that collection's order. Its temporary-render flag allows matching
hidden sprites to draw. Target rendering uses the GPU render target and restores
the ordinary target afterward.

The list's state word at offset 12 and its critical section at offset 16 are
different mechanisms. `Lock`/`Unlock` only write the state. `LockWait` polls it
with `Sleep(1)`. `LockEnter`/`LockLeave` additionally enter/leave a critical section,
and `LockIsBusy` checks its recursion count at offset 28 rather than the state.
The remembered pointer is not cleared on leave. Newer list initialization and
teardown additionally operate a global sprite/render critical section.

## Sprite state and ownership

Creation produces a hidden sprite with white modulation, unit scale, zero
rotation and offsets, render mode 1, and a full source rectangle. Metadata-only
sprites have no texture or render node; the cache-only creator returns null on a
miss rather than opening the file. Newer memory loading ignores its resource-name
argument. Texture saving takes a real filename argument even where IDA's prototype
misses it; `LoadTextureToFile` reads a host file despite the misleading name.

`Release` tears down animation/movie attachments, render node, FX, filter backup,
texture and sprite. It does not clear the caller's pointer. `ReleaseArray` traverses
backward and clears each slot; its result is **0 for all-success, 1 for any failure**.
Zero or negative count returns 0 without dereferencing the array.

Visibility is a complete 32-bit word. Storing `0x80000002` and querying it returns
that same word, although drawing treats nonzero as visible. Width/height getters
read the logical cell, while texture locking reports physical dimensions. Lock
sets the sprite's lock flag before attempting a GPU lock; failure can leave that
flag set until unlock. The lock outputs are width and height, not byte pitch.

Position setters take float X/Y/Z. `SetPos` and `MovePos` reorder using base priority
plus truncated Z, with the x86 conversion and wrapping integer addition. In
contrast, `ChangePriority` reorders using the newly supplied base priority alone.
For base 100, `SetPos(z=9.75)`, `ChangePriority(101)`, then `MovePos(dz=0)` produce
node priorities 109, 101 and 110. A permanently recomputed effective priority loses
this sequence-dependent behavior.

Old `SetScale` has one uniform float; newer `SetScale` has independent X and Y float
arguments. Integer and float rotation exports are distinct. `SetOffsetRect` only
changes cell dimensions, accepting zero, while `SetRect(null)` restores the full
texture rectangle. Newer cell selection uses signed quotient comparisons and
special-cases a one-pixel remainder; old selection uses unsigned products.

Hit testing ignores scale, rotation, center offsets and modulation alpha. It uses
visibility, integer draw offsets, truncated position and a half-open box based on
the selected source rectangle. Optional alpha testing reads the corresponding
texture texel. Foreground selection compares **base** priorities, takes the lowest,
and replaces the result on ties. A visually transformed hit box is not equivalent.

## Sprite transitions

The seven-argument setter is:

```c
int32_t PalSpriteSetTransition(Render32 *render, Sprite32 *first,
    Sprite32 *second, int32_t type, uint32_t start_time_ms,
    uint32_t duration_ms, Transition32 *handle);
```

The fifth argument is the absolute start clock and the sixth is duration. There
is no flags word. The 32-byte handle stores type, state, render, first, second,
last-frame marker, duration and start clock. For type 1, first fades in and second
fades out. Completion changes state from 1 to 2 but leaves the worker attached
until cancellation/release. A null state query returns 3. Duration zero completes
immediately. Other types are not handled by this worker and can remain state 1.

Cancellation retains the first sprite and **releases the second sprite**, unlinks
the worker and clears the handle. The setter can write state 1 before rejecting
both sprite pointers being null. Newer insertion places the worker before an
involved sprite; old builds use priority insertion. These ownership and ordering
rules must be handled at a native adapter boundary and cannot be inferred from a
portable timer named `transition`.

The portable transition adapter now declares these five exports in
`reverse/abi/graphics32.json`, preserves states 0/1/2/3, uses the absolute wrapping
clock, leaves completion attached, and releases only the second sprite on cancel.
The Game wrapper explicitly retires completed workers. Type-1 modulation uses
truncated byte alpha; the two-sprite outgoing alpha is the first sprite's alpha
minus its incoming portion. The current two ordinary textured draws still differ
from the native intermediate render-target combine and newer placement sharing.

## Pixel operations and filters

Native packed pixels are BGRA bytes representing an ARGB DWORD. Mask alpha uses
the blue channel at byte zero, not red. `PalSpriteMaskAlpha` takes a borrowed
`{width,height,pixels}` image record rather than a sprite. Its multiply is
`(alpha * blue) >> 8`, so two values of 255 produce 254. The newer path clips; old
paths require the caller's rectangle to be valid. `MaskAlphaEx` locks a sprite mask;
its empty-area path leaves that mask locked after unlocking only the destination.

RGB copy copies all four bytes, including alpha. Several legacy copy/delete
functions contain an overrun adjustment that increases an extent rather than
safely clipping it. Their invalid rectangles are native memory-corruption cases,
not a reason to add unsafe writes to Rust. Channel copy accepts unchecked byte
offsets, whereas `PalChannelCopyEx` validates channels 0 through 3, clips both
images, locks aliased images once and supports copy, saturating add and rounded
multiply `(257 * (dst * src + 128)) >> 16`.

`PalMixSpriteToSprite` preserves destination alpha. The scalar path special-cases
source alpha 255 as exact RGB copy; the MMX path still shifts the weighted sum by
eight and can darken 255 to 254. The Ex function computes retained alpha using
integer division by 255, then divides weighted RGB by the resulting alpha with
truncation. Rounded generic source-over arithmetic is not identical.

Filter options preserve the original image on the first nonzero option. Repeated
nonzero filters transform current pixels; zero restores and frees the backup.
Unknown nonzero options still allocate that backup and succeed. Options 1/2 are
palette transforms, 3/4 are native kernels, and `0x105` through `0x805` are half-size
blur paths. The reference GPU path alone additionally recognizes `0x905` as a
full-size blur.

Standalone box blur uses edge clamping, transposed intermediate storage and
truncating channel division. Motion blur takes a double angle, uses 16.16 stepping,
averages alpha and alpha-weights RGB; samples outside the image are transparent.
AlphaCopy is a modulation/premultiplication operation, not source-over blending.
StretchImage mode 0 uses fixed-point nearest RGBA sampling, mode 1 calls a byte-plane
interpolator and mode 2 calls the four-channel interpolator. Unknown modes write
nothing and return success.

The newer waveform, displacement, swirl and wall-spring functions have individual
contracts in the JSON. Their argument widths, tiling, channel choice, aliasing and
rounding should be retained rather than replaced by generic effects with similar
names. Native unchecked buffer obligations are recorded explicitly.

## Native GPU draw modes and shader routing

The newer draw-state switch at `0x100CC860` writes these D3D9 values. The numeric
interpretation was checked against the installed MinGW `d3d9types.h`; the raw state
writes are preserved in `totsulover-draw-helpers.json`.

| Mode | Operation | Source factor | Destination factor |
| --- | --- | --- | --- |
| Default | ADD | SRCALPHA | INVSRCALPHA |
| 2 | ADD | SRCALPHA | ONE |
| 3, 5 | ADD | ZERO | SRCCOLOR |
| 4 | ADD | ZERO | DESTCOLOR |
| 6 | ADD | INVDESTCOLOR | ONE |
| 7 | MIN | ONE | ONE |
| 8 | MAX | ONE | ONE |
| 9 | ADD | ONE | ONE |
| 10 | REVSUBTRACT | ONE | ONE |

Mode 4 therefore multiplies destination by destination; it is not an inverse-color
operation. Mode 11 disables alpha blending and selects texture-stage argument 1
(texture), with source/destination blend states still written as SRCALPHA and
INVSRCALPHA. These are the newer GPU path's settings; software-path equivalence and
all D3D alpha-channel consequences require separate comparison. The reference and
Koikake blend dispatchers recognize only modes 1 through 4; the portable sprite
adapter scopes the extended modes to the newer profile.

The newer ordinary draw helper restores the blend operation and factors after a
non-default draw but does not restore `ALPHABLENDENABLE` or stage-0 `COLOROP`.
The normal render walk and presentation helper do not restore them either. A
mode-11 sprite therefore changes following sprites, including in later frames:
they retain their mode words but draw without blending or RGB tint until a reset.
`PalTargetRenderListDraw` differs: it calls `0x100CACA0` before every callback and
again after the list, restoring blending and texture/diffuse modulation. Evidence
is retained in `totsulover-blend-reset.json`, `totsulover-render-walk.json`, and
`totsulover-render-end.json`, in addition to the draw helpers.

The portable engine now resolves this state after final render-priority ordering.
`SpriteDraw` and the separate host C scene view carry `blend_disabled` and
`texture_rgb` independently from `render_mode`; ordinary frame composition keeps
the state across frames. Transparent sprites are not removed before the walk,
because a prior mode-11 node can make their RGB visible. The target-list adapter
uses the same persistent state and resets it before every callback and after the
walk, including empty lists. Registered newer VM wrappers 3:82/83/84 now clear,
add and draw that collection, then copy its RGBA capture into the parent sprite.

Software composition, wgpu pipelines, Switch deko3d blend state, and Vita GXM
fragment-program variants consume the resolved state. Vita reuses vita2d's linked
precompiled texture programs and creates blend-state variants through the shader
patcher, so these modes require no runtime shader compiler module. The current
CPU presentation keeps alpha opaque; its RGB equations follow the native D3D path,
not every native DIB implementation. Console package builds do not certify hardware
output. See [render-mode implementation](render-mode-implementation.md).

The shader dispatcher at `0x100CE160` uses draw pass 0 for
`DrawPrimitiveUP_2D`, pass 1 for raster parameters `offset`, `undulate_x`,
`undulate_y`, and pass 2 for radial parameters `g_TextureWidth`, `g_TextureHeight`,
`offset_x`, `offset_y`, `level`. **Draw pass IDs are not source FX type IDs.** A
source-type declaration alone does not connect the renderer's dispatch.

The three release `FX.dll` files are byte-for-byte identical: 11,776 bytes with
SHA-256 `46e98ad11e2c43447bcc9faa74afc0893b7e5e8ecd9cbfda733cc6532508d99b`.
Thus the existing resource/bytecode extraction applies to those same FX binaries,
while the PAL invocation paths and their supported IDs remain build-specific.
The existing WGSL files are translations to review against this dispatch, not
evidence that all exported rendering operations reach the correct shader.

## Implementation implications and remaining uncertainty

Corrections implemented during this audit include transition arguments/lifetime,
separate newer X/Y scales, sequence-dependent priority updates, wrapping offset
addition, zero-sized cell storage, and the mask's blue channel. Snapshot v12
preserves independent Y scale for scene sprites and action bindings; readers keep
older uniform-scale snapshots compatible. The scale ABI has separate legacy and
Totsulover aliases with exact per-build evidence. Negative/nonfinite transformed
geometry remains a renderer difference even though its source values are retained.

The Ex pixel-blit adapter now implements retained-alpha/channel truncation and
clears the output when both alphas are zero. It safely snapshots overlapping
sources and rejects nonpositive extents, rather than reproducing invalid native
pointer access.

The newer target-render collection preserves duplicates, null entries, insertion
order and hidden-sprite drawing. Its offscreen alpha follows the recovered D3D
factors. Snapshot v13 appends attachment/render lanes, ordered stable target
references, capture pixels and independent blend flags. The native Add/Clear
source return types remain unresolved; their internal adapters do not invent
native callable declarations. `PalChannelCopyEx` and `PalSpriteHit` also have
confirmed declarations and implementations.

Remaining differences include complete visibility words, the native MaskImage32
boundary, build-specific SpriteInfo adapters, transition intermediate render
targets and insertion order. Foreign render callback pointers, blocking locked
sprites, driver failure residue and physical D3D target padding remain outside
the portable target-list representation. The machine-readable contracts retain
these distinctions; some advanced filters still have no native export adapter.

This audit establishes export-boundary coverage for the assigned families. It does
not turn incidental EAX values into return types, recover argument arity from an
empty stub, prove every driver/OS failure path, or certify GPU/CPU/console pixel
parity. The large newer GPU draw routine has an IDA stack-analysis failure; its raw
disassembly and called transform/shader helpers are retained, and that decompiler
failure must not be hidden by presenting its prototype as confirmed source.

Confirmed callable declarations for the implemented corrections are in
`abi/graphics32.json`, included by the central `abi/pal32.json` manifest. Regenerate
the C and Rust declarations with `python3 reverse/abi/generate.py` and verify them
with `python3 reverse/abi/generate.py --check`. Unresolved return types and empty
stub arities remain in the audit rather than becoming guessed declarations.
