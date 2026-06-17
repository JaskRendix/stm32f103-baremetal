## Build

Original project: **[https://github.com/gnif/STM32F103-BareMetal](https://github.com/gnif/STM32F103-BareMetal)**

### Release

```
make
```

This runs:

- CMake (Ninja generator)
- Ninja build
- Produces `build/out.elf`

### Debug

```
make BUILD_TYPE=Debug
```

If you want a dedicated debug directory instead, use:

```
cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug -B build_debug
ninja -C build_debug
```

### Clean

```
make clean
```

### Full rebuild

```
make rebuild
```
