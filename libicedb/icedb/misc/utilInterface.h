#pragma once
#ifndef ICEDB_H_UTILINTERFACE
#define ICEDB_H_UTILINTERFACE
#include "../defs.h"
#include "../dlls/linking.h"
#include "util.h"
#include <string.h>

//DL_ICEDB size_t ICEDB_COMPAT_wcsncpy_s(wchar_t *dest, size_t destSz, const wchar_t* src, size_t srcSz);
//DL_ICEDB wchar_t* ICEDB_COMPAT_wcsdup_s(const wchar_t* src, size_t srcSz);

ICEDB_DLL_INTERFACE_BEGIN(ICEDB_core_util)
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(ICEDB_core_util, strncpy_s, size_t, char *, size_t, const char*, size_t)
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(ICEDB_core_util, strdup_s, char*, const char *, size_t)
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(ICEDB_core_util, wcsncpy_s, size_t, wchar_t *, size_t, const wchar_t*, size_t)
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(ICEDB_core_util, wcsdup_s, wchar_t*, const wchar_t *, size_t)
ICEDB_DLL_INTERFACE_END


#endif
