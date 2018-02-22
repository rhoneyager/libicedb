---
title: Installation directory structure
tags: [getting_started, troubleshooting, installation]
keywords: installation
summary: "Describes the layout of an icedb installation"
sidebar: mydoc_sidebar
permalink: install_dir.html
folder: mydoc
---

The directory layout of icedb is customizable at build time. By default, icedb
is entirely encapsulated in a single directory tree, and is usually installed to a
location like "/usr/local/icedb", "/opt/icedb" or "/your/home/directory/icedb".

## Top-level directory structure

- The "bin" directory contains all executable programs.
- The "include" directory contains all header files used for C and C++ development.
- The "lib" directory contains libraries, along with any linker-dependent
  modules needed to link your code to icedb.
- The "share" directory contains examples, documentation and configuration files.

## The "bin" directory

Programs are grouped into
subfolders depending on build settings. So, if you build using g++ on Linux, with
debugging symbols, then the programs are placed in bin/bin\_x64\_GNU/Debug.
POSIX-compatible systems place shared libraries under "lib" (see below), but
on the Microsoft Windows platform, shared libraries are placed in "bin".

## The "lib" directory
icedb libraries are stored in various subdirectories, depending on the build configuration.
Multiple copies icedb are usually present; these copies were all built under different
compiler options, possibly for different system architectures and using different
compilers. These libraries are kept separate to avoid linking and runtime errors, as
linking modules of code on any system is highly complex.

The "CMake"
subdirectory is special - it contains configuration files that allow you to 
easily link your own code against icedb. For linking instructions, see
the [Linking Your Code with icedb](./linking.html) page.


