---
title: Prerequisites
sidebar_position: 1
---

# Prerequisites

## C++ toolchain

You need a C++ compiler that supports C++11 or later. GCC 4.8+ and Clang 3.5+ both work. On macOS, the Xcode command-line tools provide Clang:

```sh
xcode-select --install
```

On Debian/Ubuntu:

```sh
sudo apt install build-essential
```

## CMake

CMake 3.16 or later. Check with:

```sh
cmake --version
```

## Node.js (for the docs site only)

The docs site is built with Docusaurus and requires Node.js 18+ and npm:

```sh
node --version
npm --version
```

If you are only building the C++ code and examples, you do not need Node.

## Verify

A quick sanity check that the build tools are in place:

```sh
cmake --version && c++ --version
```

If both commands print version info, you are ready to build. Next step: [Build and test](./build-and-test).
