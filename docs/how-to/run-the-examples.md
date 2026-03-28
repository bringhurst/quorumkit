---
title: Run the examples
sidebar_position: 3
---

# Run the examples

The examples are the quickest way to see the library behaving like a system instead of a set of headers.

## Available examples

- `example/counter`
- `example/atomic`
- `example/block`

## Basic flow

Pick one example and build it locally:

```sh
cd example/counter
cmake .
make
```

Start the local servers:

```sh
bash run_server.sh
```

Then run a client in another shell:

```sh
bash run_client.sh
```

## What to watch for

- local runtime state under the example directory
- leader selection and request forwarding behavior
- client-visible command flow
- snapshot save/load logic in the example state machine

If you want the role each example plays, see [Examples](../reference/examples).
