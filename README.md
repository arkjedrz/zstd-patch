# zstd-patch

Example on how to use `libzstd` as a patching engine.

The aim of this project is to recreate the behavior of `--patch-from` and `--patch-apply` features of a `zstd` CLI app.

## Build instructions

### Requirements

Tools used for development:

- Fedora Linux 42
- Zstandard 1.5.7
- CMake 3.31.6
- Ninja 1.12.1

### Commands

```bash
mkdir build && cd build
cmake -G "Ninja" ..
cmake --build . -- -j $(nproc)
```

## Usage

### Apply patch mode

To apply patch use:

```bash
./zstd-patch apply <PATH TO OLD VERSION> <PATH TO PATCH> <PATH TO NEW VERSION>
```

This should behave like:

```bash
zstd -d --patch-from <PATH TO OLD VERSION> <PATH TO PATCH> -o <PATH TO NEW VERSION>
```

### Create patch mode

To generate patch file use:

```bash
zstd --patch-from <PATH TO OLD VERSION> <PATH TO NEW VERSION> -o <PATH TO PATCH>
```
