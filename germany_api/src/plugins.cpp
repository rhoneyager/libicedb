#include "../germany/defs.h"
#include "../germany/plugins.h"
#include "../germany/dllsImpl.hpp"

ICEDB_DLL_INTERFACE_IMPLEMENTATION_BEGIN(icedb_plugin_base);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(icedb_plugin_base, GetVerInfo, "GetVerInfo", ICEDB_VersionInfo_p);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(icedb_plugin_base, Register, "Register", void);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_CONSTRUCTOR(icedb_plugin_base);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(icedb_plugin_base, GetVerInfo, ICEDB_VersionInfo_p);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(icedb_plugin_base, Register, void);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_END(icedb_plugin_base);
