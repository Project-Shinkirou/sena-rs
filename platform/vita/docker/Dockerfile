# Reuse siglus_rs vita-build:6f6be2c078140b09 with its prepared SDK/nightly.
FROM ghcr.io/xmoezzz/siglus_rs/vita-build@sha256:f9e3eaa24d74fa48dd514fbc86b19a13d7fdf88db96e920a303b050ec8402a85

RUN apt-get update \
    && apt-get install -y --no-install-recommends cmake \
    && rm -rf /var/lib/apt/lists/*
ENV SENA_VITA_TOOLCHAIN=nightly-2026-09-18
WORKDIR /src
CMD ["bash", "platform/vita/build_vita.sh"]
