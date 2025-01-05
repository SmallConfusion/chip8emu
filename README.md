# Chip 8 emulator

This is a chip 8 emulator, it does not support schip or any chip extensions.

## Building

Cmake will handle downloading the dependencies, because of that, configuration
may take a while if you don't have the fastest internet speed.

```console
$ cmake -S . -B build
$ cmake --build build
```

Or if you prefer the older way

```console
$ mkdir build
$ cd build
$ cmake ../
$ make
```
