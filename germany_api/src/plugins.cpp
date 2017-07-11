#include "../germany/defs.h"
#include "../germany/dlls/plugins.h"
#include "../germany/dlls/dllsImpl.hpp"

ICEDB_DLL_INTERFACE_IMPLEMENTATION_BEGIN(icedb_plugin_base);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(icedb_plugin_base, GetVerInfo, "GetVerInfo", ICEDB_VersionInfo_p);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(icedb_plugin_base, Register, "Register", bool, ICEDB_register_interface_f, ICEDB_get_module_f);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(icedb_plugin_base, Unregister, "Unregister", void, ICEDB_register_interface_f, ICEDB_get_module_f);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(icedb_plugin_base, GetName, "GetName", const char*);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_CONSTRUCTOR(icedb_plugin_base);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(icedb_plugin_base, GetVerInfo, ICEDB_VersionInfo_p);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(icedb_plugin_base, Register, bool, ICEDB_register_interface_f, ICEDB_get_module_f);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(icedb_plugin_base, Unregister, void, ICEDB_register_interface_f, ICEDB_get_module_f);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(icedb_plugin_base, GetName, const char*);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_END(icedb_plugin_base);
