#pragma once

#ifndef ICEDB_H_DLLS_INTERFACE_IMPL
#define ICEDB_H_DLLS_INTERFACE_IMPL
/// Include only once per module!!!! Implements the interface.
#include "../defs.h"
#include "../misc/mem.h"
#include "dlls.h"
//#include "dlls.hpp"
#include "dllsInterface.hpp"
#include "dllsImpl.hpp"

//ICEDB_BEGIN_DECL

ICEDB_DLL_INTERFACE_IMPLEMENTATION_BEGIN(dlls)
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(dlls, DLL_BASE_HANDLE_create, "ICEDB_DLL_BASE_HANDLE_create", ICEDB_DLL_BASE_HANDLE*, const char*)
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(dlls, DLL_BASE_HANDLE_create_from_lib, "ICEDB_DLL_BASE_HANDLE_create_from_lib", ICEDB_DLL_BASE_HANDLE*)
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(dlls, DLL_BASE_HANDLE_destroy, "ICEDB_DLL_BASE_HANDLE_destroy", void, ICEDB_DLL_BASE_HANDLE*)

ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(dlls, register_interface, "ICEDB_register_interface", void, const char*, int, const char*)
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(dlls, unregister_interface, "ICEDB_unregister_interface", void, const char*, int, const char*)
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(dlls, query_interface, "ICEDB_query_interface", ICEDB_query_interface_res_t, const char*)
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(dlls, query_interface_free, "ICEDB_query_interface_free", void, ICEDB_query_interface_res_t)

ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(dlls, dll_name_mangle_simple, "ICEDB_dll_name_mangle_simple", size_t, const char*, char*, size_t)
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(dlls, load_plugin, "ICEDB_load_plugin", bool, const char*)
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(dlls, unload_plugin, "ICEDB_unload_plugin", bool, const char*)

ICEDB_DLL_INTERFACE_IMPLEMENTATION_CONSTRUCTOR(dlls);

ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(dlls, DLL_BASE_HANDLE_create, ICEDB_DLL_BASE_HANDLE*, const char*)
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(dlls, DLL_BASE_HANDLE_create_from_lib, ICEDB_DLL_BASE_HANDLE*)
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(dlls, DLL_BASE_HANDLE_destroy, void, ICEDB_DLL_BASE_HANDLE*)

ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(dlls, register_interface, void, const char*, int, const char*)
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(dlls, unregister_interface, void, const char*, int, const char*)
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(dlls, query_interface, ICEDB_query_interface_res_t, const char*)
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(dlls, query_interface_free, void, ICEDB_query_interface_res_t)

ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(dlls, dll_name_mangle_simple, size_t, const char*, char*, size_t)
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(dlls, load_plugin, bool, const char*)
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(dlls, unload_plugin, bool, const char*)
ICEDB_DLL_INTERFACE_IMPLEMENTATION_END(dlls)

//ICEDB_END_DECL
#endif
