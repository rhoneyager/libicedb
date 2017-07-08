#pragma once
#ifndef ICEDB_H_UNITSINTERFACE
#define ICEDB_H_UNITSINTERFACE
#include "defs.h"
#include "dlls.hpp"
ICEDB_BEGIN_DECL

ICEDB_DLL_INTERFACE_BEGIN(units)
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(units, canConvert, bool, const char*, const char*, const char*)
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(units, convert, double, double)
ICEDB_DLL_INTERFACE_END

namespace icedb {
	namespace units {
		namespace iface {
			ICEDB_DLL_CPP_INTERFACE_BEGIN(unitscpp, units)
				ICEDB_DLL_CPP_INTERFACE_DECLARE_FUNCTION(unitscpp, canConvert, bool, const char*, const char*, const char*)
				ICEDB_DLL_CPP_INTERFACE_DECLARE_FUNCTION(unitscpp, convert, double, double)
				ICEDB_DLL_CPP_INTERFACE_END
		}
	}
}
ICEDB_END_DECL
#endif
