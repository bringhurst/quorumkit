---
title: Troubleshoot common problems
sidebar_position: 5
---

# Troubleshoot common problems

## Docs build fails with broken links

```sh
npm run build
```

Read the error output -- it tells you exactly which file has a bad link and what path it tried to resolve. Common causes:

- A page was moved during the Diataxis restructure but a link still points to the old path.
- A custom slug does not match the file's actual location.

## `docusaurus: command not found`

The Docusaurus binary comes from the project's npm dependencies. Install them first:

```sh
npm ci
```

## Example does not build

Build the main library first. The examples link against QuorumKit, so they need the library to exist:

```sh
cmake -S . -B bld && cmake --build bld
```

Then build the example using its own CMake:

```sh
cd example/counter
cmake . && make
```

## GitHub Pages shows a 404

The production docs are served at `/docs/`, not at the site root. The root page (`/`) redirects to `/docs/`. If you see a 404 at the root, the redirect page (`src/pages/index.js`) may not have been deployed.
