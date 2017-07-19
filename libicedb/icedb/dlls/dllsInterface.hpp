#pragma once
#ifndef ICEDB_H_DLLS_INTERFACE
#define ICEDB_H_DLLS_INTERFACE
#include "../defs.h"
#include "dlls.h"
#include "linking.h"

ICEDB_BEGIN_DECL
ICEDB_DLL_INTERFACE_BEGIN(dlls)
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(dlls, DLL_BASE_HANDLE_create, ICEDB_DLL_BASE_HANDLE*, const char*)
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(dlls, DLL_BASE_HANDLE_create_from_lib, ICEDB_DLL_BASE_HANDLE*)
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(dlls, DLL_BASE_HANDLE_destroy, void, ICEDB_DLL_BASE_HANDLE*)

ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(dlls, register_interface, void, const char*, int, const char*)
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(dlls, unregister_interface, void, const char*, int, const char*)
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(dlls, query_interface, ICEDB_query_interface_res_t, const char*)
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(dlls, query_interface_free, void, ICEDB_query_interface_res_t)

ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(dlls, dll_name_mangle_simple, size_t, const char*, char*, size_t)
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(dlls, load_plugin, bool, const char*)
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(dlls, unload_plugin, bool, const char*)
ICEDB_DLL_INTERFACE_END

ICEDB_END_DECL
#endif
