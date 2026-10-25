# PAL text, number, thumbnail, and animation contracts

This audit covers every present export in these families in the three release
DLLs: 41 Koikake, 41 reference, and 46 totsulover entries. The machine-readable
inventory is `contracts/text_animation.json` (128 export/build rows, 46 unique
names). The debug DLL is not one of these profiles. An inspected export boundary
is not a claim that all downstream helpers or the Rust implementation match.

Evidence is in `evidence/pal-text-animation-{profile}.json`, with separate
`helpers`, `kernels`, `glyphs`, `extra`, and `callers` captures. Each records the binary SHA-256.
The call-site files preserve actual game import references, surrounding machine
instructions, and caller pseudocode. IDA ran only on private database copies in
`target/reverse-audit/text-animation`; the installed games were read-only.

## ABI boundaries

Exports use Win32 x86 cdecl and four-byte stack slots. A native sprite or task
pointer is a 32-bit address, not a portable arena handle. Font setters consume
16-bit size/effect values, but `PalFontSetType` compares the full 32-bit input
against 4 before narrowing. Consequently, 0x10004 and 4 take different gates in
Koikake/totsulover. The three font setting getters return AX only; their callers
store or zero-extend AX. `PalFontHasDrawTextRect` returns AL only.

Getter/setter names do not establish an integer return type. In particular,
`PalFontGetColor` leaves a destination pointer in Koikake but a color value in
the optimized DLLs. Both-null outputs need not define EAX. Font End/Unload and
several number-array operations also leave incidental values. Those source
returns remain unresolved rather than being declared as booleans. The new
`PalFontGetDrawTextRect` copies 20 bytes; its EAX happens to equal the fifth word,
but the game discards it. Its output is four signed rectangle words followed by
a glyph width, not a 16-byte RECT.

The inventory separates `abi_status`, `behavior_status`,
`downstream_behavior_status`, and `implementation_gaps`. `reviewed_export_boundary`
means the export's argument use and control paths were inspected. The PAL-owned
glyph lookup/rescaling, GDI result handling, clipping, malformed resource paths,
and thumbnail GPU readback have now been traced. Windows GDI's own face metrics
and graphics driver results remain external platform boundaries; this does not
establish Rust parity. Invalid native memory accesses are documented rather than
executed or reproduced.
The central reviewed declaration manifest remains `abi/pal32.json`; the companion
`abi/animation32.json` is prepared for integration by the ABI owner.

## Animation scheduling and data

`PalAnimation` allocates zeroed 28-byte task data and installs an initialization
callback. Creation stores sprite, delay, normalized flags, and running=1, but
neither resets the source rectangle nor samples time or dimensions. On the first
scheduled visit, the callback reads the current sprite dimensions and cells,
computes a signed quotient, stores its low 16 bits as frame count, clears current
frame and loop latch, and samples cached PAL time. It then installs the update
callback. Thus an animation first scheduled long after creation must not jump
immediately, even with delay zero; that visit initializes only.

Subsequent updates use unsigned wrapping elapsed time, advance at most one u16
frame per visit, and discard elapsed remainder by setting last_time=now. Only
exact equality with the u16 count wraps to frame zero and sets the one-shot loop
latch. A quotient of 65536 stores zero; the native implementation does not clamp
it to 65535 or one. Zero cell dimensions cause a native division fault and are
not a safe input requirement for the port.

`PalAnimationEx` borrows a live array of 24-byte records: RECT at offsets 0..15,
u32 duration at 16, and i32 next index at 20. It applies frame zero at creation,
but duration and timestamp are initialized on the first task visit. Stop/Start
change only the running word, preserving the clock. ResetEx replaces the array,
sets frame zero/running, applies its rectangle, and samples duration/time, without
replacing whichever task callback is currently installed. On terminal next=-1,
the native updater reads the preceding record's duration before its sentinel
check; the portable implementation intentionally avoids that invalid read.

Sheet Reset selects cell(0,0), clears the frame and loop latch, and samples time
without restarting or recomputing dimensions. GetAnimationPoint reads data+16,
which is the flags word for a sheet, not its frame number. GetTime reads data+4,
which would expose the frame-array pointer for Ex; the portable typed API
intentionally returns None there. Untyped misuse of native task payloads is not
reproduced by safe Rust enums.

## Font profile differences

Koikake has one active bitmap font. Totsulover caches 44-byte entries by exact,
case-sensitive filename. Duplicate loads return success without changing the
active entry. For type 4, FontBegin chooses the first exact base-size match;
otherwise the smallest base size if the request is below all entries, or the
largest base size. This is not nearest-size selection. Its FontUnload frees the
list but leaves copied active-font fields stale; Rust must preserve safe ownership
instead of recreating the resulting use-after-free possibility.

The reference DLL is a localized variant: SetType(4) stores 1, and Begin also
rewrites 4 to 1. Type 1 selects SourceHanSansCN-Medium, other types select
SourceHanSerif-Medium. It creates a face with height int(size*1.5), width size,
and bold weight only for type 1. Its DrawText helper reads one UTF-16LE code unit
per call; Game.exe iterates the text. Koikake and totsulover instead iterate a
Shift-JIS string in the PAL helper. Totsulover additionally supports backslash
quoting and multi-digit percent glyph indices.

GetSize includes native padding: width+2 and height+2 in Koikake/reference;
width+3 and height+2 in totsulover. CR and LF each add a line. ASCII and full-width
spaces have explicit widths; percent escapes refer to the extended glyph strip.
Native SetFontSize stores zero unchanged. The portable font now preserves zero,
uses a named cache for totsulover, and requires bitmap data for the type-4 gate.
Logical ADV metrics remain a separate convenience API from native GetSize's
outer padding and encoded parser.

The extended-font loader reads width/height from offsets 12/14, skips 18 bytes,
and reverses 8-bit rows. Height defines square glyph size. It does not honor a
general TGA orientation field. The raw PAL loader preserves old state on open
failure; all three Game.exe `load_font_ex` wrappers call Unload before Load, so
failed VM replacement correctly clears the old strip. These are distinct
interfaces and must not be conflated.

### Bitmap records and scaling

The last 56 resource bytes are native text metrics. Signed height and ascent at
offsets 0 and 4 determine scale and baseline; no glyph histogram is involved.
Lookup uses the low byte directly for one-byte codes. A nonzero high byte is
increased by 0x80 with eight-bit wrapping, multiplied by 255, and added to the
low byte to index the offset table. The native code does not treat offset zero
as a missing-glyph marker or check the record against the resource extent.

Each glyph has a 24-byte header: unsigned black-box width/height, signed x/y
origins, **two signed 16-bit cell advances**, then unsigned bitmap length. At
scale one, the metrics and coverage are copied directly. Otherwise native code
truncates float-scaled fields, replacing a zero result with one. Original row
stride is `(bitmap_length / height) & ~3`. Scaled width is derived from that
padded stride, not the original black-box width. Koikake/reference align scaled width
to four before resampling and use that width in the sampling ratio. Totsulover
resamples the unaligned width and then pads its rows. Horizontal advance is
raised to the scaled width using signed 16-bit comparisons. Reference/totsulover
leave metrics unchanged for zero-length glyph bitmaps; Koikake still enters the
rescaler and can divide by zero.

Resampling uses eight-bit fixed-point bilinear interpolation. Axis increments
are `trunc(256 / scale)`, with accumulators beginning at zero. The four weights
use the low eight accumulator bits and their complements to 256, and the final
sum is shifted by 16. Missing right/bottom neighbor pixels are zero. This differs
from both nearest-neighbor sampling and edge replication. Extended strip bytes
first convert to six-bit coverage with `(byte + 1) >> 2`. Totsulover replaces
invalid extended indices with an opaque square; the older profiles crop without
an index bounds check.

### Native failures and safe portability

FontBegin ignores CreateFont, CreateCompatibleDC, SelectObject, and GetTextMetrics
failures and still returns one. GetGlyphOutline query/fetch failures likewise do
not reach a meaningful PAL error return: subsequent code can use stale metrics
or the all-ones error value as a byte count. Totsulover's temporary allocation
failure during row repacking logs an error but returns success with unrepacked
bytes. A portable checked parser and explicit fallback must not reproduce these
invalid accesses.

ExFontLoad has one deliberate failure branch: opening the resource fails.
After opening, it ignores ReadFile success and the actual byte count, subtracts
44 from file size with 32-bit wrapping, and does not test allocation success.
Header dimensions can therefore come from stale scratch bytes. Its reverse-row
copy uses do-while loops without zero-dimension guards. Truncated data, zero
dimensions, or failed allocation can fault instead of returning a useful error.
Returning an error for these malformed inputs is an intentional safe-port
difference, not an unresolved native control path.

## Font pixel kernels

Plain glyph placement adds (1,1). Effect 1 ORs the four cardinal outline pixels,
not the eight neighbors. Effect 2 stamps a shadow one pixel down/right relative
to the foreground. Reference/totsulover effect 3 ORs the 12 radius-two diamond
neighbors and shifts the foreground an additional (1,1). Koikake has no effect-3
branch. Unsupported IDs draw no glyph while returning success.

Nonzero six-bit coverage is expanded as `(4*coverage-1)&255`. Native stamping ORs
coverage into the color word's alpha byte; it does not multiply requested alpha.
Blending over an existing nontransparent pixel uses weights alpha and255-alpha,
then divides by256. Full coverage or a transparent destination directly stores
the color OR the coverage alpha; blended source alpha is 255. Native right/bottom
clipping uses inclusive counts and does not skip clipped source row tails.
Several kernels lack left/top clipping and have do-while loops that are unsafe
for empty extents. Safe bounds checks are required around the recovered in-range
pixel arithmetic. The portable raster now uses these recovered kernels and alpha
rules. An ab_glyph fallback face still does not have GDI-identical metrics or
coverage.

The totsulover last-draw rectangle tracks the last glyph or space, not the union
of a string. Each DrawText clears its validity flag but retains stale rectangle
bytes. The fifth output word is bitmap black-box width for ordinary glyphs and
logical cell width for spaces/extended glyphs. This state drives Game.exe's text
animation threshold construction.

## Portable font implementation and validation

`font/bitmap.rs` validates complete lookup records and the 56-byte footer,
retains signed cell increments, and implements fixed-point resampling with the
profile's row alignment. Resource bytes and extended strips are shared immutable
allocations, so cloning a drawing context does not duplicate entire font files.
`font/raster.rs` implements the native pixel kernels with checked clipping.

`font/native.rs` provides checked encoded GetSize/DrawText adapters, native BGRA
caller surfaces, extended strip loading, effect step queries, and the 20-byte
last-draw state. Koikake consumes one byte after percent; totsulover parses a
wrapping decimal index and supports backslash quoting. Native tags recognize
only `br` and `BR`; CR/LF are separate breaks. The reference adapter retains its
one-UTF-16-unit DrawText behavior and its inconsistent ANSI tag scanner and
percent index source. Invalid surrogate/crop/buffer accesses return errors.

Koikake/totsulover markup and percent glyphs are connected to the ordinary text
path. The reference ADV renderer still aggregates a line using its portable
TrueType face; the separate native adapter models the per-unit export. Profile
selection is `registered_softpal -> totsulover`, otherwise multilingual text
metadata selects the localized reference, otherwise Koikake. This is a heuristic
for the audited releases, not general engine-version detection. Initial font
selection runs before configuration and resource loading.

Remaining limitations are external GDI face/raster equivalence, native unsafe
buffer behavior deliberately replaced with errors, resource mutation during an
explicitly begun context, and fonts not yet carried in portable save snapshots.
These are implementation acceptance limits, not unreviewed native branches.

Validation: 29 focused font/runtime tests passed, along with all six public font
fixtures. The explicit installed-resource smoke test passed read-only against
both games and rendered inspected PNGs under
`target/reverse-audit/text-animation/`. Koikake's actual footer is height 29,
ascent 25; totsulover's is height 36, ascent 31. Both native extended strips also
loaded and rendered percent glyphs. No game files were modified. The smoke test
is ignored by default because installed game resources are optional.

## Numbers and thumbnails

Number-array index zero is the rightmost/least-significant digit. NumberSet
positions index i at `x + pitch*(count-1-i)`, with native integer wrapping before
conversion to float. NumberDraw hides the supplied count first. Unless flags bit1
is set, its actual draw count is decimal digits plus a sign column when bit0 is
set, so it may exceed the supplied count. The final column priority is sign
(cells10/11), then bit2 cell12, then bit3 cell13. INT_MIN wraps during absolute
value and may produce negative remainder cell indices. No dedicated portable
number-array implementation was found; sprite primitives alone do not establish
coverage of these exported helpers.

Native thumbnails sample a composed backbuffer or caller pixel array. Sampling
uses a 7-fraction-bit accumulator with increment `trunc(abs(128/scale))`, where
scale and output extent use native floating-point calculations. Complete BGRA
words are copied without blending because source screen composition is already
done. Mosaic reduces to a destination/factor-sized intermediate and expands it
with the same sampler; it is not merely output-coordinate snapping when sizes
and factor do not divide evenly. The ignored second argument of the screen-copy
variants remains a native four-byte slot. Explicit-source variants still require
the display-initialized flag.

The GPU readback helpers are captured in `evidence/pal-platform/*-capture.json`.
They reject destinations smaller than the logical display, obtain staging
surface level zero, copy the current scene render target, and lock with
READONLY=16. Any nonzero HRESULT from those steps returns zero; later failure
releases the acquired surface. Unlock/Release errors are ignored. The source
starts at `pitch*((physical_height-logical_height)>>1) +
((4*(physical_width-logical_width))>>1)`, but later rows advance by
`physical_width*4`, not the reported pitch. Logical pixels are copied with alpha
ORed to opaque; extra destination rows/columns remain unchanged. This captures
the engine scene, not desktop window decorations.

The current portable thumbnail helper scales individual sprites and copies
nonzero-alpha pixels, so transformed/effected/partially transparent overlaps are
not equivalent to the composed native screen. Newer CreateNull writes opaque
black independent of display initialization; the existing portable thumbnail
canvas begins transparent. These are explicit implementation gaps, not native
behavior uncertainties.
