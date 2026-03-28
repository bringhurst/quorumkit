---
title: Check prerequisites
sidebar_position: 1
---

# Check prerequisites

Before you build QuorumKit, make sure the local machine has the basics in place.

## Required tools

- a C++ toolchain supported by the project
- CMake
- `make` or another generator supported by your local CMake setup
- Node.js and npm for the docs site

## Repository areas you will likely touch

- build files: `CMakeLists.txt`, `cmake/`
- docs site: `docs/`, `docusaurus.config.js`, `package.json`
- examples: `example/`

## Quick checks

```sh
cmake --version
npm --version
node --version
```

If those commands work, you are in good shape for the common local build and docs tasks.
