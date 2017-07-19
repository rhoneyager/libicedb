#pragma once
#ifndef ICEDB_PLUGIN_NETCDF_H_INTERFACE
#define ICEDB_PLUGIN_NETCDF_H_INTERFACE

#include "../../libicedb/icedb/dlls/linking.h"
ICEDB_DLL_INTERFACE_BEGIN(netcdf)
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(netcdf, nc_open, int, const char*, int, int*)
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(netcdf, nc_close, int, int)
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(netcdf, nc_create, int, const char*, int, int*)
ICEDB_DLL_INTERFACE_END
#endif
