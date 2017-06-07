#pragma once
#ifndef ICEDB_H_MEM
#define ICEDB_H_MEM
#include "defs.h"
#ifdef __cplusplus
extern "C" {
#endif

	/** Allocate memory in bytes. Generally this is just malloced, but a custom allocator may be substituted. **/
	ICEDB_SYMBOL_SHARED void* ICEDB_malloc(size_t numBytes);

	/** Free memory region. Should not be double-freed. **/
	ICEDB_SYMBOL_SHARED void ICEDB_free(void* obj);


#ifdef __cplusplus
}
#endif
#endif
