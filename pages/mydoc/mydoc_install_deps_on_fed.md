---
title: Install Dependencies on CentOS / Fedora / Red Hat Enterprise Linux
tags: [getting_started, troubleshooting, installation, Fedora, RHEL, CentOS]
keywords: dependencies Fedora RHEL Red Hat Enterprise Linux CentOS install
summary: "Installation of dependencies on Linux is usually less problematic than on Windows. However, CentOS and RHEL are rather conservative with their package choices, so additional steps may be required."
sidebar: mydoc_sidebar
permalink: install_deps_on_fed.html
folder: mydoc
---

## Basic dependencies

{% include note.html content="If your system is not based on CentOS, Fedora, Red Hat Enterprise Linux, or a similar system, then you are reading the wrong set of instructions." %}


Each development environment is different. Fortunately, icedb has few dependencies.

Requirements:
--------------

- A C++ 2011-compatible compiler. This includes any recent versions of gcc/g++, LLVM/clang, or the Intel C++ compilers.
- CMake (generates the build scripts and finds library locations)
- NetCDF, HDF5, ZLIB (needed to store and read the database)
- git (it provides some information to the build system)
- Doxygen (optional; generates local html documentation of the library functions)


To install these dependencies on CentOS, Fedora and RHEL, this command may be used:
```
sudo yum install cmake doxygen hdf5-devel hdf5 git zlib-devel netcdf-devel boost-devel
```

## Compiler-specific instructions

### CentOS 7 / Fedora / RHEL 7 with GCC

This is the easiest option.
{% include note.html content="Any recent (i.e. still supported) Fedora Linux will work. New Fedora releases occur twice a year, and they are only supported for about a year after release." %}

```
sudo yum install gcc-c++
```

### CentOS 6 / RHEL 6 with GCC 6 or 7

CentOS and Red Hat Enterprise Linux 6 are compiled using GCC 4.2, and this compiler is
too old for building icedb. It predates full C++11 support.

Fortunately, pre-built compilers are available through add-on packages provided by
the [Software Collections](https://www.softwarecollections.org) repository.

This is [Red Hat's recommended method](https://access.redhat.com/documentation/en-us/red_hat_enterprise_linux/7/html/developer_guide/scl-utils) for obtaining recent GCC compilers for RHEL, and they provide installation instructions.

For CentOS, [equivalent instructions are provided on the Software Collections website](https://www.softwarecollections.org/en/scls/rhscl/devtoolset-7/).

Make sure that you are in the correct environment before proceeding to the build phase.
For both instruction sets, the final command spawns a new sub-shell with environment variables pre-set to point to the correct compiler:
```
scl enable devtoolset-7 bash
```

The version of CMake provided by these systems is insufficient. Conveniently, the [CMake download page](https://cmake.org/download/) provides links to pre-compiled binaries for Linux. Download and extract the CMake binaries to a location like /your/home/directory/bin, /usr/local/bin, or another appropriate location. It would help if this location were in your PATH.

### CentOS 7 / Fedora / RHEL 6 / RHEL 7 with Clang (Work in Progress; Needs Further Testing)

{% include note.html content="Not available for CentOS 6. Unavailable to test on RHEL." %}

CentOS 7, Fedora, RHEL 6, and RHEL 7 provide the LLVM/Clang 3.4 compiler, which fully supports C++11. The command to install Clang is:
```
sudo yum install clang
```

You will need to pass environment variables to the build system to indicate that Clang is your preferred compiler. On Bash, this is:
```
export C=/your/path/to/clang
export CXX=/your/path/to/clang++
```
Or, tell CMake your preferred compiler directly by specifying options:
- CMAKE\_CXX\_COMPILER=/your/path/to/clang++
- CMAKE\_C\_COMPILER=/your/path/to/clang

Make sure that you have the correct environment loaded before proceeding to the build phase.

### Other RHEL-like systems

If the system is based on CentOS or Red Hat Enterprise Linux 7, follow those instructions.
If the system is based on Fedora, follow those instructions.

If the system is older, or if you cannot install packages on your system, then you may need to compile the compiler yourself. For GCC, see the instructions at the [GNU Compiler Collection Website](https://gcc.gnu.org/). For Clang, see [their website](https://clang.llvm.org/).
Your mileage may vary. You will also probably have to compile HDF5 ([link](https://www.hdfgroup.org/downloads/hdf5/)) and zlib ([link](https://zlib.net/)). If possible, use a virtual machine with a more recent Linux distribution.


