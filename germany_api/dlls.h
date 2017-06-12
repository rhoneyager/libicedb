#pragma once
#ifndef ICEDB_H_DLLS
#define ICEDB_H_DLLS
#include "defs.h"
#include "error.h"
ICEDB_BEGIN_DECL

/// Handle for the dll object.
/// The pointer is opaque. Stores information regarding mapped functions, whether the dll is loaded or
/// unloaded, the path, et cetera.
struct ICEDB_DLL_HANDLE;
typedef struct ICEDB_DLL_HANDLE ICEDB_DLL_HANDLE;

/// Prep a dll for loading
ICEDB_SYMBOL_SHARED ICEDB_error_code ICEDB_dll_createHandle(ICEDB_DLL_HANDLE* res);

/// Load the dll right now
ICEDB_SYMBOL_SHARED ICEDB_error_code ICEDB_dll_load(ICEDB_DLL_HANDLE* res);


ICEDB_END_DECL
#endif
