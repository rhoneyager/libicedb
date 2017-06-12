#pragma once
#ifndef ICEDB_H_ERRORCODES
#define ICEDB_H_ERRORCODES

#include "defs.h"

ICEDB_BEGIN_DECL

	enum ICEDB_ERRORCODES {
		ICEDB_ERRORCODES_NONE, // There is no error
		ICEDB_ERRORCODES_TODO, // Placeholder error value
		ICEDB_ERRORCODES_TOTAL // Not really an error, but used for counting
	};

	extern ICEDB_SYMBOL_PRIVATE const char* ICEDB_ERRORCODES_MAP[ICEDB_ERRORCODES_TOTAL];

ICEDB_END_DECL

#endif