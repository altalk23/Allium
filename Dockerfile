# syntax=docker/dockerfile:1.6

ARG MOD_ID
ARG CPM_CACHE_DIR=/workspace/cpm-cache
ARG BINDINGS=geode-sdk/bindings
ARG SDK_VERSION=stable

# Android 32
FROM prevter/geode-sdk:android-latest AS android32
ARG MOD_ID
ARG CPM_CACHE_DIR
ARG BINDINGS
ARG SDK_VERSION

ENV LANG=C.UTF-8
ENV LC_ALL=C.UTF-8

RUN geode sdk update ${SDK_VERSION} \
    && geode sdk install-binaries -p android32 \
    && git clone https://github.com/${BINDINGS} --depth=1 /workspace/bindings

WORKDIR /workspace/project
COPY . .

RUN --mount=type=cache,target=/workspace/cpm-cache \
    --mount=type=cache,target=/workspace/project/build,id=android32 \
    cmake -G Ninja -B build \
      -DCMAKE_BUILD_TYPE=RelWithDebInfo \
      -DCPM_SOURCE_CACHE=${CPM_CACHE_DIR} \
      -DGEODE_BINDINGS_REPO_PATH=/workspace/bindings \
      -DCMAKE_TOOLCHAIN_FILE=/opt/android-ndk/build/cmake/android.toolchain.cmake \
      -DANDROID_ABI=armeabi-v7a \
      -DANDROID_PLATFORM=android-23 \
      -DANDROID_STL=c++_shared \
    && cmake --build build --parallel \
    && cp build/${MOD_ID}.geode /workspace/

FROM scratch AS export-android32
ARG MOD_ID
COPY --from=android32 /workspace/${MOD_ID}.geode /${MOD_ID}.geode

# Android 64
FROM prevter/geode-sdk:android-latest AS android64
ARG MOD_ID
ARG CPM_CACHE_DIR
ARG BINDINGS
ARG SDK_VERSION

ENV LANG=C.UTF-8
ENV LC_ALL=C.UTF-8

RUN geode sdk update ${SDK_VERSION} \
    && geode sdk install-binaries -p android64 \
    && git clone https://github.com/${BINDINGS} --depth=1 /workspace/bindings

WORKDIR /workspace/project
COPY . .

RUN --mount=type=cache,target=/workspace/cpm-cache \
    --mount=type=cache,target=/workspace/project/build,id=android64 \
    cmake -G Ninja -B build \
      -DCMAKE_BUILD_TYPE=RelWithDebInfo \
      -DCPM_SOURCE_CACHE=${CPM_CACHE_DIR} \
      -DGEODE_BINDINGS_REPO_PATH=/workspace/bindings \
      -DCMAKE_TOOLCHAIN_FILE=/opt/android-ndk/build/cmake/android.toolchain.cmake \
      -DANDROID_ABI=arm64-v8a \
      -DANDROID_PLATFORM=android-23 \
      -DANDROID_STL=c++_shared \
    && cmake --build build --parallel \
    && cp build/${MOD_ID}.geode /workspace/

FROM scratch AS export-android64
ARG MOD_ID
COPY --from=android64 /workspace/${MOD_ID}.geode /${MOD_ID}.geode

# iOS
FROM prevter/geode-sdk:ios-latest AS ios
ARG MOD_ID
ARG CPM_CACHE_DIR
ARG BINDINGS
ARG SDK_VERSION

ENV LANG=C.UTF-8
ENV LC_ALL=C.UTF-8

RUN geode sdk update ${SDK_VERSION} \
    && geode sdk install-binaries -p ios \
    && git clone https://github.com/${BINDINGS} --depth=1 /workspace/bindings

WORKDIR /workspace/project
COPY . .

RUN --mount=type=cache,target=/workspace/cpm-cache \
    --mount=type=cache,target=/workspace/project/build,id=ios \
    cmake -G Ninja -B build \
      -DCMAKE_BUILD_TYPE=RelWithDebInfo \
      -DCPM_SOURCE_CACHE=${CPM_CACHE_DIR} \
      -DGEODE_BINDINGS_REPO_PATH=/workspace/bindings \
      -DGEODE_IOS_SDK="${CMAKE_OSX_SYSROOT}" \
    && cmake --build build --parallel \
    && cp build/${MOD_ID}.geode /workspace/

FROM scratch AS export-ios
ARG MOD_ID
COPY --from=ios /workspace/${MOD_ID}.geode /${MOD_ID}.geode

# MacOS
FROM prevter/geode-sdk:macos-latest AS macos
ARG MOD_ID
ARG CPM_CACHE_DIR
ARG BINDINGS
ARG SDK_VERSION

ENV LANG=C.UTF-8
ENV LC_ALL=C.UTF-8

RUN geode sdk update ${SDK_VERSION} \
    && geode sdk install-binaries -p macos \
    && git clone https://github.com/${BINDINGS} --depth=1 /workspace/bindings

WORKDIR /workspace/project
COPY . .

RUN --mount=type=cache,target=/workspace/cpm-cache \
    --mount=type=cache,target=/workspace/project/build,id=macos \
    cmake -G Ninja -B build \
      -DCMAKE_BUILD_TYPE=RelWithDebInfo \
      -DCMAKE_OSX_DEPLOYMENT_TARGET=11.0 \
      -DCPM_SOURCE_CACHE=${CPM_CACHE_DIR} \
      -DGEODE_BINDINGS_REPO_PATH=/workspace/bindings \
    && cmake --build build --parallel \
    && cp build/${MOD_ID}.geode /workspace/

FROM scratch AS export-macos
ARG MOD_ID
COPY --from=macos /workspace/${MOD_ID}.geode /${MOD_ID}.geode

# Windows
FROM prevter/geode-sdk:windows-latest AS windows
ARG MOD_ID
ARG CPM_CACHE_DIR
ARG BINDINGS
ARG SDK_VERSION

ENV LANG=C.UTF-8
ENV LC_ALL=C.UTF-8

RUN geode sdk update ${SDK_VERSION} \
    && geode sdk install-binaries -p windows \
    && git clone https://github.com/${BINDINGS} --depth=1 /workspace/bindings

WORKDIR /workspace/project
COPY . .

RUN --mount=type=cache,target=/workspace/cpm-cache \
    --mount=type=cache,target=/workspace/project/build,id=windows \
    cmake -G Ninja -B build \
      -DCMAKE_BUILD_TYPE=RelWithDebInfo \
      -DCPM_SOURCE_CACHE=${CPM_CACHE_DIR} \
      -DGEODE_BINDINGS_REPO_PATH=/workspace/bindings \
      -DCMAKE_TOOLCHAIN_FILE=/root/.local/share/Geode/cross-tools/clang-msvc-sdk/clang-msvc.cmake \
      -DSPLAT_DIR=/root/.local/share/Geode/cross-tools/splat \
      -DHOST_ARCH=x64 \
      && cmake --build build --parallel \
      && cp build/${MOD_ID}.geode /workspace/

FROM scratch AS export-windows
ARG MOD_ID
COPY --from=windows /workspace/${MOD_ID}.geode /${MOD_ID}.geode