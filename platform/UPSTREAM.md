# Console port provenance

Reference project: [xmoezzz/siglus_rs](https://github.com/xmoezzz/siglus_rs/tree/5f358ef1360b0439e92aed2b749e8f25d8721954),
commit `5f358ef1360b0439e92aed2b749e8f25d8721954`, checked against origin/main
on 2026-10-23. Upstream is MPL-2.0, as is this repository.

- `switch/rust/aarch64-switch.json` and
  `switch/patches/libc-horizon-layout.patch` are copied from upstream's Switch
  runtime build support. The libc patch is applied to 0.2.178, matching the
  selected nightly's build-std lockfile. The earlier 0.2.186 patch was unused
  by std, which caused incorrect file metadata and prevented actual boot.
- CI consumes upstream's published Switch and Vita GHCR images directly;
  the optional local Docker wrappers use the same pinned base images and add
  only small packaging helpers. Sena retains its own CMake/VPK packaging.
  See the exact image identities below; Vita's prepared nightly differs from
  the earlier locally tested January toolchain.
- The native audio pull boundary, Vita extended-memory configuration, and
  Vita3K SLP workaround follow upstream's console ports. The Kira 0.12 backend
  here is adapted to sena_rs and uses per-engine sink ownership.
- `switch/source/gpu.c`, `gpu.h`, and `switch/shaderc` are adapted from
  upstream's deko3d renderer and Naga shader compiler. Symbol names and the
  shader selection/binding map are adapted for sena_rs. `scene_gpu.c` is the
  new PAL scene consumer; the ordinary/raster programs are sena_rs WGSL.
- The C frontends, shared controls, and scene ABI are implemented for sena_rs.
  The siglus_rs VM and custom Vita GXM renderer are not imported. The Vita
  ordinary scene consumer uses libvita2d's embedded programs. Small-allocation
  policy, explicit non-render-target initialization, and four-frame texture
  retirement follow the upstream Vita implementation.

The Vita Ogg streaming adapter, exact seek/preroll behavior, direct shared PCM
allocation and GE/PGD3 row streaming are implemented in sena_rs; they are not
copied from siglus_rs. Their evidence is retained in
[ADR-0009](../reverse/adr/0009-retain-decoded-pixel-allocations.md) and
[ADR-0010](../reverse/adr/0010-stream-vita-ogg-bgm.md).

Decoder provenance is recorded separately in
[`na_mpeg2_decoder/UPSTREAM.md`](../crates/na_mpeg2_decoder/UPSTREAM.md) and
[`na_wmv_player/UPSTREAM.md`](../crates/na_wmv_player/UPSTREAM.md).

Kira 0.12.1 is retained locally under `vendor/kira` with one stream-destruction
fix. Its upstream commit, registry checksum, MIT/Apache licenses and exact source
change are described in [its provenance record](../vendor/kira/UPSTREAM.md).

## Prepared console build images

Checked anonymously against GHCR on 2026-10-24. Both manifests are Linux/amd64
and publicly pullable. Source Dockerfiles and publishing workflows belong to
siglus_rs commit `5f358ef1360b0439e92aed2b749e8f25d8721954`. Upstream tags
are the first 16 characters of their Dockerfile SHA-256; Sena pins the resulting
manifest digest so a changed tag does not silently change its build environment.

| Platform | Upstream image/tag | Rust toolchain |
| --- | --- | --- |
| Switch | `ghcr.io/xmoezzz/siglus_rs/switch-build:f7740a0a96c7d24a` | `nightly-2026-01-20` |
| Vita | `ghcr.io/xmoezzz/siglus_rs/vita-build:6f6be2c078140b09` | `nightly-2026-09-18` |

Switch manifest: `sha256:bb3f47af6951a4770171fe1c8e5a508ddb233d09a17bd6c8650c2e928ab504f3`.
Vita manifest: `sha256:f9e3eaa24d74fa48dd514fbc86b19a13d7fdf88db96e920a303b050ec8402a85`.

The manifests retain upstream's installed SDK, target libraries and Rust sources.
Sena's CI installs only CMake, Python and patch helpers; it does not rebuild the
SDK, invoke a package-manager SDK upgrade, or require a container registry login.
