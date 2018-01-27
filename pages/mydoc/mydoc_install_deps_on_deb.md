---
title: Install Dependencies on Debian / Ubuntu
tags: [getting_started, troubleshooting, installation, Debian, Ubuntu]
keywords: dependencies Linux Debian Ubuntu install
summary: "Installation of dependencies on Linux is usually less problematic than on Windows"
sidebar: mydoc_sidebar
permalink: install_deps_on_deb.html
folder: mydoc
---

## Basic dependencies

{% include note.html content="If your system is not based on Debian, Ubuntu, or a similar system, then you are reading the wrong set of instructions." %}


Each development environment is different. Fortunately, icedb has few dependencies.

Requirements:
--------------

- A C++ 2011-compatible compiler. This includes any recent versions of gcc/g++, LLVM/clang, or the Intel C++ compilers.
- CMake (generates the build scripts and finds library locations)
- NetCDF, HDF5, ZLIB (needed to store and read the database)
- git (it provides some information to the build system)
- Doxygen (optional; generates local html documentation of the library functions)


To install these dependencies on Debian-based systems, this command may be used:
```
sudo apt update
sudo apt install cmake doxygen libhdf5-dev hdf5-tools git zlib1g-dev libnetcdf-dev libboost-all-dev
```
If you have a very old installation or distribution, then you may need to use apt-get instead of apt.

## Compiler-specific instructions

### Debian Stretch / Ubuntu 16.04 LTS / Ubuntu 17.10 with GCC

This is the easiest option.

```
sudo apt install g++
```

### Debian Stretch / Ubuntu 16.04 LTS / Ubuntu 17.10 with clang

This is also very easy. Clang comes in multiple versions on this platform, including 3.8, 3.9, 5.0.

```
sudo apt install clang
```
Run CMake with these options:
- CMAKE\_CXX\_COMPILER=/usr/bin/clang++
- CMAKE\_C\_COMPILER=/usr/bin/clang

Conveniently, the hdf5 and boost libraries' C++11 bindings are compatible with clang on these distributions. No linking errors were observed.


### Ubuntu 14.04 LTS (Work in Progress) with clang only

The provided GCC compilers predate full C++11 support. They will not work.

The clang compiler, version 3.8 or 3.9 are available, but clang's default C++ Standard Template Library (STL) headers are the ones provided by GCC (in the libstdc++ package).

You will need to install several packages:
```
sudo apt install libc++-dev libc++abi-dev clang-3.8
```
You will have to install an updated CMake, available from [here](https://cmake.org/download/).

Run CMake with these options:
- CMAKE\_CXX\_COMPILER=/usr/bin/clang++-3.8
- CMAKE\_C\_COMPILER=/usr/bin/clang-3.8
- CMAKE\_CXX\_FLAGS=-I/usr/include/c++/v1
- CMAKE\_EXE\_LINKER\_FLAGS=-nodefaultlibs -lc++ -lc++abi -lm -lc -lgcc\_s -lgcc
- CMAKE\_SHARED\_LINKER\_FLAGS=-nodefaultlibs -lc++ -lc++abi -lm -lc -lgcc\_s -lgcc

When building, boost::filesystem is missing a few small functions. These may be worked around in the future. The rest of the build is untested.

### Debian jessie

The default GCC compiler is version 4.9.2, which is too old.
The available clang compiler is version 3.5, which is also too old.
You will have to compile the compiler from sctatch, and may also have to provide updated headers.
You will have to install an updated CMake, available from [here](https://cmake.org/download/).

### Other Debian-like systems

If the system is based on Debian, follow those instructions.
If the system is based on Ubuntu, follow those instructions.

If the system is older, or if you cannot install packages on your system, then you may need to compile the compiler yourself. For GCC, see the instructions at the [GNU Compiler Collection Website](https://gcc.gnu.org/). For Clang, see [their website](https://clang.llvm.org/).
Your mileage may vary. You will also probably have to compile HDF5 ([link](https://www.hdfgroup.org/downloads/hdf5/)) and zlib ([link](https://zlib.net/)). If possible, use a virtual machine with a more recent Linux distribution.


