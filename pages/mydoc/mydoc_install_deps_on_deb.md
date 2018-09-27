---
title: Configure, Build and Install Debian / Ubuntu
tags: [getting_started, troubleshooting, installation, Debian, Ubuntu]
keywords: dependencies Linux Debian Ubuntu install
last_updated: September 26, 2018
summary: "Installation on Debian and Ubuntu-based Linuxes"
sidebar: mydoc_sidebar
permalink: install_deps_on_deb.html
folder: mydoc
---

## Basic dependencies

{% include note.html content="If your system is not based on Debian, Ubuntu, or a similar system, then you are reading the wrong set of instructions." %}


Each development environment is different. Fortunately, icedb has few dependencies.

Requirements:
--------------

- A C++ 2014-compatible compiler. This includes any recent versions of gcc/g++, LLVM/clang, or the Intel C++ compilers.
- CMake (generates the build scripts and finds library locations)
- HDF5, ZLIB (needed to store and read the database)
- git (it provides some information to the build system)
- Doxygen (optional; generates local html documentation of the library functions)
- Graphviz (optional; used in generating some documentation)
- NetCDF (optional; used for build tests)
- LLNL SILO (optional; a visualization plugin)
- Eigen (optional; a linear algebra library)


To install these dependencies on Debian-based systems, this command may be used:
```
sudo apt update
sudo apt install cmake libhdf5-dev git zlib1g-dev libboost-all-dev 
```
To install optional dependencies:
```
sudo apt install cmake-curses-gui doxygen graphviz libsilo-dev libeigen3-dev netcdf-bin libnetcdf-dev hdf5-tools
```

If you have a very old installation or distribution, then you may need to use apt-get instead of apt.

## Compiler-specific instructions

### Ubuntu 16.04 LTS / Ubuntu 18.04 LTS with GCC

This is the easiest option.

```
sudo apt install g++
```

Now, scroll down to the Build Instructions section.

### Ubuntu 18.04 LTS / Ubuntu 17.10 with clang

This is also very easy. 

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

Scroll down to the Build Instructions section.

### Older Ubuntu versions, and most Debian versions

This includes Debian Stretch and Ubuntu 14.04.

Try building using recent compilers provided by the [Conda Build System](./install_using_conda.html).

### Other Debian-like systems

If the system is based on Debian, follow those instructions.
If the system is based on Ubuntu, follow those instructions.

## Build instructions


- Download (and perhaps extract) the source code package. 
- Create a new build directory. It can be anywhere. Switch into this directory.
- Run cmake to generate the build scripts. e.g.:
```
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX={install path} {Path to source directory}
```
- This command, on Linux, typically defaults to generating Makefiles and will use gcc as the compiler. Consult the CMake
   documentation for details regarding how to change the compiler or other settings.
   - To change the compiler, the CMake option is -DCMAKE\_CXX\_COMPILER='path to compiler' for C++ and -DCMAKE\_C\_COMPILER='path' for C.
- Alternative GUI interfaces to cmake include ccmake (curses interface) and cmake-gui (windowed).
- If cmake is set to generate Makefiles, run:
```
make
```
- If the build is successful, binaries and libraries should be in the ./Release directory.
- If you changed the CMAKE\_BUILD\_TYPE option to another setting, like "Debug" or "Release", the binaries and libraries will be in the ./Debug or ./Release directory, respectively. These options trigger different compiler switches, such as turning on/off debug symbols and different levels of compiler optimizations. There is a marked difference in the size and speed of the compiled programs.
- Run the built-in tests on the executables:
```
make test
```

If these tests do not all succeed, please email.
- The icedb installation can be copied to an installation directory using:
```
sudo make install
```
{% include note.html content="If you already have write privileges on your install directory, such as when it is in your home directory, then you can just run make install without sudo." %}

- To create redistributable packages of the compiled binaries, try
```
make package
```
CMake can make many different types of packages.
You may have to set a CMake option to specify the type of package that you want. 
Consult the CMake / CPack documentation for details.


