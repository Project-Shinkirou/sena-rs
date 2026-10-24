# Reuse siglus_rs switch-build:f7740a0a96c7d24a with its prepared SDK/nightly.
FROM ghcr.io/xmoezzz/siglus_rs/switch-build@sha256:bb3f47af6951a4770171fe1c8e5a508ddb233d09a17bd6c8650c2e928ab504f3

RUN apt-get update \
    && apt-get install -y --no-install-recommends python3 patch \
    && rm -rf /var/lib/apt/lists/*
ENV SENA_SWITCH_TOOLCHAIN=nightly-2026-01-20
WORKDIR /src
CMD ["bash", "platform/switch/build_switch.sh"]
