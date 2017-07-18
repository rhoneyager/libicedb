#pragma once
#ifndef ICEDB_H_FS_BACKEND
#define ICEDB_H_FS_BACKEND
#include "../defs.h"
#include "fs.h"
#include "../dlls/linking.h"
#include <memory>
#include <cstdint>

namespace icedb {
	namespace fs {
		struct handle_inner; // Used internally for plugins.
		typedef std::shared_ptr<handle_inner> hnd_t;
	}
}

ICEDB_BEGIN_DECL_C
struct ICEDB_fs_plugin_capabilities {
	bool has_folders;
	bool can_move, can_delete, can_copy, can_soft_link, can_hard_link;
	bool has_external_links;
	bool fs_has_cyclic_links;
	bool has_xattrs;
};

struct interface_ICEDB_fs_plugin;

struct ICEDB_FS_HANDLE {
	uint64_t magic;
	icedb::fs::hnd_t h;
	std::shared_ptr<interface_ICEDB_fs_plugin> i;
	ICEDB_fs_plugin_capabilities c;
};


ICEDB_DLL_INTERFACE_BEGIN(ICEDB_fs_plugin)
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(ICEDB_fs_plugin,
	can_open_path, bool, const char*, const char*, ICEDB_file_open_flags);
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(ICEDB_fs_plugin,
	open_path, ICEDB_FS_HANDLE_p, const char*, const char*, ICEDB_file_open_flags);
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(ICEDB_fs_plugin,
	can_open_stream, bool, const char*, const char*, ICEDB_file_open_flags);
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(ICEDB_fs_plugin,
	open_stream, ICEDB_FS_HANDLE_p, const char*, const char*, ICEDB_file_open_flags);

ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(ICEDB_fs_plugin,
	get_open_flags, ICEDB_file_open_flags, ICEDB_FS_HANDLE_p);
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(ICEDB_fs_plugin,
	destroy, void, ICEDB_FS_HANDLE_p);

ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(ICEDB_fs_plugin,
	move, ICEDB_error_code, ICEDB_FS_HANDLE_p, const char*, const char*);
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(ICEDB_fs_plugin,
	copy, ICEDB_error_code, ICEDB_FS_HANDLE_p, const char*, const char*);
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(ICEDB_fs_plugin,
	unlink, ICEDB_error_code, ICEDB_FS_HANDLE_p, const char*);
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(ICEDB_fs_plugin,
	create_hard_link, ICEDB_error_code, ICEDB_FS_HANDLE_p, const char*, const char*);
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(ICEDB_fs_plugin,
	create_sym_link, ICEDB_error_code, ICEDB_FS_HANDLE_p, const char*, const char*);
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(ICEDB_fs_plugin,
	follow_sym_link, ICEDB_error_code, ICEDB_FS_HANDLE_p, 
	const char*, size_t, size_t*, char**);

ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(ICEDB_fs_plugin,
	path_exists, bool, ICEDB_FS_HANDLE_p, const char*);
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(ICEDB_fs_plugin,
	path_info, ICEDB_error_code, ICEDB_FS_HANDLE_p, const char*, ICEDB_FS_PATH_CONTENTS**);
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(ICEDB_fs_plugin,
	readobjs, ICEDB_error_code, ICEDB_FS_HANDLE_p, ICEDB_FS_PATH_CONTENTS**);
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(ICEDB_fs_plugin,
	rewind, ICEDB_error_code, ICEDB_FS_HANDLE_p);

ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(ICEDB_fs_plugin,
	readobjattrs, ICEDB_error_code, ICEDB_FS_HANDLE_p, ICEDB_FS_ATTR_CONTENTS**);
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(ICEDB_fs_plugin,
	attr_rewind, ICEDB_error_code, ICEDB_FS_HANDLE_p);
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(ICEDB_fs_plugin,
	attr_remove, ICEDB_error_code, ICEDB_FS_HANDLE_p, const char*);
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(ICEDB_fs_plugin,
	attr_insert, ICEDB_error_code, ICEDB_FS_HANDLE_p, const char*, const char*, size_t, ICEDB_attr_types);

ICEDB_DLL_INTERFACE_END

ICEDB_END_DECL_C
#endif