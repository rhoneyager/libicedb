#pragma once
#ifndef ICEDB_H_ERRORCODES
#define ICEDB_H_ERRORCODES

#include "defs.h"

#ifdef __cplusplus
extern "C" {
#endif

	enum ICEDB_ERRORCODES {
		ICEDB_ERRORCODES_NONE, // There is no error
		ICEDB_ERRORCODES_TODO, // Placeholder error value
		ICEDB_ERRORCODES_TOTAL // Not really an error, but used for counting
	};

	extern ICEDB_SYMBOL_PRIVATE const char* ICEDB_ERRORCODES_MAP[ICEDB_ERRORCODES_TOTAL];

#ifdef __cplusplus
}
#endif

#endif