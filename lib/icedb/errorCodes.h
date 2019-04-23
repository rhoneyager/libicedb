#pragma once
#ifndef ICEDB_H_ERRORCODES
#define ICEDB_H_ERRORCODES

#include "defs.h"
#include <wchar.h>
ICEDB_BEGIN_DECL

	enum ICEDB_ERRORCODES {
		ICEDB_ERRORCODES_NONE, ///< There is no error
		ICEDB_ERRORCODES_TODO, ///< Placeholder error value
		ICEDB_ERRORCODES_DLLOPEN, ///< Error when opening a dll. Generic - see system error.
		ICEDB_ERRORCODES_NO_DLHANDLE, ///< The DLL is not open
		ICEDB_ERRORCODES_NO_DLSYMBOL, ///< Cannot open the symbol
		ICEDB_ERRORCODES_DLL_BASE_REFS_EXIST, ///< Attempting to destroy a dll handle, but references are still held.
		ICEDB_ERRORCODES_DLL_DEC_REFS_LE_0, ///< Trying to decrement dll reference count below zero
		ICEDB_ERRORCODES_OS, ///< Error returned by an os subsystem (general)
		ICEDB_ERRORCODES_UNIMPLEMENTED, ///< Unimplemented function path
		ICEDB_ERRORCODES_BAD_PLUGIN, ///< Either not a plugin or an incompatible version
		ICEDB_ERRORCODES_NONEXISTENT_PATH, ///< The path does not exist
		ICEDB_ERRORCODES_READONLY, ///< The path is read only. No modifications allowed.
		ICEDB_ERRORCODES_NULLPTR, ///< Null pointer exception
		ICEDB_ERRORCODES_TOTAL ///< Not really an error, but used for counting
	};

	extern ICEDB_PRIVATE const char* ICEDB_ERRORCODES_MAP[ICEDB_ERRORCODES_TOTAL];

ICEDB_END_DECL

#endif