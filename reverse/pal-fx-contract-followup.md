# PAL FX lifecycle contract follow-up

This audit compares the Koikake PAL DLL and the reference release PAL DLL. It
covers type-0 raster state ownership and export control flow, not complete FX
rendering or save-state parity. The reviewed declarations belong in
`reverse/abi/pal32.json`; that manifest generates the C header and Rust native
function-pointer types. Portable methods use arena keys and are not DLL exports.

## Native interfaces

All arguments occupy one 32-bit x86 stack slot and callers clean the argument
stack (`__cdecl`). Both builds return the stated 32-bit result in EAX. The export
body, stack accesses, instruction return and helper behavior were inspected,
rather than accepting an inferred IDA prototype alone.

```c
int32_t __cdecl PalSetFxEffect(
    const int32_t *target, uint32_t effect_flags, int32_t action,
    int32_t duration_ms, const void *source);
int32_t __cdecl PalSpriteSetFxEffect(
    PalSpriteOpaque *sprite, uint32_t effect_id, int32_t action,
    int32_t duration_ms, const void *source);
int32_t __cdecl PalReleaseFxEffect(
    const int32_t *target, uint32_t effect_flags);
int32_t __cdecl PalGetFxEffectState(
    const int32_t *target, uint32_t effect_flags);
int32_t __cdecl PalSpriteGetFxEffectState(PalSpriteOpaque *sprite);
```

The `const` qualifications describe observed caller-buffer access, rather than
recovering lost source-level typedefs. `effect_flags` is a raw bit carrier. The
global bit is `0x10000000`; generic start strips the upper nibble before indexing
the type tables. The sprite-specific setter passes the whole type word onward.

| Export | Koikake address | Reference address |
| --- | --- | --- |
| PalSetFxEffect | `0x100B3B10` | `0x100A8B90` |
| PalSpriteSetFxEffect | `0x100C47C0` | `0x100B1FE0` |
| PalReleaseFxEffect | `0x100B3C30` | `0x100A8AA0` |
| PalGetFxEffectState | `0x100B3D20` | `0x100A8A70` |
| PalSpriteGetFxEffectState | `0x100C4880` | `0x100B1FD0` |

Build hashes, disassembly and pseudocode are recorded in:

- `evidence/pal-abi-koikake.json` and `evidence/pal-abi-reference.json` for setters.
- `evidence/pal-fx-lifecycle-koikake.json` and
  `evidence/pal-fx-lifecycle-reference.json` for release/state exports.
- `evidence/runtime-audit/audit_fixture_100b3f20.c`,
  `fx_fixture_100b3f60.c`, `fx_fixture_100b3fc0.c`,
  `fx_fixture_100b3fe0.c` and `fx_fixture_100b4180.c` for control and timing.

## Setter behavior

The sprite setter allocates the seven-word FX owner at `sprite + 28` before
checking the requested action. This includes action 0, action 2 and unknown
actions. An allocation failure returns 0 with Win32 last error 6.

- Action 0 sets state 0 and frees the source/work payloads. The owner remains
  attached to the sprite. Effect ID, source pointer and duration are ignored.
- Action 1 returns 0 if state is nonzero. Otherwise it allocates the type-specific
  work/source payloads, copies the source, records the duration bits and current
  clock, and sets state 1. Only type 0 is supported by the portable adapter; no
  layout is invented for other types.
- Action 2 records state 2, duration and current clock, and selects the exit ramp.
  It returns 1 even if no start supplied a source. Effect ID/source are ignored.
- Every other action returns 1 without changing the already allocated owner.

The duration's native parameter is a signed 32-bit word, but elapsed-time
comparison treats its bits as unsigned. A duration of -1 therefore does not
become zero or a short timeout. Stop restarts at full source amplitude even when
issued during the entry ramp.

`SpriteSystem::set_sprite_fx_effect_with_source` now preserves these behaviors.
`SpriteFxEffect::has_source` is portable ownership metadata, not an added native
structure field. A source-free stop is accepted, but the portable renderer does
not dereference absent source/work allocations. The native updater has no such
guard; rendering that invalid sequence is not a supported native contract.

## Global ownership

Global start stores the active render context and inserts a callback before it
checks the effect state. A second start fails while this context latch is set,
even after action 0 or a completed stop changes the effect state to 0.
`PalReleaseFxEffect` is the operation that removes the render entry and zeroes the
context latch and all seven state words. Reference release assumes a valid
attached context; repeated global release is not claimed as a safe native call.

The portable adapter keeps attachment and effect activity separate:

- `start_global_raster` owns the attachment until release, including a start that
  attaches a node but then fails because state was already nonzero.
- `clear_global_raster` clears payload/state but retains that attachment.
- `stop_global_raster` sets the global state even before an attachment exists.
- `release_global_raster` removes the attachment and clears portable state.
- `global_raster_state` returns the state independently of visibility/payload.

An unattached global stop has no render callback to advance it. Frame ticks do
not silently complete it before a later start. Explicit release is required to
recover from the resulting attached-but-unsourced state safely.

## Pointer and query contracts

Generic sprite paths borrow a pointer to one 32-bit native sprite-address word;
they do not accept an arena ID in that word. Generic global start borrows a
pointer to its signed priority word. Global release and state query ignore the
target pointer, so that parameter may be null on those paths; the render-context
precondition for release still applies. Source is borrowed only for action 1 and
copied into owned native memory; other actions never read it.

The generic state query returns 0 for a sprite with no FX owner. In contrast,
`PalSpriteGetFxEffectState` directly dereferences the sprite's owner, requiring
both a valid sprite and a previously allocated FX owner. The portable getter
returns 0 for invalid/absent handles as a safe adapter behavior and must not be
used to claim that the native export accepts null.

Sprite release returns 0 if there is no owner; otherwise it frees both payloads
and the owner, clears `sprite + 28`, and returns 1. Successful native owner
release also clears the Win32 last-error value. The Rust adapter currently
returns a boolean without a Win32 last-error store.

## Validation and remaining limits

`cargo test -p pal-vm --no-default-features --test sprite_fixture` passes 23 tests,
including regressions for unknown actions, nonzero ignored type IDs, retained
owners, source-free stop, unsigned duration bits and global restart requiring
release. These are control-flow and lifetime checks against recovered behavior.
They do not certify native raster pixels, renderer scheduling or all FX types.

Per-sprite updater timing is still driven by the portable frame tick instead of
every original render callback. Dynamic FX ownership/source/timing is not yet
part of portable save images. Type tables beyond raster, original error-state
propagation, invalid pointer behavior and cross-platform rendering still need
separate verification.
