#pragma once
#ifndef ICEDB_H_UNITS
#define ICEDB_H_UNITS
#include "defs.h"
ICEDB_BEGIN_DECL_C

struct ICEDB_UNIT_CONVERTER;
typedef ICEDB_UNIT_CONVERTER* ICEDB_UNIT_CONVERTER_p;
DL_ICEDB ICEDB_UNIT_CONVERTER_p ICEDB_create_unit_converter(const char* type, const char* inUnits, const char* outUnits);
DL_ICEDB void ICEDB_destroy_unit_converter(ICEDB_UNIT_CONVERTER_p);
DL_ICEDB double ICEDB_unit_convert(ICEDB_UNIT_CONVERTER_p, double in);

ICEDB_END_DECL_C
#endif
