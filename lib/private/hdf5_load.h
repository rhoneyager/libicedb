#pragma once

#ifdef _MSC_FULL_VER
#pragma warning( push )
#pragma warning(disable:4251)
#endif
#include <hdf5.h>
#include <H5Cpp.h>
#include <hdf5_hl.h>

// HDF5 functions jump around between base classes in the C++
// library. This is unfortunate.
// H5pubconf.h defines a H5_VERSION macro that can help.



#ifdef _MSC_FULL_VER
#pragma warning( pop )
#endif
