---
title: Build and test QuorumKit
sidebar_position: 1
---

# Build and test QuorumKit

Use this page when you just want the commands.

## Build the project

```sh
cmake -S . -B bld
cmake --build bld
```

## Build with unit tests

```sh
cmake -S . -B bld -DBUILD_UNIT_TESTS=ON
cmake --build bld
```

The test binaries are produced in the build tree.

## Build the docs

The docs site uses Docusaurus.

```sh
npm ci
npm run build
```

For local interactive preview:

```sh
npm run start
```

The published docs live under `/docs`, and the site root redirects there.

## Common places to look

- public docs source: `docs/`
- Docusaurus config: `docusaurus.config.js`
- docs workflow: `.github/workflows/docs-pages.yml`
- examples: `example/`
- tests: `test/`
