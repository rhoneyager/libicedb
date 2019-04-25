# CMake Build Options                                 {#cmakeoptions}

## General

### CMAKE_BUILD_TYPE

Options are Debug, Release, RelWithDebInfo, MinSizeRel.
This is a "master option" that determines the optimization level of the 
compiler and turns on/off the presense of debugging symbols within the
compiled libraries and executables.

## Documentation

### BT_BUILD_DOCUMENTATION

Options are No, Separate, and Integrated.
- "No" means that no documentation is built.
- "Separate" means that documentation may be build with 
the ```make docs``` command, but this documentation is not incorporated
into either the install tree or any packages.
- "Integrated" causes the documentation to be build with the ```make``` command.
Documentation is incorporated into the installation path and 
into packages.

Documentation requires the presence of [Doxygen](http://www.doxygen.nl).
The "dot" command from [GraphViz](https://www.graphviz.org/) is optional, and suggested.

## Testing

### BT_TEST

Options are yes or no. Turn this option on to build the test executables.
It allows you to run the ```make test``` command. Highly recommended.

### BT_TEST_WITH_BOOST

Options are yes or no. This option allows for Boost-dependent test
programs to be compiled and run. Highly recommended.
