#pragma once
#ifndef ICEDB_H_UTILINTERFACE
#define ICEDB_H_UTILINTERFACE
#include "../defs.h"
#include "../dlls/linking.h"
#include "util.h"
#include <string.h>

#include "utilInterface.h"

ICEDB_DLL_INTERFACE_IMPLEMENTATION_BEGIN(ICEDB_core_util);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(ICEDB_core_util, strncpy_s, "ICEDB_COMPAT_strncpy_s", size_t, char *, size_t, const char*, size_t);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(ICEDB_core_util, strdup_s, "ICEDB_COMPAT_strdup_s", char*, const char *, size_t);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(ICEDB_core_util, wcsncpy_s, "ICEDB_COMPAT_wcsncpy_s", size_t, wchar_t *, size_t, const wchar_t*, size_t);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(ICEDB_core_util, wcsdup_s, "ICEDB_COMPAT_wcsdup_s", wchar_t*, const wchar_t *, size_t);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_CONSTRUCTOR(ICEDB_core_util);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_core_util, strncpy_s, size_t, char *, size_t, const char*, size_t);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_core_util, strdup_s, char*, const char *, size_t);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_core_util, wcsncpy_s, size_t, wchar_t *, size_t, const wchar_t*, size_t);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_core_util, wcsdup_s, wchar_t*, const wchar_t *, size_t);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_END(ICEDB_core_util);



#endif
