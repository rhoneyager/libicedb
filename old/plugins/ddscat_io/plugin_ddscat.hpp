#pragma once
#include "../../libicedb/icedb/dlls/plugins.h"
#include "../../libicedb/icedb/dlls/dllsImpl.hpp"
#include "../../libicedb/icedb/misc/mem.h"
#include "../../libicedb/icedb/misc/memInterface.h"
#include "../../libicedb/icedb/misc/util.h"
#include "../../libicedb/icedb/misc/utilInterface.h"
#include "../../libicedb/icedb/error/error_contextInterface.hpp"
#include "../../libicedb/icedb/error/errorInterface.hpp"
#include "../../libicedb/icedb/fs/fs_backend.hpp"

struct ICEDB_handle_inner {
	ICEDB_handle_inner();
	uint64_t magic;
	std::string cwd;
	std::map<std::string, std::string> props;
};
namespace icedb {
	namespace plugins {
		namespace ddscat_io {
			extern const uint64_t pluginMagic;
			// The name of the plugin matters when handles are opened. They act as reserved types.
			extern const char* pluginName;
			extern ICEDB_fs_plugin_capabilities caps;
			extern ICEDB_DLL_BASE_HANDLE *hnd, *hndSelf;
			extern std::map<std::string, std::string> libprops;
			extern std::shared_ptr<interface_ICEDB_core_util> i_util;
			extern std::shared_ptr<interface_ICEDB_core_mem> i_mem;
			extern std::shared_ptr<interface_ICEDB_core_error> i_error;
			extern std::shared_ptr<interface_ICEDB_core_error_context> i_error_context;
			extern std::shared_ptr<interface_ICEDB_core_fs> i_fs_core;

		}
	}
}
