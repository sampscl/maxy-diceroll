# 🦖🎲 Maxy-diceroll 🎲🦖
Dice rolling TO THE MAX

## Pre-requisites
- A compiler and the usual toolchain
- CMake

On Ubuntu:
```bash
sudo apt install build-essential cmake
```

# Compiling
Maxy-diceroll uses build scripts and out of tree builds (into the ./build dir):

```bash
scripts/clean.sh &&
scripts/build.sh &&
scripts/deploy.sh && echo Successful
```
This will work on ubuntu and other unbuntu-like distros (e.g. kubuntu, mint,
etc.).

More stuff is supported:
```bash
CMAKE_BUILD_TYPE=Debug scripts/build.sh # build Debug build (default is Release)
TARGET=all CMAKE_BUILD_TYPE=Debug scripts/build.sh # build debug build and tests too
```

Run the generated programs:
```bash
build/tests/run_test # run the tests
build/src/maxy_diceroll # run the program
build/src/maxy_diceroll --port=8080 --ui-dir=`pwd`/ui # with some opts
```
