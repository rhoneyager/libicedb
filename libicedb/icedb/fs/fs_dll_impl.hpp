#pragma once
#ifndef ICEDB_H_FS_IMPL
#define ICEDB_H_FS_IMPL
#include "../defs.h"
#include "fs_backend.hpp"
#include "../dlls/dllsImpl.hpp"

// ONLY INCLUDE THIS FILE ONCE PER MODULE!
// It implements the dll interface described in fs_backend.hpp.
// These files must always reflect each other.

ICEDB_DLL_INTERFACE_IMPLEMENTATION_BEGIN(ICEDB_fs_plugin)
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(ICEDB_fs_plugin,
	can_open_path, "fs_can_open_path", bool, const char*, const char*, ICEDB_file_open_flags);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(ICEDB_fs_plugin,
	open_path, "fs_open_path", ICEDB_FS_HANDLE_p, const char*, const char*, ICEDB_file_open_flags);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(ICEDB_fs_plugin,
	can_open_stream, "fs_can_open_stream", bool, const char*, const char*, ICEDB_file_open_flags);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(ICEDB_fs_plugin,
	open_stream, "fs_open_stream", ICEDB_FS_HANDLE_p, const char*, const char*, ICEDB_file_open_flags);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(ICEDB_fs_plugin,
	get_open_flags, "fs_get_open_flags", ICEDB_file_open_flags, ICEDB_FS_HANDLE_p);

ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(ICEDB_fs_plugin,
	destroy, "fs_destroy", void, ICEDB_FS_HANDLE_p);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(ICEDB_fs_plugin,
	move, "fs_move", void, ICEDB_FS_HANDLE_p, const char*, const char*);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(ICEDB_fs_plugin,
	copy, "fs_copy", void, ICEDB_FS_HANDLE_p, const char*, const char*);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(ICEDB_fs_plugin,
	unlink, "fs_unlink", void, ICEDB_FS_HANDLE_p, const char*);

ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(ICEDB_fs_plugin,
	create_hard_link, "fs_create_hard_link", bool, ICEDB_FS_HANDLE_p, const char*, const char*);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(ICEDB_fs_plugin,
	create_sym_link, "fs_create_sym_link", bool, ICEDB_FS_HANDLE_p, const char*, const char*);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(ICEDB_fs_plugin,
	follow_sym_link, "fs_follow_sym_link", size_t, ICEDB_FS_HANDLE_p, const char*, size_t, char**);

ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(ICEDB_fs_plugin,
	path_exists, "fs_path_exists", bool, ICEDB_FS_HANDLE_p, const char*);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(ICEDB_fs_plugin,
	path_info, "fs_path_info", ICEDB_error_code, ICEDB_FS_HANDLE_p, const char*, ICEDB_FS_PATH_CONTENTS**);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(ICEDB_fs_plugin,
	readobjs, "fs_readobjs", ICEDB_error_code, ICEDB_FS_HANDLE_p, ICEDB_FS_PATH_CONTENTS**);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(ICEDB_fs_plugin,
	rewind, "fs_rewind", void, ICEDB_FS_HANDLE_p);

ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(ICEDB_fs_plugin,
	readobjattrs, "fs_readobjattrs", ICEDB_error_code, ICEDB_FS_HANDLE_p, ICEDB_FS_ATTR_CONTENTS**);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(ICEDB_fs_plugin,
	attr_rewind, "fs_attr_rewind", void, ICEDB_FS_HANDLE_p);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(ICEDB_fs_plugin,
	attr_remove, "fs_attr_remove", void, ICEDB_FS_HANDLE_p, const char*);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(ICEDB_fs_plugin,
	attr_insert, "fs_attr_insert", void, ICEDB_FS_HANDLE_p, const char*, const char* data, size_t, ICEDB_attr_types);

ICEDB_DLL_INTERFACE_IMPLEMENTATION_CONSTRUCTOR(ICEDB_fs_plugin)

ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_fs_plugin,
	can_open_path, bool, const char*, const char*, ICEDB_file_open_flags);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_fs_plugin,
	open_path, ICEDB_FS_HANDLE_p, const char*, const char*, ICEDB_file_open_flags);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_fs_plugin,
	can_open_stream, bool, const char*, const char*, ICEDB_file_open_flags);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_fs_plugin,
	open_stream, ICEDB_FS_HANDLE_p, const char*, const char*, ICEDB_file_open_flags);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_fs_plugin,
	get_open_flags, ICEDB_file_open_flags, ICEDB_FS_HANDLE_p);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_fs_plugin,
	destroy, void, ICEDB_FS_HANDLE_p);

ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_fs_plugin,
	move, void, ICEDB_FS_HANDLE_p, const char*, const char*);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_fs_plugin,
	copy, void, ICEDB_FS_HANDLE_p, const char*, const char*);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_fs_plugin,
	unlink, void, ICEDB_FS_HANDLE_p, const char*);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_fs_plugin,
	create_hard_link, bool, ICEDB_FS_HANDLE_p, const char*, const char*);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_fs_plugin,
	create_sym_link, bool, ICEDB_FS_HANDLE_p, const char*, const char*);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_fs_plugin,
	follow_sym_link, size_t, ICEDB_FS_HANDLE_p, const char*, size_t, char**);

ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_fs_plugin,
	path_exists, bool, ICEDB_FS_HANDLE_p, const char*);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_fs_plugin,
	path_info, ICEDB_error_code, ICEDB_FS_HANDLE_p, const char*, ICEDB_FS_PATH_CONTENTS**);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_fs_plugin,
	readobjs, ICEDB_error_code, ICEDB_FS_HANDLE_p, ICEDB_FS_PATH_CONTENTS**);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_fs_plugin,
	rewind, void, ICEDB_FS_HANDLE_p);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_fs_plugin,
	readobjattrs, ICEDB_error_code, ICEDB_FS_HANDLE_p, ICEDB_FS_ATTR_CONTENTS**);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_fs_plugin,
	attr_rewind, void, ICEDB_FS_HANDLE_p);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_fs_plugin,
	attr_remove, void, ICEDB_FS_HANDLE_p, const char*);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_fs_plugin,
	attr_insert, void, ICEDB_FS_HANDLE_p, const char*, const char*, size_t, ICEDB_attr_types);

ICEDB_DLL_INTERFACE_IMPLEMENTATION_END(ICEDB_fs_plugin)


#endif
