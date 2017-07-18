#pragma once
#ifndef ICEDB_PLUGINS_H_FS_WIN
#define ICEDB_PLUGINS_H_FS_WIN
#include <cstdint>
#include <string>
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
		namespace fs_win {
			extern const uint64_t pluginMagic;
			// The name of the plugin matters when handles are opened. They act as reserved types.
			extern const char* pluginName;
		}
	}
}

#endif