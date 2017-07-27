#pragma once
#ifndef ICEDB_H_MEMINTERFACE
#define ICEDB_H_MEMINTERFACE
#include "../defs.h"
#include "../dlls/linking.h"
#include "mem.h"

ICEDB_DLL_INTERFACE_BEGIN(ICEDB_core_mem)
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(ICEDB_core_mem, malloc, void*, size_t)
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(ICEDB_core_mem, free, void, void*)
ICEDB_DLL_INTERFACE_END


#endif
