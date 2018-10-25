---
title: Configure, Build and Install on Mac OS X
tags: [getting_started, troubleshooting, installation, OS_X, Macintosh, Mac]
keywords: dependencies install Macintosh Mac
summary: "Installation on Mac OS X"
sidebar: mydoc_sidebar
permalink: install_deps_on_osx.html
folder: mydoc
---

## Basic dependencies


Each development environment is different. Fortunately, icedb has few dependencies.

Requirements:

- A C++ 2014-compatible compiler. This includes any recent versions of gcc/g++, LLVM/clang, or the Intel C++ compilers.
- CMake (generates the build scripts and finds library locations)
- NetCDF, HDF5, ZLIB (needed to store and read the database)
- git (it provides some information to the build system)
- Doxygen (optional; generates local html documentation of the library functions)


On MacOS, install XCode and Homebrew (https://brew.sh), then run
```
brew install netcdf doxygen hdf5 cmake git boost
```

{% include note.html content="TODO: Finish these" %}


