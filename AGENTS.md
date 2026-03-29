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
  contrib/                  # Local Conan recipes for deps not on Conan Center
    brpc/                   # brpc 1.16.0 recipe (see "Dependencies" below)
  website/                  # Docusaurus app (config, theme, content)
    docs/                   # Doc markdown content (Diataxis structure)
      index.md              # Landing page
      tutorials/            # Step-by-step learning
      how-to/               # Task-oriented guides
      reference/            # API reference, glossary, repo map
      explanation/          # Architecture and design rationale
    src/                    # Docusaurus app source (root redirect, CSS)
    static/                 # Docusaurus static assets (includes CNAME)
    docusaurus.config.js    # Docusaurus config
    sidebars.js             # Sidebar auto-generated from directory structure
    package.json            # Node deps for Docusaurus
  src/
    braft/                  # Library source (C++ and proto files)
    CMakeLists.txt          # Proto generation + braft library target
  examples/                 # Example applications (counter, atomic, block)
  tests/                    # Tests (test_*.cpp)
  .github/workflows/        # GitHub Actions (ci.yml, docs-pages.yml)
  conanfile.py              # Top-level Conan recipe for QuorumKit
  CMakeLists.txt            # Top-level CMake project setup
  UPSTREAM_BUGS.md          # Documented upstream braft bugs
```

## Key naming conventions

- The project is called **QuorumKit** everywhere except the compatibility layer.
- **braft** is used only when referring to the compatibility API surface or the original upstream.
- Do not rename things to `braft` in new code. New public API goes under `quorumkit`.

## C++ library

### Building

Conan 2 is the package manager. CMake is the build system. The developer workflow:

```sh
conan export contrib/brpc/                              # Export local brpc recipe
conan install . --output-folder=build --build=missing   # Install all deps
cmake --preset conan-release -DBUILD_UNIT_TESTS=ON      # Configure
cmake --build --preset conan-release                    # Build
ctest --preset conan-release --output-on-failure -LE known_crash  # Test
```

Without tests, omit `-DBUILD_UNIT_TESTS=ON` from the configure step.

### Dependencies

All dependencies are managed through Conan. Most come from Conan Center:

| Package | Version | Notes |
|---------|---------|-------|
| protobuf | 3.21.12 | Last 3.x release. **Must stay on 3.x** -- `google::protobuf::Closure` was removed in 3.22+. |
| gflags | 2.2.2 | Conan ships as `libgflags_nothreads.a`, not `libgflags.a`. |
| leveldb | 1.23 | |
| openssl | 3.4.1 | |
| zlib | 1.3.1 | |
| gtest | 1.14+ | Requires C++14 minimum (the project builds with C++17). |

**brpc 1.16.0** is not on Conan Center. A local recipe lives at `contrib/brpc/conanfile.py`. It downloads brpc source from GitHub, patches out the unused `protoc-gen-mcpack` tool (which has link-order issues on Linux), keeps Debug sanitizer builds from being forced back to `-O2 -DNDEBUG`, and builds `libbrpc.a` with all dependency paths pointed at Conan packages. The recipe exposes a `with_asan` option so CI can turn on brpc's ASan fiber annotations. The recipe contains extensive documentation in its docstring and comments explaining each workaround.

Any dependency not in Conan Center should go in `contrib/` as a local Conan recipe (not vendored source).

### Build scope

The default build covers the core library and unit tests. Example applications in `examples/` are built only when `-DBUILD_EXAMPLES=ON`; CI enables that on representative Linux and macOS jobs.

### Proto files

There are 8 proto files in `src/braft/`. They have no standard protobuf imports (no `google/protobuf/*`). They only import each other via `braft/` prefix (e.g., `import "braft/enum.proto"`). Proto generation is handled in `src/CMakeLists.txt` (must be in the same directory scope as `add_library`).

### Tests

- **20 passing tests** in the default GCC/macOS runs, 3 with known upstream crashes (labeled `known_crash`, excluded from CI with `-LE known_crash`).
- Tests use `-Dprivate=public -Dprotected=public` to access internals -- ugly but necessary for now.
- Each test binary gets its own working directory under `testwd/<test_name>/`.
- Tests sharing the same ports use `RESOURCE_LOCK` properties to avoid conflicts.
- All tests have a 120-second timeout in normal builds and 300 seconds in sanitizer builds.

The 3 known-crash tests (`test_leader_lease`, `test_cli`, `test_node`) are documented in `UPSTREAM_BUGS.md`. These are real upstream braft memory-corruption bugs during configuration changes / leader failover.

Linux Clang CI also excludes 3 additional upstream crashers (`test_file_service`, `test_snapshot`, `test_snapshot_executor`) via the `known_clang_crash` label. The sanitizer job is narrower still: upstream brpc/butil UBSan violations currently make almost the entire suite abort, so CI excludes tests labeled `known_sanitizer_upstream_bug` there and keeps only `test_ballot` enabled as a smoke test.

### Compile definitions

The braft/brpc code requires these compile definitions (set in `src/CMakeLists.txt`):

`BRPC_WITH_GLOG=0`, `GFLAGS_NS=gflags`, `BTHREAD_USE_FAST_PTHREAD_MUTEX`, `__const__=__unused__`, `_GNU_SOURCE`, `USE_SYMBOLIZE`, `NO_TCMALLOC`, `__STDC_FORMAT_MACROS`, `__STDC_LIMIT_MACROS`, `__STDC_CONSTANT_MACROS`, `__STRICT_ANSI__`

### macOS-specific notes

- Linker undefined symbols for optional profiling libraries (gperftools, jemalloc) are handled via `-Wl,-U,<symbol>` in `src/CMakeLists.txt`.
- The brpc Conan recipe includes Homebrew contamination prevention -- see the detailed comments in `contrib/brpc/conanfile.py`.
- Never install Conan via pip on macOS if Homebrew Conan is present. This corrupts the Homebrew symlink. Use `brew install conan`.

## Design principles

1. **Transport is modular.** The current transport uses brpc, but the interface supports replacement (raw TCP, RDMA, OpenMPI-style, IPv6, simulation loopback).
2. **Storage is modular.** Existing braft storage backends (local segments, RocksDB) must keep working. The interface supports adding new backends, and migration between backends is part of the design.
3. **Public API boundaries are explicit.** The directory structure and headers make it clear what is public (`include/quorumkit/`, `include/braft/`) and what is internal (`src/internal/`).
4. **The braft compatibility layer is bounded.** It wraps the QuorumKit API -- it never adds functionality that QuorumKit does not have.

## CI

Two GitHub Actions workflows:

- **`ci.yml`** -- runs a 4-entry Linux matrix on `ubuntu-24.04` (`gcc-15`, `clang-21-libstdcxx`, `clang-21-libcxx`, `clang-21-sanitizers`) plus a `macos-15` arm64 build-and-test job. The sanitizer job uses Conan profile flags for `-fsanitize=address,undefined` and enables brpc's `with_asan` option. Linux Clang jobs exclude tests labeled `known_clang_crash`, and the sanitizer job also excludes tests labeled `known_sanitizer_upstream_bug` until the upstream brpc/butil UBSan issues are fixed. Triggered on pushes/PRs to `master` (skips docs-only changes). Uses `workflow_dispatch` for manual runs.
- **`docs-pages.yml`** -- deploys the Docusaurus site to GitHub Pages.

## Contributor workflow

- Do not push directly to `master`. Use feature branches and pull requests.
- Run `black` on all Python files (Conan recipes, scripts) before committing.
- Run `npm run build` in `website/` after editing docs to catch broken links (the build throws on broken links).
- Document workarounds in Conan recipes with comments explaining WHY, not just what.
- Upstream bugs worth reporting (correctness or crashing issues) go in `UPSTREAM_BUGS.md`. Compilation portability nits and test infrastructure issues do not.

## Docs

### Structure

Docs use Docusaurus 3.9 and follow the Diataxis framework:

| Category     | Path               | Purpose                              |
|--------------|--------------------|--------------------------------------|
| Tutorials    | `website/docs/tutorials/`  | Learning-oriented walkthroughs       |
| How-to       | `website/docs/how-to/`     | Task-oriented guides                 |
| Reference    | `website/docs/reference/`  | API overview, glossary, repo map     |
| Explanation  | `website/docs/explanation/` | Architecture and design discussion  |

Sidebar is auto-generated from directory structure (`website/sidebars.js`). Each category directory has a `_category_.json` for ordering and labels.

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
- The footer in `website/docusaurus.config.js` uses absolute paths -- that is the one place where absolute `/docs/...` paths are correct.

### Building and verifying

```sh
cd website
npm ci                  # Install deps (first time)
npm run build           # Build site to site-build/ -- this checks all links
npm start               # Dev server with hot reload
npm run serve           # Serve the built site locally
```

### Deployment

Docs are published to GitHub Pages via `.github/workflows/docs-pages.yml`. The custom domain is `quorumkit.org`. The root `/` redirects to `/docs/` via `website/src/pages/index.js`. The `website/static/CNAME` file ensures the custom domain persists across deploys.

## License

Apache License 2.0. The `NOTICE` file contains fork attribution to braft.
