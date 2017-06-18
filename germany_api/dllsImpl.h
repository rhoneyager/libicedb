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

#define ICEDB_DLL_INTERFACE_BEGIN(InterfaceName) \
	struct interface_##InterfaceName; \
	ICEDB_CALL_C DL_ICEDB interface_##InterfaceName* create_##InterfaceName(ICEDB_DLL_BASE_HANDLE *); \
	ICEDB_CALL_C DL_ICEDB void destroy_##InterfaceName(interface_##InterfaceName*); \
    HIDDEN_ICEDB struct _impl_interface_##InterfaceName; \
	struct interface_##InterfaceName { \
		ICEDB_DLL_BASE_HANDLE *_base; \
		_impl_interface_##InterfaceName *_p; \
// m_funcname is the actual function
// f_funcname is the implementation / handler, that will load the function if necessary.
// f_funcname gets an extra pointer to the interface struct (interface_##InterfaceName),
//  which has the base dll handle and the variables of interest (m_is... etc.)
// The variable args are the list of parameters passed to the function (type names only).
#define ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(InterfaceName, retVal, FuncName, ...) \
	short int status_m_##FuncName; \
	typedef retVal (* TYPE_##FuncName)(__VA_ARGS__); \
	typedef retVal (* F_TYPE_##FuncName)(interface_##InterfaceName *, __VA_ARGS__); \
	TYPE_##FuncName m_##FuncName; \
	F_TYPE_##FuncName FuncName; \
	;
#define ICEDB_DLL_INTERFACE_END \
	};

ICEDB_END_DECL
#endif
