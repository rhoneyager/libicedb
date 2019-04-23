#pragma once
#ifndef ICEDB_H_OS_FUNCTIONS
#define ICEDB_H_OS_FUNCTIONS
#include "../defs.h"
//#include "../dlls/linking.h"
//#include <stdio.h>

ICEDB_BEGIN_DECL_C

ICEDB_DL bool ICEDB_pidExists(int pid, bool &res);
ICEDB_DL int ICEDB_getPID();
ICEDB_DL int ICEDB_getPPID(int pid);

ICEDB_DL bool ICEDB_waitOnExitGetDefault();
ICEDB_DL void ICEDB_waitOnExitSet(bool val);
ICEDB_DL bool ICEDB_waitOnExitGet();

ICEDB_DL const char* ICEDB_getUserName();
ICEDB_DL const char* ICEDB_getHostName();
ICEDB_DL const char* ICEDB_getAppConfigDir();
ICEDB_DL const char* ICEDB_getHomeDir();

struct ICEDB_enumModulesRes {
	size_t sz;
	const char** modules;
};
ICEDB_DL void ICEDB_free_enumModulesRes(ICEDB_enumModulesRes*);
ICEDB_DL ICEDB_enumModulesRes* ICEDB_enumModules(int pid);
ICEDB_DL char* ICEDB_findModuleByFunc(void* ptr, size_t sz, char* res);
ICEDB_DL char* ICEDB_getLibDir(size_t sz, char* res);
ICEDB_DL char* ICEDB_getAppDir(size_t sz, char* res);
ICEDB_DL char* ICEDB_getPluginDir(size_t sz, char* res);
ICEDB_DL char* ICEDB_getLibPath(size_t sz, char* res);
ICEDB_DL char* ICEDB_getAppPath(size_t sz, char* res);
ICEDB_DL char* ICEDB_getCWD(size_t sz, char* res);
ICEDB_DL char* ICEDB_getShareDir(size_t sz, char* res);

ICEDB_DL const char* ICEDB_getLibDirC();
ICEDB_DL const char* ICEDB_getAppDirC();
ICEDB_DL const char* ICEDB_getPluginDirC();
ICEDB_DL const char* ICEDB_getLibPathC();
ICEDB_DL const char* ICEDB_getAppPathC();
ICEDB_DL const char* ICEDB_getCWDC();
ICEDB_DL const char* ICEDB_getShareDirC();

ICEDB_DL void ICEDB_libEntry(int argc, char** argv);
ICEDB_DL void ICEDB_libExit();

ICEDB_DL void ICEDB_writeDebugString(const char*);

ICEDB_END_DECL_C


#endif
