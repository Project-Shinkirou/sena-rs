# Newer SoftPAL audit: totsulover_try

Audited 2026-10-23 using IDA 9 against copies in `target/reverse-audit/totsulover`.
The original installation was read-only. This report establishes specific
interfaces and differences; it does not certify complete engine parity.

## Build identity

- EXE SHA-256: `4778fe70629a7c0ee6cb3caa30ce572cc45440ac392ea998b406ca45ec304483`.
- PAL SHA-256: `744219b7c2b2ba80c5bb0440ac556201652ffbcc01d2de1211c4b576ad21db7e`.
- PAL exports: 369; EXE imports from PAL: 271.

See [PE inventory](evidence/totsulover-interface-inventory.json).

## VM dispatch and version boundaries

The primary table is `0x4EB0A0`; the category-pointer table is `0x4D25B8`.
Opcode 23 at `0x458D80` still reads the packed category/index and destination,
with high 16 bits selecting category and low 16 bits selecting index. Core
primary dispatch appears in `0x47BC00` and `0x47C240`.

The EXE additionally registers commands through `0x4046F0`. All registrations
with a nonzero category table agree exactly with the table's function pointer.
Category 1 contains the primary-core registrations; its extcall table is null.
Registration calls contain duplicates; use deduplicated coordinates for counts.

- [Registration call disassembly](evidence/totsulover-dispatch-registration.json)
- [Machine-readable command map](evidence/totsulover-command-map.json)
- [Table comparison](evidence/totsulover-table-verification.json)
- [New-build primary aliases](evidence/totsulover-primary-aliases.json)
- [Core dispatch evidence](evidence/totsulover-primary-control.json)
- [Reviewed startup call signatures](evidence/totsulover-reviewed-extcalls.json)

Do not reuse the old high-primary alias map for this build. Do not infer new
handler arities from the old name catalog: `18:127` is a zero-argument window
mode timestamp query, and `9:63` is zero-argument `is_active`. Misidentifying
these as tangent/system-button configuration consumes unrelated stack words.
`18:121` remains the zero-argument wrapping millisecond clock. `12:0` and
`12:1` are system-button set/release, not sound playback.

## Archive profiles

`0x47E1D0` initially sets the `data` search path and opens `archive.dat`, with a
`csv` fallback. It selects one comma-separated field by its ECX profile index,
trims CR/LF/space/tab, then registers that field's pipe-separated paths. An
out-of-range field fails; the implementation must not merge profiles.

Both native startup callers select profile 0. Extcall `9:44`, `0x465340`, pops
one profile index, calls `PalSystemPathClear`, then reloads that selected
profile. Consequently trial resources can become available after script-driven
profile selection without an engine-specific game-name heuristic.

Evidence: [loader](evidence/totsulover-boot-handlers.json),
[selection wrapper and tokenizer](evidence/totsulover-dispatch-core.json).

## Existing PAL contracts

The eleven previously reviewed declarations retain their x86 cdecl argument
shape in this DLL. Body/disassembly evidence is in
[PAL contract audit](evidence/totsulover-pal-contracts.json). This does not mean
all bodies retain old behavior:

- `PalSpriteRectSetPos`, `0x100B8DC0`, compares signed cell indices with the
  unsigned dimension quotient stored as a signed word. Negative indices remain
  negative; they do not trigger the old unsigned product clamp. For an excessive
  index, dimension remainder 1 selects `cell_size * (cell_count - 1)`; otherwise
  it selects `texture_size - cell_size`. Multiplication retains 32-bit wrapping.
  Keep this behavior versioned; older DLLs use a different algorithm.
- `PalEffectEx`, `0x100A9050`, accepts nonzero effect IDs below 100, compared
  with 50 in the reference build and 30 in Koikake.

The fresh evidence is ready for reviewed manifest integration. It is not an
instruction to promote every IDA-inferred return type to a confirmed contract.

## Movie sprite interface

`PalMSpriteLoad`, `0x100AED90`, has three cdecl 32-bit arguments: a render-object
pointer, a filename pointer, and signed render priority. It returns an allocated
sprite pointer or null. The movie object is attached at sprite offset 20; the
sprite is inserted into the supplied render tree. `0x100AE920` consumes the
filename as a narrow string (`strlen`) and calls the common movie initializer.
This interface is not inherently WMV-only. Filename lifetime beyond the loader
and every accepted codec/container remain separate checks.

`PalMSpriteInfoCreate`, `0x100AECF0`, has four words: optional filename pointer,
fallback width, fallback height, and sprite flags. When a filename is supplied,
it opens metadata, takes the movie dimensions, and closes the temporary movie.

Evidence: [exports](evidence/totsulover-msprite-contract.json),
[filename loader](evidence/totsulover-msprite-loader.json).

## New sound and desktop APIs

Fresh evidence includes `PalSoundSetFreq`, `PalSoundGetFreq`, voice-speed APIs,
blur/copy filters, and desktop sizing. Frequency access uses a native channel
identifier and two/one stack words respectively. The sound backend forwards
frequency to its buffer interface. Voice-speed setter clamps its float input
into 0..200 and returns the previous stored value through x87; the source-level
float-versus-double return type is not proven solely by that register.

Desktop setters/getters have incidental register values on some paths. Their
source-level return contracts remain unresolved; do not copy inferred LONG or
LPARAM returns into the confirmed ABI manifest without caller evidence.

## Remaining verification

Only the explicitly reviewed startup handlers have checked arities here.
The full command map proves handler addresses, not semantic completeness.
Complete new-build parity still requires checking all used coordinates, versioned
primary aliases, negative/overflow sprite cells, sound speed/frequency behavior,
new filters, original save layouts, and native rendering captures.
