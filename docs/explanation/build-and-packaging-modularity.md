---
title: Build and packaging modularity
sidebar_position: 10
---

# Build and packaging modularity

Build tooling should follow the architecture, not define it.

```text
cmake/       canonical CMake modules, targets, install, export logic
bazel/       Bazel-only wrappers and BUILD glue
packaging/   package-manager metadata and integration helpers
```

The source tree decides what is public, what is internal, and where optional pieces plug in. Build and packaging tools should consume that structure rather than forcing the repository into tool-specific shapes.
