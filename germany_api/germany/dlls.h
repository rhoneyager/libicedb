#pragma once
#ifndef ICEDB_H_DLLS
#define ICEDB_H_DLLS
#include "defs.h"
#include "error.h"
#include "mem.h"
ICEDB_BEGIN_DECL

typedef uint16_t ICEDB_DLL_FUNCTION_STATUSES;

struct _dlHandleType_impl;


struct ICEDB_DLL_BASE_HANDLE;
typedef ICEDB_error_code(*ICEDB_DLL_open_dll_f)(ICEDB_DLL_BASE_HANDLE*);
typedef ICEDB_error_code(*ICEDB_DLL_close_dll_f)(ICEDB_DLL_BASE_HANDLE*);
typedef uint16_t(*ICEDB_DLL_isOpen_f)(ICEDB_DLL_BASE_HANDLE*);
typedef void(*ICEDB_DLL_set_autoopen_f)(ICEDB_DLL_BASE_HANDLE*,bool);
typedef bool(*ICEDB_DLL_get_autoopen_f)(ICEDB_DLL_BASE_HANDLE*);
typedef uint16_t(*ICEDB_DLL_getRefCount_f)(ICEDB_DLL_BASE_HANDLE*);
typedef void(*ICEDB_DLL_incRefCount_f)(ICEDB_DLL_BASE_HANDLE*);
typedef ICEDB_error_code(*ICEDB_DLL_decRefCount_f)(ICEDB_DLL_BASE_HANDLE*);
typedef void*(*ICEDB_DLL_getSym_f)(ICEDB_DLL_BASE_HANDLE*, const char* symbol_name);
typedef const char*(*ICEDB_DLL_getPath_f)(ICEDB_DLL_BASE_HANDLE*);
typedef ICEDB_error_code(*ICEDB_DLL_setPath_f)(ICEDB_DLL_BASE_HANDLE*, const char* filename);

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
	ICEDB_DLL_set_autoopen_f setAutoOpen;
	ICEDB_DLL_get_autoopen_f getAutoOpen;
};

ICEDB_CALL_C DL_ICEDB ICEDB_DLL_BASE_HANDLE* ICEDB_DLL_BASE_HANDLE_create(const char* filename);
ICEDB_CALL_C DL_ICEDB void ICEDB_DLL_BASE_HANDLE_destroy(ICEDB_DLL_BASE_HANDLE*);
struct ICEDB_DLL_BASE_HANDLE {
	_dlHandleType_impl *_dlHandle;
	uint16_t refCount;
	bool autoOpen;
	uint16_t openCount;
	const char* path;
	ICEDB_DLL_BASE_HANDLE_vtable *_vtable;
};

// DLL Registry functions

ICEDB_CALL_C DL_ICEDB void ICEDB_register_interface(const char* topic, int priority, const char* path);
ICEDB_CALL_C DL_ICEDB void ICEDB_unregister_interface(const char* topic, int priority, const char* path);
typedef char** ICEDB_query_interface_res_t;
ICEDB_CALL_C DL_ICEDB ICEDB_query_interface_res_t ICEDB_query_interface(const char* topic);
ICEDB_CALL_C DL_ICEDB void ICEDB_query_interface_free(ICEDB_query_interface_res_t);


ICEDB_CALL_C DL_ICEDB bool ICEDB_load_plugin(const char* path);
ICEDB_CALL_C DL_ICEDB bool ICEDB_unload_plugin(const char* path);


#define ICEDB_DLL_INTERFACE_BEGIN(InterfaceName) \
	struct interface_##InterfaceName; \
	ICEDB_CALL_C interface_##InterfaceName* create_##InterfaceName(ICEDB_DLL_BASE_HANDLE *); \
	ICEDB_CALL_C void destroy_##InterfaceName(interface_##InterfaceName*); \
     struct HIDDEN_ICEDB _impl_interface_##InterfaceName; \
	struct interface_##InterfaceName { \
		ICEDB_DLL_BASE_HANDLE *_base; \
		_impl_interface_##InterfaceName *_p;
#define ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(InterfaceName, FuncName, retVal, ...) \
	typedef retVal (* F_TYPE_##FuncName)(interface_##InterfaceName *, ##__VA_ARGS__); \
	F_TYPE_##FuncName FuncName; \
	typedef bool (* MKBIND_TYPE_##FuncName)(interface_##InterfaceName *); \
	MKBIND_TYPE_##FuncName Bind_##FuncName;
#define ICEDB_DLL_INTERFACE_END \
	};

ICEDB_END_DECL
#endif
