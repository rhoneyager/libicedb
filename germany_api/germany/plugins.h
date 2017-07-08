#pragma once
#ifndef ICEDB_H_PLUGINS
#define ICEDB_H_PLUGINS
#include "defs.h"
#include "dlls.h"
#include "versioningForwards.h"
#include "versioning.hpp"
ICEDB_DLL_INTERFACE_BEGIN(icedb_plugin_base)
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(icedb_plugin_base, GetVerInfo, ICEDB_VersionInfo_p)
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(icedb_plugin_base, Register, void)
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(icedb_plugin_base, Unregister, void)
ICEDB_DLL_INTERFACE_END

#define ICEDB_DLL_PLUGINS_COMMON(pluginName) \
extern "C" SHARED_EXPORT_ICEDB ICEDB_VersionInfo_p GetVerInfo() { \
	static auto v = icedb::versioning::genVersionInfo(); \
	static ICEDB_VersionInfo_p res(new ICEDB_VersionInfo); \
	res->p = v; \
	return res; \
}
#endif
