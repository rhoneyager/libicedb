#pragma once
#ifndef ICEDB_H_DLLS_IMPL
#define ICEDB_H_DLLS_IMPL
#include "defs.h"
#include "error.h"
#include "dlls.h"
#include <stdarg.h>

#ifdef _WIN32
#include "windows.h"
#endif
#ifdef __unix__
#include <dlfcn.h>
#endif

#ifdef _WIN32
typedef HINSTANCE dlHandleType;
#endif
#ifdef __APPLE__
typedef void* dlHandleType;
#endif
#ifdef __unix__
typedef void* dlHandleType;
#endif


ICEDB_BEGIN_DECL

struct _dlHandleType_impl {
	dlHandleType h;
};

ICEDB_CALL_C DL_ICEDB ICEDB_DLL_BASE_HANDLE_vtable* ICEDB_DLL_BASE_create_vtable();
ICEDB_CALL_C DL_ICEDB void ICEDB_DLL_BASE_destroy_vtable(ICEDB_DLL_BASE_HANDLE_vtable*);



ICEDB_END_DECL
#endif
