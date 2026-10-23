FROM ubuntu:22.04

ARG RUST_TOOLCHAIN=nightly-2026-01-20
ENV DEBIAN_FRONTEND=noninteractive \
    VITASDK=/usr/local/vitasdk \
    RUSTUP_HOME=/usr/local/rustup \
    CARGO_HOME=/usr/local/cargo
ENV PATH=$VITASDK/bin:$CARGO_HOME/bin:$PATH

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        build-essential ca-certificates cmake curl git python3 xz-utils zstd unzip \
    && rm -rf /var/lib/apt/lists/*
RUN git clone --depth 1 https://github.com/vitasdk/vdpm /tmp/vdpm \
    && /tmp/vdpm/bootstrap-vitasdk.sh \
    && VDPM_NONINTERACTIVE=1 "$VITASDK/bin/vdpm" install libvita2d \
    && test -f "$VITASDK/arm-vita-eabi/lib/libvita2d.a" \
    && rm -rf /tmp/vdpm
RUN curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs \
    | sh -s -- -y --no-modify-path --profile minimal \
        --default-toolchain "$RUST_TOOLCHAIN" --component rust-src
ENV SENA_VITA_TOOLCHAIN=$RUST_TOOLCHAIN
WORKDIR /src
CMD ["platform/vita/build_vita.sh"]
