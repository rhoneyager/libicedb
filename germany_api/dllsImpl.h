#pragma once
#ifndef ICEDB_H_DLLS_IMPL
#define ICEDB_H_DLLS_IMPL
#include "defs.h"
#include "error.h"
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

struct ICEDB_DLL_BASE_HANDLE;
typedef dlHandleType(*ICEDB_DLL_open_dll_f)(ICEDB_DLL_BASE_HANDLE*);
typedef void(*ICEDB_DLL_close_dll_f)(ICEDB_DLL_BASE_HANDLE*);
typedef bool(*ICEDB_DLL_isOpen_f)(ICEDB_DLL_BASE_HANDLE*);
typedef uint16_t(*ICEDB_DLL_getRefCount_f)(ICEDB_DLL_BASE_HANDLE*);
typedef void(*ICEDB_DLL_incRefCount_f)(ICEDB_DLL_BASE_HANDLE*);
typedef void(*ICEDB_DLL_decRefCount_f)(ICEDB_DLL_BASE_HANDLE*);
typedef void*(*ICEDB_DLL_getSym_f)(ICEDB_DLL_BASE_HANDLE*, const char* symbol_name);
typedef const char*(*ICEDB_DLL_getPath_f)(ICEDB_DLL_BASE_HANDLE*);
typedef void(*ICEDB_DLL_setPath_f)(ICEDB_DLL_BASE_HANDLE*, const char* filename);
typedef void(*ICEDB_DLL_ctor_f)(ICEDB_DLL_BASE_HANDLE*);

struct ICEDB_DLL_BASE_HANDLE_vtable {
	ICEDB_DLL_open_dll_f open;
	ICEDB_DLL_close_dll_f close;
	ICEDB_DLL_isOpen_f isOpen;
	ICEDB_DLL_getSym_f getSym;
	ICEDB_DLL_getRefCount_f getRefCount;
	ICEDB_DLL_incRefCount_f incRefCount;
	ICEDB_DLL_decRefCount_f decRefCount;
	ICEDB_DLL_getPath_f getPath;
	ICEDB_DLL_setPath_f setPath;
};
ICEDB_CALL_C DL_ICEDB ICEDB_DLL_BASE_HANDLE_vtable* ICEDB_DLL_BASE_create_vtable();
ICEDB_CALL_C DL_ICEDB void ICEDB_DLL_BASE_destroy_vtable(ICEDB_DLL_BASE_HANDLE_vtable*);
ICEDB_CALL_C DL_ICEDB ICEDB_DLL_BASE_HANDLE* ICEDB_DLL_BASE_HANDLE_create(const char* filename);
ICEDB_CALL_C DL_ICEDB void ICEDB_DLL_BASE_HANDLE_destroy(ICEDB_DLL_BASE_HANDLE*);

struct ICEDB_DLL_BASE_HANDLE {
	dlHandleType dlHandle;
	uint16_t refCount;
	const char* path;
	ICEDB_DLL_BASE_HANDLE_vtable *_vtable;
};

enum ICEDB_DLL_FUNCTION_STATUSES {
	ICEDB_DLL_FUNCTION_UNLOADED, // Must always be zero
	ICEDB_DLL_FUNCTION_LOADED
};

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

ICEDB_DLL_INTERFACE_BEGIN(testdll)
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(testdll, int, testNum, int)
ICEDB_DLL_INTERFACE_END

// This should only be in the implementation file, which is in C++.
template <class InterfaceClass>
void destroy_interface(InterfaceClass* p) {
	delete p->_p->p;
	ICEDB_free(p->_p);
	p->_base->_vtable->decRefCount(p->_base);
	ICEDB_free(p);
}

#define ICEDB_DLL_INTERFACE_IMPLEMENTATION_BEGIN(InterfaceName) \
	ICEDB_CALL_C DL_ICEDB void destroy_##InterfaceName(interface_##InterfaceName* p) { destroy_interface<interface_##InterfaceName>(p); }
#define ICEDB_DLL_INTERFACE_IMPLEMENTATION_END(InterfaceName)
#define ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(InterfaceName, retVal, FuncName, ...)

ICEDB_END_DECL
#endif
