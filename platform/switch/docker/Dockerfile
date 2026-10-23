FROM devkitpro/devkita64:latest

ARG RUST_TOOLCHAIN=nightly-2026-01-20
ENV RUSTUP_HOME=/usr/local/rustup \
    CARGO_HOME=/usr/local/cargo \
    DEVKITPRO=/opt/devkitpro \
    DEVKITA64=/opt/devkitpro/devkitA64
ENV PATH=$CARGO_HOME/bin:$DEVKITA64/bin:$DEVKITPRO/tools/bin:$PATH

RUN dkp-pacman -Syu --noconfirm && dkp-pacman -S --needed --noconfirm switch-deko3d uam
RUN apt-get update \
    && apt-get install -y --no-install-recommends build-essential ca-certificates curl python3 patch \
    && rm -rf /var/lib/apt/lists/*
RUN curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs \
    | sh -s -- -y --no-modify-path --profile minimal \
        --default-toolchain "$RUST_TOOLCHAIN" --component rust-src
ENV SENA_SWITCH_TOOLCHAIN=$RUST_TOOLCHAIN
WORKDIR /src
CMD ["platform/switch/build_switch.sh"]
