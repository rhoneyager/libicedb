#pragma once
#ifndef ICEDB_H_PLUGINS
#define ICEDB_H_PLUGINS
#include "dlls.h"
#include "versioningForwards.h"
#include "versioning.hpp"
ICEDB_DLL_INTERFACE_BEGIN(icedb_plugin_base)
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(icedb_plugin_base, GetVerInfo, ICEDB_VersionInfo_p)
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(icedb_plugin_base, Register, void)
ICEDB_DLL_INTERFACE_END

#define ICEDB_DLL_PLUGINS_COMMON(pluginName) \
SHARED_EXPORT_ICEDB ICEDB_VersionInfo_p GetVerInfo() { \
	static auto res = icedb::versioning::genVersionInfo(); \
	return res; \
}
#endif
