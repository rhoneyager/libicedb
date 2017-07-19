#pragma once
#ifndef ICEDB_PLUGINS_H_FS_NETCDF
#define ICEDB_PLUGINS_H_FS_NETCDF
#include <cstdint>
#include <string>
#include "../../libicedb/icedb/dlls/dllsInterface.hpp"
// This struct is deliberately redefined in each plugin.
// The first field, however, is always a magic uint64_t, that indicates that the
// passed handle is appropriate for this plugin.
struct ICEDB_handle_inner {
	ICEDB_handle_inner();
	uint64_t magic;
	std::string cwd;
};

namespace icedb {
	namespace plugins {
		namespace fs_netcdf {
			extern const uint64_t pluginMagic;
			// The name of the plugin matters when handles are opened. They act as reserved types.
			extern const char* pluginName;
			extern ICEDB_fs_plugin_capabilities caps;
			extern ICEDB_DLL_BASE_HANDLE* hnd;
			extern std::shared_ptr<ICEDB_DLL_BASE_HANDLE> hndNetCDF;
			extern std::shared_ptr<interface_dlls> i_dlls;
		}
	}
}

extern "C" bool isValidHandle(ICEDB_handle_inner* p);
#endif