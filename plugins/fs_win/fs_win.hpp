#pragma once
#ifndef ICEDB_PLUGINS_H_FS_WIN
#define ICEDB_PLUGINS_H_FS_WIN
#include <cstdint>
#include <string>
#include <map>
#include "../../libicedb/icedb/fs/fs.h"
#include "../../libicedb/icedb/fs/fs_backend.hpp"
#include <Windows.h>
// This struct is deliberately redefined in each plugin.
// The first field, however, is always a magic uint64_t, that indicates that the
// passed handle is appropriate for this plugin.
struct ICEDB_handle_inner {
	ICEDB_handle_inner();
	uint64_t magic;
	std::string cwd;
	std::map<std::string, std::string> props;
};

namespace icedb {
	namespace plugins {
		namespace fs_win {
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
			void GenerateWinOSerror(DWORD winerr = 0);
		}
	}
}

extern "C" {
	bool isValidHandle(ICEDB_FS_HANDLE_p p);
	bool isValidHandleInner(ICEDB_handle_inner* p);
	ICEDB_FS_HANDLE_p makeHandle();
	SHARED_EXPORT_ICEDB void fs_get_capabilities(ICEDB_fs_plugin_capabilities* p);
	SHARED_EXPORT_ICEDB void fs_destroy(ICEDB_FS_HANDLE_p p);
	SHARED_EXPORT_ICEDB void fs_set_property(
		ICEDB_FS_HANDLE_p p, const char* key, const char* val);
	SHARED_EXPORT_ICEDB void fs_set_global_property(const char* key, const char* val);
	SHARED_EXPORT_ICEDB size_t fs_get_property(ICEDB_FS_HANDLE_p p,
		const char* key, size_t mxsz, char** val, size_t *sz);
	SHARED_EXPORT_ICEDB size_t fs_get_global_property(
		const char* key, size_t mxsz, char** val, size_t *sz);
	SHARED_EXPORT_ICEDB bool Register(
		ICEDB_register_interface_f fReg, ICEDB_get_module_f fMod, 
		ICEDB_DLL_BASE_HANDLE* hDll, ICEDB_DLL_BASE_HANDLE* hSelf);
	SHARED_EXPORT_ICEDB void Unregister(
		ICEDB_register_interface_f fUnReg, ICEDB_get_module_f fMod);

	SHARED_EXPORT_ICEDB bool fs_path_exists(ICEDB_FS_HANDLE_p, const char* path);
	SHARED_EXPORT_ICEDB ICEDB_error_code fs_path_info(
		ICEDB_FS_HANDLE_p p, const char* path, ICEDB_FS_PATH_CONTENTS* data);
	SHARED_EXPORT_ICEDB size_t fs_can_open_path(
		ICEDB_FS_HANDLE_p, const char* p, const char* t, ICEDB_file_open_flags flags);
	SHARED_EXPORT_ICEDB	ICEDB_file_open_flags fs_get_open_flags(ICEDB_FS_HANDLE_p p);
	SHARED_EXPORT_ICEDB ICEDB_FS_HANDLE_p fs_open_path(
		ICEDB_FS_HANDLE_p p, const char* path, const char* typ, ICEDB_file_open_flags flags);

	SHARED_EXPORT_ICEDB ICEDB_error_code fs_copy(ICEDB_FS_HANDLE_p p,
		const char* from, const char* to, bool overwrite);
	SHARED_EXPORT_ICEDB ICEDB_error_code fs_move(ICEDB_FS_HANDLE_p p,
		const char* from, const char* to, bool overwrite);
	SHARED_EXPORT_ICEDB ICEDB_error_code fs_unlink(ICEDB_FS_HANDLE_p p, const char* path);

	SHARED_EXPORT_ICEDB ICEDB_error_code fs_readobjs(ICEDB_FS_HANDLE_p p,
		const char* from, ICEDB_FS_PATH_CONTENTS*** res);
	SHARED_EXPORT_ICEDB ICEDB_error_code fs_free_objs(ICEDB_FS_HANDLE_p p, ICEDB_FS_PATH_CONTENTS** pc);
}

#endif