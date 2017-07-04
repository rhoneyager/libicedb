#pragma once
#ifndef ICEDB_H_OS_FUNCTIONS
#define ICEDB_H_OS_FUNCTIONS
#include "defs.h"
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
DL_ICEDB void ICEDB_enumModules(int pid, ICEDB_enumModulesRes*);

DL_ICEDB void ICEDB_libEntry(int argc, char** argv);
DL_ICEDB void ICEDB_libExit();

ICEDB_END_DECL_C

#endif
