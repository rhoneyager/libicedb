#pragma once
#ifndef ICEDB_H_PLUGINS
#define ICEDB_H_PLUGINS
#include "../defs.h"
#include "dlls.h"
#include "../versioning/versioningForwards.h"
#include "../versioning/versioning.hpp"

typedef void(*ICEDB_register_interface_f)(const char*, int, const char*);
typedef char*(*ICEDB_get_module_f)(void*, size_t, char*);

ICEDB_DLL_INTERFACE_BEGIN(icedb_plugin_base)
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(icedb_plugin_base, GetVerInfo, ICEDB_VersionInfo_p)
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(icedb_plugin_base, Register, bool, ICEDB_register_interface_f, ICEDB_get_module_f, ICEDB_DLL_BASE_HANDLE*)
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(icedb_plugin_base, Unregister, void, ICEDB_register_interface_f, ICEDB_get_module_f)
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(icedb_plugin_base, GetName, const char*)
ICEDB_DLL_INTERFACE_END

#define ICEDB_DLL_PLUGINS_COMMON(pluginName) \
extern "C" SHARED_EXPORT_ICEDB ICEDB_VersionInfo_p GetVerInfo() { \
	static auto v = icedb::versioning::genVersionInfo(); \
	static ICEDB_VersionInfo_p res(new ICEDB_VersionInfo); \
	res->p = v; \
	return res; \
} \
extern "C" SHARED_EXPORT_ICEDB const char* GetName() { return #pluginName; }
#endif
