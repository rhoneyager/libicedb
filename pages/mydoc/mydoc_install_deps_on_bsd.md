---
title: Configure, Build and Install on BSDs
tags: [getting_started, troubleshooting, installation, FreeBSD, BSD, OpenBSD, NetBSD, DragonflyBSD]
last_updated: January 27, 2018
keywords: dependencies, BSD, FreeBSD, OpenBSD, NetBSD, DragonflyBSD, install
summary: "Installation on recent versions of FreeBSD is easy. Other BSDs, like OpenBSD, DragonFlyBSD and NetBSD, should also work well."
sidebar: mydoc_sidebar
permalink: install_deps_on_bsd.html
folder: mydoc
---

## Basic dependencies

{% include note.html content="If your system is Linux-based, then you are reading the wrong set of instructions." %}


Each development environment is different. Fortunately, icedb has few dependencies. The instructions here are primarity for FreeBSD, but can be readily adapted for other systems.

Requirements:
--------------

- A C++ 2011-compatible compiler. This includes any recent versions of gcc/g++, LLVM/clang, or the Intel C++ compilers.
- CMake (generates the build scripts and finds library locations)
- NetCDF, HDF5, ZLIB (needed to store and read the database)
- git (it provides some information to the build system)
- Doxygen (optional; generates local html documentation of the library functions)


To install these dependencies on FreeBSD-based systems, this command may be used:
```
sudo pkg install hdf5 cmake doxygen git netcdf
```
On NetBSD and OpenBSD, pkg-add should be used instead. The package names remain the same.

## Compiler-specific instructions

### FreeBSD 11.1 with Clang

Clang 3.8 is the default compiler on this OS. It should be preinstalled.

```
sudo pkg install clang
```
CMake options for reference (if Clang somehow is not your default, perhaps if you are not using FreeBSD):
- CMAKE\_CXX\_COMPILER=/usr/bin/clang++
- CMAKE\_C\_COMPILER=/usr/bin/clang

### FreeBSD 11.1 with GCC

```
sudo pkg install gcc7
```
Run CMake with these options:
- CMAKE\_CXX\_COMPILER=/usr/local/bin/g++7
- CMAKE\_C\_COMPILER=/usr/local/bin/gcc7

## Build instructions


- Download (and perhaps extract) the source code package. 
- Create a new build directory. It can be anywhere. Switch into this directory.
- Run cmake to generate the build scripts. e.g.:
```
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_INSTALL_PREFIX={install path} {Path to source directory}
```
- This command, on Linux, typically defaults to generating Makefiles and will use gcc as the compiler. Consult the CMake
   documentation for details regarding how to change the compiler or other settings.
   - To change the compiler, the CMake option is -DCMAKE\_CXX\_COMPILER='path to compiler' for C++ and -DCMAKE\_C\_COMPILER='path' for C.
- If cmake is set to generate Makefiles, run:
```
make
```
- If the build is successful, binaries and libraries should be in the ./RelWithDebInfo directory. These can all be copied
to the install directory using:
```
sudo make install
```

{% include note.html content="If you already have write privileges on your install directory, such as when it is in your home directory, then you can just run make install without sudo." %}


