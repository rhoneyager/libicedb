#pragma once
#ifndef ICEDB_H_MEM
#define ICEDB_H_MEM
#include "../defs.h"
#include "../dlls/linking.h"
ICEDB_BEGIN_DECL_C

	/** Allocate memory in bytes. Generally this is just malloced, but a custom allocator may be substituted. **/
	ICEDB_SYMBOL_SHARED void* ICEDB_malloc(size_t numBytes);

	/** Free memory region. Should not be double-freed. **/
	ICEDB_SYMBOL_SHARED void ICEDB_free(void* obj);


ICEDB_END_DECL_C

ICEDB_DLL_INTERFACE_BEGIN(ICEDB_core_mem)
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(ICEDB_core_mem, malloc, void*, size_t)
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(ICEDB_core_mem, free, void, void*)
ICEDB_DLL_INTERFACE_END

#define ICEDB_core_mem_impl \
	ICEDB_DLL_INTERFACE_IMPLEMENTATION_BEGIN(ICEDB_core_mem); \
	ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(ICEDB_core_mem, malloc, "ICEDB_malloc", void*, size_t); \
	ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(ICEDB_core_mem, free, "ICEDB_free", void, void*); \
	ICEDB_DLL_INTERFACE_IMPLEMENTATION_CONSTRUCTOR(ICEDB_core_mem); \
	ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_core_mem, malloc, void*, size_t); \
	ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_core_mem, free, void, void*); \
	ICEDB_DLL_INTERFACE_IMPLEMENTATION_END(ICEDB_core_mem);



#endif
