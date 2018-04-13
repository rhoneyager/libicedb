---
title: Configure, Build and Install Debian / Ubuntu
tags: [getting_started, troubleshooting, installation, Debian, Ubuntu]
keywords: dependencies Linux Debian Ubuntu install
summary: "Installation on Linux is usually less problematic than on Windows"
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
sudo apt install cmake libhdf5-dev hdf5-tools git zlib1g-dev libnetcdf-dev libboost-program-options-dev libboost-dev libboost-filesystem-dev libboost-system-dev
```
To install optional dependencies:
```
sudo apt install libboost-all-dev cmake-curses-gui doxygen
```

If you have a very old installation or distribution, then you may need to use apt-get instead of apt.

## Compiler-specific instructions

### Debian Stretch / Ubuntu 16.04 LTS / Ubuntu 17.10 with GCC

This is the easiest option.

```
sudo apt install g++
```

Now, scroll down to the Build Instructions section.

### Debian Stretch / Ubuntu 16.04 LTS / Ubuntu 17.10 with clang

This is also very easy. Clang comes in multiple versions on this platform, including 3.8, 3.9, 5.0.

```
sudo apt install clang
```
Either run CMake with these options:
- CMAKE\_CXX\_COMPILER=/usr/bin/clang++
- CMAKE\_C\_COMPILER=/usr/bin/clang

or, invoke cmake this way:
```
CC=`which clang` CXX=`which clang` cmake ...
```

Conveniently, the hdf5 and boost libraries' C++11 bindings are compatible with clang on these distributions. No linking errors were observed.

Scroll down to the Build Instructions section.

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
- If the build is successful, binaries and libraries should be in the ./RelWithDebInfo directory.
- If you changed the CMAKE\_BUILD\_TYPE option to another setting, like "Debug" or "Release", the binaries and libraries will be in the ./Debug or ./Release directory, respectively. These options trigger different compiler switches, such as turning on/off debug symbols and different levels of compiler optimizations. There is a marked difference in the size and speed of the compiled programs.

- To copy the binaries to the install directory:
```
sudo make install
```
{% include note.html content="If you already have write privileges on your install directory, such as when it is in your home directory, then you can just run make install without sudo." %}

- To create archives of the source code and compiled binaries, try
```
make package
```
CMake can make many different types of packages. Consult the CMake / CPack documentation for details.


