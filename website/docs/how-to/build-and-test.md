---
title: Build and test
sidebar_position: 1
---

# Build and test

QuorumKit uses [Conan 2](https://conan.io/) for dependency management and CMake for building. Make sure you have all [prerequisites](./prerequisites) installed before starting.

## One-time setup

brpc is not on Conan Center, so export the local recipe first:

```sh
conan export contrib/brpc/
```

You only need to re-run this if the brpc recipe in `contrib/brpc/conanfile.py` changes.

## Install dependencies

```sh
conan install . --output-folder=build --build=missing
```

This downloads and builds all dependencies (protobuf, gflags, leveldb, openssl, zlib, brpc, gtest) and generates CMake presets under `build/`.

The first run takes a while because it builds brpc and protobuf from source. Subsequent runs use the Conan cache and finish in seconds.

## Build the library

```sh
cmake --preset conan-release
cmake --build --preset conan-release
```

The library (`libbraft.a`) and test binaries land under `build/`.

## Build with tests

Tests are on by default in the Conan recipe. To pass the option explicitly to CMake:

```sh
cmake --preset conan-release -DBUILD_UNIT_TESTS=ON
cmake --build --preset conan-release
```

## Run tests

```sh
ctest --preset conan-release --output-on-failure -LE known_crash
```

The `-LE known_crash` flag excludes three tests that crash due to upstream braft bugs in configuration-change code paths. See `UPSTREAM_BUGS.md` for details. To run only the crashing tests (for investigation):

```sh
ctest --preset conan-release -L known_crash
```

## Build the docs site

```sh
cd website
npm ci
npm run build
```

The static output goes into `website/site-build/`. For a local preview with live reload:

```sh
cd website
npm start
```

The docs are served at `http://localhost:3000/docs/`.

## Key paths

| What | Where |
|---|---|
| Conan recipe (QuorumKit) | `conanfile.py` |
| Conan recipe (brpc) | `contrib/brpc/conanfile.py` |
| CMake root | `CMakeLists.txt` |
| Library source + protos | `src/braft/` |
| Tests | `test/` |
| Docusaurus config | `website/docusaurus.config.js` |
| Docs source | `website/docs/` |
| CI workflow (C++) | `.github/workflows/ci.yml` |
| CI workflow (docs) | `.github/workflows/docs-pages.yml` |
