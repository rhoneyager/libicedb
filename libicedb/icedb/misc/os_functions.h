#pragma once
#ifndef ICEDB_H_OS_FUNCTIONS
#define ICEDB_H_OS_FUNCTIONS
#include "../defs.h"
#include "../dlls/linking.h"
#include <stdio.h>

ICEDB_BEGIN_DECL_C

DL_ICEDB bool ICEDB_pidExists(int pid, bool &res);
DL_ICEDB int ICEDB_getPID();
DL_ICEDB int ICEDB_getPPID(int pid);

DL_ICEDB bool ICEDB_waitOnExitGetDefault();
DL_ICEDB void ICEDB_waitOnExitSet(bool val);
DL_ICEDB bool ICEDB_waitOnExitGet();

DL_ICEDB const char* ICEDB_getUserName();
DL_ICEDB const char* ICEDB_getHostName();
DL_ICEDB const char* ICEDB_getAppConfigDir();
DL_ICEDB const char* ICEDB_getHomeDir();

struct ICEDB_enumModulesRes {
	size_t sz;
	const char** modules;
};
DL_ICEDB void ICEDB_free_enumModulesRes(ICEDB_enumModulesRes*);
DL_ICEDB ICEDB_enumModulesRes* ICEDB_enumModules(int pid);
DL_ICEDB char* ICEDB_findModuleByFunc(void* ptr, size_t sz, char* res);
DL_ICEDB char* ICEDB_getLibDir(size_t sz, char* res);
DL_ICEDB char* ICEDB_getAppDir(size_t sz, char* res);
DL_ICEDB char* ICEDB_getPluginDir(size_t sz, char* res);
DL_ICEDB char* ICEDB_getLibPath(size_t sz, char* res);
DL_ICEDB char* ICEDB_getAppPath(size_t sz, char* res);
DL_ICEDB char* ICEDB_getCWD(size_t sz, char* res);

DL_ICEDB const char* ICEDB_getLibDirC();
DL_ICEDB const char* ICEDB_getAppDirC();
DL_ICEDB const char* ICEDB_getPluginDirC();
DL_ICEDB const char* ICEDB_getLibPathC();
DL_ICEDB const char* ICEDB_getAppPathC();
DL_ICEDB const char* ICEDB_getCWDC();

DL_ICEDB void ICEDB_libEntry(int argc, char** argv);
DL_ICEDB void ICEDB_libExit();

// Filesystem functions
// Does path exist?
// Resolve symlink
// Get type of path
// List contents of folder (recursion optional)
// Create hard link
// Copy file
// Remove file
// Move file

ICEDB_END_DECL_C

/*
ICEDB_DLL_INTERFACE_BEGIN(ICEDB_core_os_functions)
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(ICEDB_core_os_functions, strncpy_s, size_t, char *, size_t, const char*, size_t)
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(ICEDB_core_os_functions, strdup_s, char*, const char *, size_t)
ICEDB_DLL_INTERFACE_END

#define ICEDB_core_util_impl \
	ICEDB_DLL_INTERFACE_IMPLEMENTATION_BEGIN(ICEDB_core_os_functions); \
	ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(ICEDB_core_os_functions, strncpy_s, "ICEDB_COMPAT_strncpy_s", size_t, char *, size_t, const char*, size_t); \
	ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(ICEDB_core_os_functions, strdup_s, "ICEDB_COMPAT_strdup_s", size_t, char *, size_t); \
	ICEDB_DLL_INTERFACE_IMPLEMENTATION_CONSTRUCTOR(ICEDB_core_os_functions); \
	ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_core_os_functions, strncpy_s, size_t, char *, size_t, const char*, size_t); \
	ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_core_os_functions, strdup_s, char*, const char *, size_t); \
	ICEDB_DLL_INTERFACE_IMPLEMENTATION_END(ICEDB_core_os_functions);
*/


#endif
