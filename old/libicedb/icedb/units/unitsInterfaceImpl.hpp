#pragma once
/// Include only once per module!!!! Implements the interface.
#include "units.hpp"
#include "../misc/mem.h"
#include "../dlls/dlls.h"
#include "../dlls/dlls.hpp"
#include "../units/unitsInterface.hpp"
#include "../dlls/dllsImpl.hpp"
#include <string.h>

ICEDB_DLL_INTERFACE_IMPLEMENTATION_BEGIN(units);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(units, canConvert, "canConvert", bool, const char*, const char*, const char*);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(units, makeConverter, "makeConverter", ICEDB_unit_converter_s*, const char*, const char*, const char*);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(units, freeConverter, "freeConverter", void, ICEDB_unit_converter_s*);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_CONSTRUCTOR(units);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(units, canConvert, bool, const char*, const char*, const char*);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(units, makeConverter, ICEDB_unit_converter_s*, const char*, const char*, const char*);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(units, freeConverter, void, ICEDB_unit_converter_s*);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_END(units);


namespace icedb {
	namespace units {
		namespace iface {
			ICEDB_DLL_CPP_INTERFACE_IMPLEMENTATION_BEGIN(unitscpp, units)
				ICEDB_DLL_CPP_INTERFACE_IMPLEMENTATION_FUNCTION(units, canConvert, bool, const char*, const char*, const char*)
				ICEDB_DLL_CPP_INTERFACE_IMPLEMENTATION_FUNCTION(units, makeConverter, ICEDB_unit_converter_s*, const char*, const char*, const char*)
				ICEDB_DLL_CPP_INTERFACE_IMPLEMENTATION_FUNCTION(units, freeConverter, void, ICEDB_unit_converter_s*)
				ICEDB_DLL_CPP_INTERFACE_IMPLEMENTATION_END
		}
	}
}
