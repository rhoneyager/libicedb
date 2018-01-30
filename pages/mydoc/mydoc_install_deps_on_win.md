---
title: Configure, Build and Install on Windows
tags: [getting_started, troubleshooting, installation, Windows]
keywords: dependencies Windows install
summary: "Installation on Windows is rather painful, but the Visual Studio debugger is excellent."
sidebar: mydoc_sidebar
permalink: install_deps_on_win.html
folder: mydoc
---

## Basic dependencies

{% include note.html content="If your system is not Windows, then you are reading the wrong set of instructions." %}


Each development environment is different. Fortunately, icedb has few dependencies.

Requirements:
--------------

- A C++ 2011-compatible compiler. This includes any recent versions of gcc/g++, LLVM/clang, Microsoft Visual Studio 2017, or the Intel C++ compilers.
- CMake (generates the build scripts and finds library locations)
- NetCDF, HDF5, ZLIB (needed to store and read the database)
- git (it provides some information to the build system)
- Doxygen (optional; generates local html documentation of the library functions)


{% include note.html content="TODO: Finish these" %}


