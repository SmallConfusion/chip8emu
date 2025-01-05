# Chip 8 emulator

This is a chip 8 emulator, it does not support schip or any chip extensions.

Press space to open or close the menus.

## Running

### Windows

Downloads are available in the releases. If you do not have the Microsoft
Visual C++ Redistributable installed on your computer, you will need to.
Instructions and downloads can be found at
https://learn.microsoft.com/en-us/cpp/windows/latest-supported-vc-redist?view=msvc-170#latest-microsoft-visual-c-redistributable-version

### Linux and Mac

Build it from source idk you probably know what you're doing.


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
