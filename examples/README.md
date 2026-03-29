Brief introduction of examples:

* Counter: An integer that can be incremented by a given amount in each request.
* Atomic: An integer that supports exchange and compare_exchange operations.
* Block: A single block device that supports concurrent random read/write.

# Build steps

From the repository root:

```sh
conan export contrib/brpc/
conan install . --output-folder=build --build=missing
cmake --preset conan-release -DBUILD_EXAMPLES=ON
cmake --build --preset conan-release
```

This builds the example binaries into `build/build/Release/examples/`.

# Run Server

```sh
cd examples/counter   # or atomic / block
bash run_server.sh
```

* Default number of servers in the group is `3`, changed by `--server_num`
* Servers run in `./runtime/`, which is reused unless you pass `--clean=0`
* The run scripts automatically look for binaries in the source directory first, then in the top-level CMake build tree

# Run Client

```sh
cd examples/counter   # or atomic / block
bash run_client.sh
```

* Default client concurrency is 1, changed by `--thread_num`
* If `run_server.sh` uses a different `--server_num`, pass the same value to `run_client.sh`
* Add `--log_each_request` if you want detailed per-request logging
