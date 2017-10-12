# LIBICEDB

## Synopsis

Libicedb is a library and set of programs that manipulates snow scattering databases. It is under heavy development, so this readme file doesn't have much yet.

## Building and Installing

Requirements:
--------------

- A C++ 2011-compatible compiler. This includes any recent versions of gcc/g++, LLVM/clang, or Microsoft Visual C++.
- CMake (generates the build scripts and finds library locations)
- NetCDF, HDF5, ZLIB (needed to store and read the database)
- git (used to checkout the code; it provided some information to the build system)
- Doxygen (optional; generates html documentation of the library functions)

On Debian-based systems (e.g. Ubuntu), the necessary dependencies may be installed using this command:
```
sudo apt install cmake doxygen libhdf5-dev hdf5-tools git zlib1g-dev libnetcdf-dev libboost-all-dev
```
On Fedora, this command may be used:
```
sudo dnf install cmake doxygen hdf5-devel hdf5 git zlib-devel netcdf-devel boost-devel gcc-c++
```
On FreeBSD, use this command:
```
sudo pkg install hdf5 cmake doxygen git netcdf
```
On MacOS, install XCode and Homebrew (https://brew.sh), then run
```
brew install homebrew/science/netcdf doxygen hdf5 cmake git
```

Building:
-------------

- Download (and perhaps extract) the source code package. 
- Create a new build directory. It can be anywhere. Switch into this directory.
- Run cmake to generate the build scripts. e.g.:
```
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_INSTALL_PREFIX={install path} {Path to source directory}
```
- This command, on Linux, typically defaults to generating Makefiles and will use gcc as the compiler. Consult the CMake
   documentation for details regarding how to change the compiler or other settings.
   - To change the compiler, prefix the CMake command with CMAKE_CXX_COMPILER='path to compiler'.
- If cmake is set to generate Makefiles, run:
```
make
```
- If the build is successful, binaries and libraries should be in the ./RelWithDebInfo directory. These can all be copied
to the install directory using:
```
sudo make install
```

## Warning

This library is quite new and under heavy development. Expect much to change, and use at your own risk.

## License

See [LICENSE.txt](./LICENSE.txt)

