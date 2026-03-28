# AGENTS.md

This file provides context for AI coding agents working on QuorumKit.

## What is QuorumKit

QuorumKit is a hard fork of braft (a C++ Raft consensus library). It provides:

- A canonical **QuorumKit** public API under `include/quorumkit/`.
- A bounded **braft** compatibility layer under `include/braft/` that calls into QuorumKit.
- A shared internal implementation under `src/internal/` (future -- currently still `src/braft/`).

The long-term goal is a single-threaded, provable Raft core with deterministic (FoundationDB-style) simulation testing.

## Repository layout

```
quorumkit/
  docs/                     # Docusaurus docs site (Diataxis structure)
    index.md                # Landing page
    tutorials/              # Step-by-step learning
    how-to/                 # Task-oriented guides
    reference/              # API reference, glossary, repo map
    explanation/            # Architecture and design rationale
  src/
    braft/                  # Current library source (being restructured)
    css/                    # Docusaurus custom styles
    pages/                  # Docusaurus pages (root redirect)
  example/                  # Example applications (counter, atomic, block)
  test/                     # Tests
  cmake/                    # CMake modules
  bazel/                    # Bazel build files (isolated)
  tools/                    # Build/CI tooling
  static/                   # Docusaurus static assets (includes CNAME)
  .github/workflows/        # GitHub Actions (docs-pages.yml)
  docusaurus.config.js      # Docusaurus config
  sidebars.js               # Sidebar auto-generated from directory structure
  package.json              # Node deps for Docusaurus
  CMakeLists.txt            # Top-level CMake
```

## Key naming conventions

- The project is called **QuorumKit** everywhere except the compatibility layer.
- **braft** is used only when referring to the compatibility API surface or the original upstream.
- Do not rename things to `braft` in new code. New public API goes under `quorumkit`.

## Docs

### Structure

Docs use Docusaurus 3.9 and follow the Diataxis framework:

| Category     | Path               | Purpose                              |
|--------------|--------------------|--------------------------------------|
| Tutorials    | `docs/tutorials/`  | Learning-oriented walkthroughs       |
| How-to       | `docs/how-to/`     | Task-oriented guides                 |
| Reference    | `docs/reference/`  | API overview, glossary, repo map     |
| Explanation  | `docs/explanation/` | Architecture and design discussion  |

Sidebar is auto-generated from directory structure (`sidebars.js`). Each category directory has a `_category_.json` for ordering and labels.

### Writing style

- English only.
- Practical, grounded, plainspoken prose -- like a well-written No Starch Press book.
- Short and focused pages (roughly 2-minute read as a soft target).
- Do not sound generated or generic. No filler. No superlatives.
- Do not imitate a living author.
- No custom slugs on doc pages -- keep links simple and use the filesystem path.

### Links

- Use **relative links** between docs pages (e.g., `./explanation/architecture-overview`, `../how-to/prerequisites`).
- Do not use absolute paths like `/docs/explanation/...` in doc markdown.
- The footer in `docusaurus.config.js` uses absolute paths -- that is the one place where absolute `/docs/...` paths are correct.

### Building and verifying

```sh
npm ci                  # Install deps (first time)
npm run build           # Build site to site-build/ -- this checks all links
npm start               # Dev server with hot reload
npm run serve           # Serve the built site locally
```

The build is configured to **throw on broken links** (`onBrokenLinks: 'throw'`). Always run `npm run build` after editing docs to catch link errors.

### Deployment

Docs are published to GitHub Pages via `.github/workflows/docs-pages.yml`. The custom domain is `quorumkit.org`. The root `/` redirects to `/docs/` via `src/pages/index.js`. The `static/CNAME` file ensures the custom domain persists across deploys.

## C++ library

### Building

```sh
cmake -S . -B bld
cmake --build bld
```

With tests:

```sh
cmake -S . -B bld -DBUILD_UNIT_TESTS=ON
cmake --build bld
```

### Build system philosophy

- CMake is the primary build system.
- Bazel support exists but is isolated under `bazel/`.
- Build and packaging should stay modular -- future Conan/vcpkg support is planned.

## Design principles

1. **Transport is modular.** The current transport uses brpc, but the interface supports replacement (raw TCP, RDMA, OpenMPI-style, IPv6, simulation loopback).
2. **Storage is modular.** Existing braft storage backends (local segments, RocksDB) must keep working. The interface supports adding new backends, and migration between backends is part of the design.
3. **Public API boundaries are explicit.** The directory structure and headers make it clear what is public (`include/quorumkit/`, `include/braft/`) and what is internal (`src/internal/`).
4. **The braft compatibility layer is bounded.** It wraps the QuorumKit API -- it never adds functionality that QuorumKit does not have.

## Branches

- `master` -- do not push directly.
- `bringhurst/initial-docs` -- docs rewrite (current working branch).
- `bringhurst/disable-workflows` -- disabled stale CI workflows.
- `bringhurst/remove-jepsen` -- removed Jepsen test infrastructure.
- `bringhurst/remove-benchmark` -- removed old benchmark code.

## License

Apache License 2.0. The `NOTICE` file contains fork attribution to braft.
