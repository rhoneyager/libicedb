---
title: Configure, Build and Install on CentOS / Fedora / Red Hat Enterprise Linux
tags: [getting_started, troubleshooting, installation, Fedora, RHEL, CentOS]
keywords: dependencies Fedora RHEL Red Hat Enterprise Linux CentOS install
last_updated: September 26, 2018
summary: "Installation on Linux is usually less problematic than on Windows. However, CentOS and RHEL are rather conservative with their package choices, so additional steps may be required."
sidebar: mydoc_sidebar
permalink: install_deps_on_fed.html
folder: mydoc
---

## Basic dependencies

{% include note.html content="If your system is not based on CentOS, Fedora, Red Hat Enterprise Linux, or a similar system, then you are reading the wrong set of instructions." %}


Each development environment is different. Fortunately, icedb has few dependencies.

Requirements:
--------------

- A C++ 2014-compatible compiler. This includes any recent versions of gcc/g++, LLVM/clang, or the Intel C++ compilers.
- CMake (generates the build scripts and finds library locations)
- HDF5, ZLIB (needed to store and read the database)
- git (it provides some information to the build system)
- Doxygen (optional; generates local html documentation of the library functions)
- NetCDF (optional; used for build tests)
- The Linux Standard Base system packages.


To install these dependencies on CentOS, Fedora and RHEL, this command may be used 
(note: yum has been replaced with **dnf** on certain distributions):
```
sudo yum install cmake doxygen hdf5-devel hdf5 git zlib-devel netcdf-devel boost-devel redhat-lsb-core make
```

Optional packages may be installed with:
```
sudo yum install rpm-build
```

## Compiler-specific instructions

### Fedora 28 with gcc

This is the easiest option.
{% include note.html content="Any recent (i.e. still supported) Fedora Linux will work. New Fedora releases occur twice a year, and they are only supported for about a year after release." %}

```
sudo yum install gcc-c++
```

### CentOS 7 / RHEL 6 using the SCL toolsets

CentOS and Red Hat Enterprise Linux 6 are compiled using GCC 4.2, and this compiler is
too old for building icedb. It predates full C++11 support.

Fortunately, pre-built compilers are available through add-on packages provided by
the [Software Collections](https://www.softwarecollections.org) repository.

This is [Red Hat's recommended method](https://access.redhat.com/documentation/en-us/red_hat_enterprise_linux/7/html/developer_guide/scl-utils) for obtaining recent GCC compilers for RHEL, and they provide installation instructions.

For CentOS, [equivalent instructions are provided on the Software Collections website](https://www.softwarecollections.org/en/scls/rhscl/devtoolset-7/).

Make sure that you are in the correct environment before proceeding to the build phase.
For both instruction sets, the final command spawns a new sub-shell with environment 
variables pre-set to point to the correct compiler:
```
scl enable devtoolset-7 bash
```

The version of CMake provided by these systems might be insufficient. 
Conveniently, the [CMake download page](https://cmake.org/download/) provides links to 
pre-compiled binaries for Linux. Download and extract the CMake binaries to a 
location like /your/home/directory/bin, /usr/local/bin, or another appropriate 
location. It would help if this location were in your PATH.


### Older OS versions

Try building using recent compilers provided by the 
[Conda Build System](./install_using_conda.html).

The version of CMake provided by CentOS and RHEL may be insufficient. 
Conveniently, the [CMake download page](https://cmake.org/download/) provides 
links to pre-compiled binaries for Linux. Download and extract the CMake binaries 
to a location like /your/home/directory/bin, /usr/local/bin, or another appropriate 
location. It would help if this location were in your PATH.

If you do not want to use Conda, then might be able to compile the compiler and
all of the dependencies yourself. Your mileage may vary.
For GCC, see the instructions at the [GNU Compiler Collection Website](https://gcc.gnu.org/).
For Clang, see [their website](https://clang.llvm.org/).
You will also probably have to compile HDF5 ([link](https://www.hdfgroup.org/downloads/hdf5/)),
zlib ([link](https://zlib.net/)) and Boost ([link](https://www.boost.org)).
If possible, use a virtual machine with a more recent Linux distribution.

Occasionally, you may need to also set -D\_GLIBCXX\_USE\_CXX11\_ABI=0 as a
CMake option, if you are linking using a self-compiled compiler combined with the
distribution-packaged libraries.

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


