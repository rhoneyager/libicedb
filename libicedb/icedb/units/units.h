#pragma once
#ifndef ICEDB_H_UNITS
#define ICEDB_H_UNITS
#include "../defs.h"
ICEDB_BEGIN_DECL_C

struct ICEDB_unit_converter;
//typedef ICEDB_UNIT_CONVERTER* ICEDB_UNIT_CONVERTER_p;
DL_ICEDB ICEDB_unit_converter* ICEDB_create_unit_converter(const char* type, const char* inUnits, const char* outUnits);
DL_ICEDB void ICEDB_destroy_unit_converter(ICEDB_unit_converter*);
DL_ICEDB double ICEDB_unit_convert(const ICEDB_unit_converter*, double in);

ICEDB_END_DECL_C
#endif
