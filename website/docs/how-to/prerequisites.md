---
title: Prerequisites
sidebar_position: 1
---

# Prerequisites

## C++ toolchain

You need a C++ compiler that supports C++17. Clang 10+ and GCC 10+ both work. On macOS, the Xcode command-line tools provide Clang:

```sh
xcode-select --install
```

On Debian/Ubuntu:

```sh
sudo apt install build-essential
```

## CMake

CMake 3.15 or later. Check with:

```sh
cmake --version
```

## Conan 2

[Conan 2](https://conan.io/) manages all C++ dependencies (protobuf, gflags, leveldb, brpc, etc.). Install it with Homebrew on macOS:

```sh
brew install conan
```

Or with pip on Linux:

```sh
pip install conan
```

If you have never used Conan on this machine, detect a default profile:

```sh
conan profile detect
```

Check the version:

```sh
conan --version   # must be 2.x
```

## Node.js (for the docs site only)

The docs site is built with Docusaurus and requires Node.js 18+ and npm:

```sh
node --version
npm --version
```

If you are only building the C++ code, you do not need Node.

## Verify

A quick sanity check that the build tools are in place:

```sh
cmake --version && c++ --version && conan --version
```

If all three commands print version info, you are ready to build. Next step: [Build and test](./build-and-test).
