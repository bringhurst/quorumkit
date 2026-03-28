---
title: Troubleshoot common problems
sidebar_position: 5
---

# Troubleshoot common problems

This page collects the annoying, ordinary failures that slow down local work.

## Docs build fails with broken links

Run:

```sh
npm run build
```

Then read the broken link output carefully. In this docs tree, the usual culprits are:

- moved pages after the Diataxis restructure
- links that still point at old paths
- custom slugs such as `/docs/architecture`

## `docusaurus: command not found`

Install docs dependencies first:

```sh
npm ci
```

## Example build issues

If an example does not build, verify that the main repository build completed first and that you are using the example’s own local CMake flow.

## GitHub Pages confusion

The production docs site is published under `/docs`, not at the bare site root. The root page redirects to `/docs/`.
