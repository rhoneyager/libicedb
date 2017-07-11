#pragma once
#ifndef ICEDB_H_DLLS_LINKING
#define ICEDB_H_DLLS_LINKING
#include "../defs.h"
ICEDB_BEGIN_DECL

typedef uint16_t ICEDB_DLL_FUNCTION_STATUSES;

struct ICEDB_DLL_BASE_HANDLE;

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
