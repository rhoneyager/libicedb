#pragma once
#ifndef ICEDB_H_UNITSINTERFACE
#define ICEDB_H_UNITSINTERFACE
#include "../defs.h"
#include "../dlls/dlls.hpp"
ICEDB_BEGIN_DECL

extern "C" struct ICEDB_unit_converter_s;
typedef double(*ICEDB_unit_Converter_f)(ICEDB_unit_converter_s*, double);
extern "C" struct ICEDB_unit_converter_s {
	ICEDB_unit_Converter_f convert;
	void* _p;
};
typedef ICEDB_unit_converter_s* ICEDB_unit_converter_p;

ICEDB_DLL_INTERFACE_BEGIN(units)
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(units, canConvert, bool, const char*, const char*, const char*)
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(units, makeConverter, ICEDB_unit_converter_s*, const char*, const char*, const char*)
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(units, freeConverter, void, ICEDB_unit_converter_s*)
ICEDB_DLL_INTERFACE_END

namespace icedb {
	namespace units {
		namespace iface {
			ICEDB_DLL_CPP_INTERFACE_BEGIN(unitscpp, units)
				ICEDB_DLL_CPP_INTERFACE_DECLARE_FUNCTION(unitscpp, canConvert, bool, const char*, const char*, const char*)
				ICEDB_DLL_CPP_INTERFACE_DECLARE_FUNCTION(unitscpp, makeConverter, ICEDB_unit_converter_s*, const char*, const char*, const char*)
				ICEDB_DLL_CPP_INTERFACE_DECLARE_FUNCTION(unitscpp, freeConverter, void, ICEDB_unit_converter_s*)
				ICEDB_DLL_CPP_INTERFACE_END
		}
	}
}
ICEDB_END_DECL
#endif
