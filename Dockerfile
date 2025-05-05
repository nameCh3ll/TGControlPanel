# syntax = docker/dockerfile:1.3
FROM archlinux:latest AS builder

RUN pacman -Syu --noconfirm && pacman -S --noconfirm \
    base-devel \
    man-pages \
    pkg-config \
    cmake \
    glibc \
    python-pip \
    git \
    gcc \
    wget \
    pkg-config \
    libx11 \
    openssl \
    zlib \
    mesa \
    libvlc

RUN python -m venv /opt/venv
ENV PATH="/opt/venv/bin:$PATH"

RUN pip install conan

WORKDIR /app

RUN mkdir build
RUN conan profile detect --force
COPY conanProfiles/ conanProfiles/
COPY conanfile.py CMakeLists.txt .

RUN --mount=type=cache,target=/app/build \
    conan install . \
    --build=missing \
    --profile:host=conanProfiles/tgbot \
    --profile:build=conanProfiles/tgbot 

COPY include/ include/
COPY src/ src/
COPY tst/ tst/

RUN --mount=type=cache,target=/app/build \
    cmake --preset conan-release

RUN --mount=type=cache,target=/app/build \
    cmake --build --preset conan-release --parallel && \
    cp /app/build/Release/TGControlPanel /app/TGControlPanel

FROM scratch
COPY --from=builder /app/TGControlPanel ./build/Release/
