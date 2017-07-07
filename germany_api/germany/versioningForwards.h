#pragma once
#ifndef ICEDB_H_VERSIONING_FORWARDS
#define ICEDB_H_VERSIONING_FORWARDS
#include "defs.h"
ICEDB_BEGIN_DECL_C

enum ICEDB_ver_match {
	ICEDB_VER_INCOMPATIBLE, ICEDB_VER_COMPATIBLE_1, 
	ICEDB_VER_COMPATIBLE_2, ICEDB_VER_COMPATIBLE_3, 
	ICEDB_VER_EXACT_MATCH
};

struct ICEDB_VersionInfo;
typedef ICEDB_VersionInfo* ICEDB_VersionInfo_p;

DL_ICEDB void ICEDB_VersionInfo_Free(ICEDB_VersionInfo_p);
DL_ICEDB ICEDB_VersionInfo_p ICEDB_getLibVersionInfo();
DL_ICEDB char* ICEDB_WriteLibVersionInfoC(ICEDB_VersionInfo_p, char*, size_t sz);
DL_ICEDB const char* ICEDB_WriteLibVersionInfoCC(ICEDB_VersionInfo_p);
DL_ICEDB ICEDB_ver_match ICEDB_CompareVersions(const ICEDB_VersionInfo_p a, const ICEDB_VersionInfo_p b);

ICEDB_END_DECL_C
#endif
