---
title: Build and test
sidebar_position: 1
---

# Build and test

## Build the C++ project

```sh
cmake -S . -B bld
cmake --build bld
```

Build artifacts go into `bld/`. Libraries land in `bld/lib` or `bld/src` depending on your CMake generator.

## Build with tests

```sh
cmake -S . -B bld -DBUILD_UNIT_TESTS=ON
cmake --build bld
```

Test binaries are produced inside the build tree. Run them directly or through `ctest`:

```sh
ctest --test-dir bld
```

## Build the docs site

```sh
npm ci
npm run build
```

The static output goes into `build/`. For a local preview with live reload:

```sh
npm run start
```

The docs are served at `http://localhost:3000/docs/`.

## Key paths

| What | Where |
|---|---|
| CMake root | `CMakeLists.txt` |
| CMake modules | `cmake/` |
| Docusaurus config | `docusaurus.config.js` |
| Docs source | `docs/` |
| Examples | `example/` |
| Tests | `test/` |
| GitHub Pages workflow | `.github/workflows/docs-pages.yml` |
