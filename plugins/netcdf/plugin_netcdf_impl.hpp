#pragma once
#ifndef ICEDB_PLUGIN_NETCDF_HPP_IMPL
#define ICEDB_PLUGIN_NETCDF_HPP_IMPL

#include "plugin_netcdf_interface.h"
#include "../../libicedb/icedb/dlls/dllsImpl.hpp"
/*
ICEDB_DLL_INTERFACE_BEGIN(netcdf)
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(netcdf, nc_open, int, const char*, int, int*)
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(netcdf, nc_close, int, int)
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(netcdf, nc_create, int, const char*, int, int*)
ICEDB_DLL_INTERFACE_END
*/
ICEDB_DLL_INTERFACE_IMPLEMENTATION_BEGIN(netcdf);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION_B(netcdf, nc_open, int, const char*, int, int*)
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(netcdf, nc_close, "nc_close", int, int)
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(netcdf, nc_create, "nc_create", int, const char*, int, int*)

ICEDB_DLL_INTERFACE_IMPLEMENTATION_CONSTRUCTOR(netcdf);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(netcdf, nc_open, int, const char*, int, int*)
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(netcdf, nc_close, int, int)
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(netcdf, nc_create, int, const char*, int, int*)

ICEDB_DLL_INTERFACE_IMPLEMENTATION_END(netcdf);


#endif
