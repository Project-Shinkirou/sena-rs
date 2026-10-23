# Desktop PAL window requests

The winit host now drains `Engine::take_window_requests` after VM updates, including diagnostic update paths. Requests change borderless fullscreen, client size, or outer position. Move/resize observations update PAL geometry without enqueueing new requests. Startup dimensions are observations as well, so opening a host with explicit dimensions no longer leaves an INI resize request waiting to overwrite them.

## Native evidence

The following functions are in `reverse/PAL.sqlite` (PAL_d.dll):

- `PalWindowChangeMode_0` posts thread message `0x8065` and returns success. The worker `sub_10234EC0` checks `Block[308]`, the `PalWindowChangeEnable` latch, when consuming the request. Disabled changes are discarded; the flag does not disable window positioning or resizing.
- `sub_1026D100` skips an unchanged mode and treats zero as windowed and any nonzero value as fullscreen. It also has an initialization flag gate (`Block[264] & 4`) not represented by the portable host.
- `PalChangeWindowSize_0` accepts but ignores requests while fullscreen. `sub_1026CE50` computes the new height from the requested width times the game's logical height/width ratio. The caller's height is preserved only when the requested pair already matches the current dimensions.
- `PalWindowSetPos_0` stores the position and calls `SetWindowPos` with `SWP_NOSIZE`.
- `PalGetWindowSize_0` and `PalWindowGetPos_0` use the fullscreen content rectangle in fullscreen mode and the remembered windowed fields otherwise.
- Game `sub_438950` separately stores the requested mode in task data +8 after posting. The category 9 index 9 getter returns that script cache. It must not become the host's accepted mode: a disabled request still changes the script cache.

The portable system now separates the requested script mode from the accepted PAL mode. Mode acceptance happens at request consumption, applying the current enable latch. Windowed geometry survives fullscreen observations. Explicit script dimensions remain asynchronous; host constraints replace them when a resize observation arrives.

## Host policy and remaining limits

The desktop host uses winit logical desktop units for script size/position and physical pixel sizes for rendering and pointer mapping. This is consistent with its existing startup sizing on HiDPI displays. Fullscreen uses the current monitor's borderless mode, without changing monitor resolution. Zero-sized minimize observations are ignored.

`ChangeAspect` is consumed with a warning; the four native renderer scaling policies are not mapped yet. PAL's D3D/backend aspect tables and sprite aspect-position conversion need separate renderer/input work. External FFI hosts do not yet consume the request queue, so mobile/custom hosts retain this limitation. `Engine::take_window_requests` and `Engine::observe_window_geometry` provide the bridge for those hosts. Platform restrictions can still prevent positioning or fullscreen changes; this pass does not add OS completion/error callbacks.

Validation: focused system, runtime, and engine fixtures pass (11 tests). A real Sv20 instruction sequence posts a size change, posts fullscreen, disables mode changes, and queries the script cache; only the aspect-correct size request reaches the host queue. Additional tests cover no resize feedback, host-constrained sizes, consumption-time enable gating, fullscreen size suppression, and preservation of windowed geometry. Interactive OS window behavior was compiled but not exercised by a GUI automation test.
