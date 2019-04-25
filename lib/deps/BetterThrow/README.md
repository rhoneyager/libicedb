# BetterThrow                                        {#mainpage}

If you are viewing the README on GitHub, [click here to go to the documentation site](https://rhoneyager.github.io/BetterThrow-devel/).

## Introduction

BetterThrow does two things:
1. It helps C++ developers write exception handling and error 
reporting code by providing a versatile exception class.
2. It abstracts many useful system-dependent (WinAPI, POSIX, macOS API) 
functions for querying information about an application's runtime environment.

Like Boost::Exception, it supports transporting of arbitrary data to the catch site,
and data can be added to any object, either directly in a throw expression, or at
a later time as the exception object propagates up the call stack.

The ability to add data to exception objects after they have been passed to 
throw is important, because often some of the information needed to handle 
an exception is unavailable in the context where the failure is detected.

Unlike Boost::Exception, developers do not have to define compile-time 
error_info tags. BetterThrow does not focus on creating exception hierarchies; 
it instead focuses on providing useful diagnostic information to developers.

## License
BetterThrow is released under the [BSD 2-Clause License](@ref LICENSE).

## Get the code
Available on GitHub at [https://github.com/rhoneyager/BetterThrow-devel](https://github.com/rhoneyager/BetterThrow-devel).

## Building the code

System Requirements:
- A C++ compiler that supports at least C++11.
- [CMake](https://cmake.org/).

Build instructions:
1. Get the code
2. Create an empty "build directory" where the sources will be compiled.
3. Run ```cmake```, ```ccmake```, or ```cmake-gui``` to produce the 
build system files. Options are described in 
[the build options section](@ref cmakeoptions).
4. Compile the sources, generally using ```make```.
5. Run the tests (e.g. ```make test```).
6. (Optional) Install (```make install```) or create a binary 
package (```make package```).

## Linking to your projects

BetterThrow exports CMake package information.

Use ```find_package(BetterThrow)``` to find the package.
Use ```target_link_libraries({your-app-or-lib-name} BetterThrow::BetterThrow)``` 
to link to the package.

Header files are available under the "BetterThrow/" path.

NOTE: On certain platforms, such as Windows, there are multiple versions of the 
C++ runtime library. Make sure that you use matching runtimes when 
building and linking both your code and BetterThrow. Otherwise, and resulting 
applications will either crash or fail to link.

## Function Documentation

[See the Error documentation](@ref Error) for details on using the exception class.

[See the Info documentation](@ref Info) for details on the system runtime environment functions.
