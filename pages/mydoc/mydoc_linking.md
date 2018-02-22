---
title: Linking your apps to icedb
tags: [getting_started, troubleshooting, installation]
keywords: linking
summary: "Use CMake to link code to the icedb libraries"
sidebar: mydoc_sidebar
permalink: linking.html
folder: mydoc
---

For now, icedb is distributed as a static library. Any changes to the library
will require you to recompile any dependent code.

## Using CMake

When icedb is installed, the "lib/CMake" directory is provisioned with
a series of CMake scripts that allow for the easy linking of external code.

Here is a minimal example: [Download the example source](examples/icedb-linking-example.tar.bz2)

test.cpp: --- This is the example program that is built
```
#include <icedb/defs.h>
#include <iostream>
int main() {
        using namespace std;
        cout << "Linked\n";
        return 0;
}
```

CMakeLists.txt --- This is the CMake configuration file that finds icedb and builds the application.
```
cmake_minimum_required (VERSION 3.1)
project(icedbLinkTest C CXX)

# Find icedb
find_package(icedb REQUIRED)
add_executable(link-test test.cpp)
# Link the test program to icedb
target_link_libraries(link-test icedb)
# Add icedb to the program's include directories (i.e. for finding the icedb header files)
target_include_directories(link-test SYSTEM PRIVATE ${icedb_INCLUDE_DIRS})
```

In your project, you need to find the appropriate icedb package. The "find\_package(icedb REQUIRED)" command instructs CMake to find this package. CMake will look for an "icedbConfig.cmake" file; you can pass the location of this file by setting the "icedb\_DIR" in CMake or as an environment variable. 

For example, if you are building for the amd64 architecture using g++, then the appropriate option is:
```
icedb_DIR=/your/icedb/install/path/lib/CMake/icedb/conf_x64_GNU
```

## Not using CMake

To link to icedb, you need to give pass the paths of the icedb include and library directories to the compiler, and also need to link to the appropriate icedb library.

g++ example:
```
g++ -olink-test test.cpp -L/your/path/to/the/icedb/root/lib/lib_x64_GNU -licedb_Release_x64_GNU -I/your/path/to/the/icedb/root/include
```

