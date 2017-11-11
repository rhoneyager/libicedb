#pragma once
#ifndef ICEDB_H_FS_IMPL
#define ICEDB_H_FS_IMPL
#include "../defs.h"
#include "fs_backend.hpp"
#include "../dlls/dllsImpl.hpp"

// ONLY INCLUDE THIS FILE ONCE PER MODULE!
// It implements the dll interface described in fs_backend.hpp.
// These files must always reflect each other.

ICEDB_DLL_INTERFACE_IMPLEMENTATION_BEGIN(ICEDB_core_fs)

ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION_B(ICEDB_core_fs,
	ICEDB_file_handle_create, ICEDB_FS_HANDLE_p, const char*, const char*, ICEDB_file_open_flags);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION_B(ICEDB_core_fs,
	ICEDB_file_handle_create_with_registry, ICEDB_FS_HANDLE_p, const char*, const char*, ICEDB_file_open_flags, const char*);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION_B(ICEDB_core_fs,
	ICEDB_file_handle_get_name, const char*, ICEDB_FS_HANDLE_p);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION_B(ICEDB_core_fs,
	ICEDB_file_handle_destroy, void, ICEDB_FS_HANDLE_p);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION_B(ICEDB_core_fs,
	ICEDB_file_handle_move, ICEDB_error_code, ICEDB_FS_HANDLE_p, const char*, const char*);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION_B(ICEDB_core_fs,
	ICEDB_file_handle_copy, ICEDB_error_code, ICEDB_FS_HANDLE_p, const char*, const char*, bool);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION_B(ICEDB_core_fs,
	ICEDB_file_handle_unlink, ICEDB_error_code, ICEDB_FS_HANDLE_p, const char*);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION_B(ICEDB_core_fs,
	ICEDB_file_handle_create_hard_link, ICEDB_error_code, ICEDB_FS_HANDLE_p, const char*, const char*);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION_B(ICEDB_core_fs,
	ICEDB_file_handle_create_sym_link, ICEDB_error_code, ICEDB_FS_HANDLE_p, const char*, const char*);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION_B(ICEDB_core_fs,
	ICEDB_file_handle_follow_sym_link, ICEDB_error_code, ICEDB_FS_HANDLE_p, const char*, size_t, size_t*, char**);

ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION_B(ICEDB_core_fs,
	ICEDB_FS_PATH_CONTENTS_alloc, bool, ICEDB_FS_PATH_CONTENTS*);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION_B(ICEDB_core_fs,
	ICEDB_FS_PATH_CONTENTS_free, bool, ICEDB_FS_PATH_CONTENTS*);

ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION_B(ICEDB_core_fs,
	ICEDB_file_handle_path_exists, bool, ICEDB_FS_HANDLE_p, const char*);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION_B(ICEDB_core_fs,
	ICEDB_file_handle_path_info, ICEDB_error_code, ICEDB_FS_HANDLE_p, const char*, ICEDB_FS_PATH_CONTENTS *);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION_B(ICEDB_core_fs,
	ICEDB_file_handle_readobjs, ICEDB_error_code, ICEDB_FS_HANDLE_p, const char*, size_t *, ICEDB_FS_PATH_CONTENTS ***);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION_B(ICEDB_core_fs,
	ICEDB_file_handle_free_objs, ICEDB_error_code, ICEDB_FS_HANDLE_p, ICEDB_FS_PATH_CONTENTS **);


ICEDB_DLL_INTERFACE_IMPLEMENTATION_CONSTRUCTOR(ICEDB_core_fs);


ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_core_fs,
	ICEDB_file_handle_create, ICEDB_FS_HANDLE_p, const char*, const char*, ICEDB_file_open_flags);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_core_fs,
	ICEDB_file_handle_create_with_registry, ICEDB_FS_HANDLE_p, const char*, const char*, ICEDB_file_open_flags, const char*);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_core_fs,
	ICEDB_file_handle_get_name, const char*, ICEDB_FS_HANDLE_p);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_core_fs,
	ICEDB_file_handle_destroy, void, ICEDB_FS_HANDLE_p);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_core_fs,
	ICEDB_file_handle_move, ICEDB_error_code, ICEDB_FS_HANDLE_p, const char*, const char*);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_core_fs,
	ICEDB_file_handle_copy, ICEDB_error_code, ICEDB_FS_HANDLE_p, const char*, const char*, bool);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_core_fs,
	ICEDB_file_handle_unlink, ICEDB_error_code, ICEDB_FS_HANDLE_p, const char*);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_core_fs,
	ICEDB_file_handle_create_hard_link, ICEDB_error_code, ICEDB_FS_HANDLE_p, const char*, const char*);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_core_fs,
	ICEDB_file_handle_create_sym_link, ICEDB_error_code, ICEDB_FS_HANDLE_p, const char*, const char*);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_core_fs,
	ICEDB_file_handle_follow_sym_link, ICEDB_error_code, ICEDB_FS_HANDLE_p, const char*, size_t, size_t*, char**);

ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_core_fs,
	ICEDB_FS_PATH_CONTENTS_alloc, bool, ICEDB_FS_PATH_CONTENTS*);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_core_fs,
	ICEDB_FS_PATH_CONTENTS_free, bool, ICEDB_FS_PATH_CONTENTS*);

ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_core_fs,
	ICEDB_file_handle_path_exists, bool, ICEDB_FS_HANDLE_p, const char*);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_core_fs,
	ICEDB_file_handle_path_info, ICEDB_error_code, ICEDB_FS_HANDLE_p, const char*, ICEDB_FS_PATH_CONTENTS *);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_core_fs,
	ICEDB_file_handle_readobjs, ICEDB_error_code, ICEDB_FS_HANDLE_p, const char*, size_t *, ICEDB_FS_PATH_CONTENTS ***);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_core_fs,
	ICEDB_file_handle_free_objs, ICEDB_error_code, ICEDB_FS_HANDLE_p, ICEDB_FS_PATH_CONTENTS **);


ICEDB_DLL_INTERFACE_IMPLEMENTATION_END(ICEDB_core_fs)







ICEDB_DLL_INTERFACE_IMPLEMENTATION_BEGIN(ICEDB_fs_plugin)

ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(ICEDB_fs_plugin,
	get_capabilities, "fs_get_capabilities", void, ICEDB_fs_plugin_capabilities*);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(ICEDB_fs_plugin,
	set_property, "fs_set_property", void, ICEDB_FS_HANDLE_p, const char*, const char*);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(ICEDB_fs_plugin,
	get_property, "fs_get_property", size_t, ICEDB_FS_HANDLE_p, const char*, size_t, char**, size_t*);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(ICEDB_fs_plugin,
	set_global_property, "fs_set_global_property", void, const char*, const char*);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(ICEDB_fs_plugin,
	get_global_property, "fs_get_global_property", size_t, const char*, size_t, char**, size_t*);

ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(ICEDB_fs_plugin,
	can_open_path, "fs_can_open_path", size_t, ICEDB_FS_HANDLE_p, const char*, const char*, ICEDB_file_open_flags);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(ICEDB_fs_plugin,
	open_path, "fs_open_path", ICEDB_FS_HANDLE_p, ICEDB_FS_HANDLE_p, const char*, const char*, ICEDB_file_open_flags);
//ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(ICEDB_fs_plugin,
//	can_open_stream, "fs_can_open_stream", bool, const char*, const char*, ICEDB_file_open_flags);
//ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(ICEDB_fs_plugin,
//	open_stream, "fs_open_stream", ICEDB_handle_inner*, const char*, const char*, ICEDB_file_open_flags);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(ICEDB_fs_plugin,
	get_open_flags, "fs_get_open_flags", ICEDB_file_open_flags, ICEDB_FS_HANDLE_p);

ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(ICEDB_fs_plugin,
	destroy, "fs_destroy", void, ICEDB_FS_HANDLE_p);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(ICEDB_fs_plugin,
	move, "fs_move", ICEDB_error_code, ICEDB_FS_HANDLE_p, const char*, const char*);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(ICEDB_fs_plugin,
	copy, "fs_copy", ICEDB_error_code, ICEDB_FS_HANDLE_p, const char*, const char*, bool);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(ICEDB_fs_plugin,
	unlink, "fs_unlink", ICEDB_error_code, ICEDB_FS_HANDLE_p, const char*);

ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(ICEDB_fs_plugin,
	create_hard_link, "fs_create_hard_link", ICEDB_error_code, ICEDB_FS_HANDLE_p, const char*, const char*);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(ICEDB_fs_plugin,
	create_sym_link, "fs_create_sym_link", ICEDB_error_code, ICEDB_FS_HANDLE_p, const char*, const char*);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(ICEDB_fs_plugin,
	follow_sym_link, "fs_follow_sym_link", ICEDB_error_code, 
	ICEDB_FS_HANDLE_p, const char*, size_t, size_t*, char**);

ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(ICEDB_fs_plugin,
	path_exists, "fs_path_exists", bool, ICEDB_FS_HANDLE_p, const char*);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(ICEDB_fs_plugin,
	path_info, "fs_path_info", ICEDB_error_code, ICEDB_FS_HANDLE_p, const char*, ICEDB_FS_PATH_CONTENTS*);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(ICEDB_fs_plugin,
	readobjs, "fs_readobjs", ICEDB_error_code, ICEDB_FS_HANDLE_p, const char*, size_t*, ICEDB_FS_PATH_CONTENTS***);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(ICEDB_fs_plugin,
	free_objs, "fs_free_objs", ICEDB_error_code, ICEDB_FS_HANDLE_p, ICEDB_FS_PATH_CONTENTS**);

ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(ICEDB_fs_plugin,
	readobjattrs, "fs_readobjattrs", ICEDB_error_code, ICEDB_FS_HANDLE_p, ICEDB_FS_attr_CONTENTS**);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(ICEDB_fs_plugin,
	free_objattrs, "fs_free_objattrs", ICEDB_error_code, ICEDB_FS_HANDLE_p, ICEDB_FS_attr_CONTENTS**);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(ICEDB_fs_plugin,
	attr_remove, "fs_attr_remove", ICEDB_error_code, ICEDB_FS_HANDLE_p, const char*);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(ICEDB_fs_plugin,
	attr_insert, "fs_attr_insert", ICEDB_error_code, ICEDB_FS_HANDLE_p, const char*, const char* data, size_t, ICEDB_attr_types);

ICEDB_DLL_INTERFACE_IMPLEMENTATION_CONSTRUCTOR(ICEDB_fs_plugin)

ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_fs_plugin,
	get_capabilities, void, ICEDB_fs_plugin_capabilities*);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_fs_plugin,
	set_property, void, ICEDB_FS_HANDLE_p, const char*, const char*);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_fs_plugin,
	get_property, size_t, ICEDB_FS_HANDLE_p, const char*, size_t, char**, size_t*);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_fs_plugin,
	set_global_property, void, const char*, const char*);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_fs_plugin,
	get_global_property, size_t, const char*, size_t, char**, size_t*);

ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_fs_plugin,
	can_open_path, size_t, ICEDB_FS_HANDLE_p, const char*, const char*, ICEDB_file_open_flags);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_fs_plugin,
	open_path, ICEDB_FS_HANDLE_p, ICEDB_FS_HANDLE_p, const char*, const char*, ICEDB_file_open_flags);
//ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_fs_plugin,
//	can_open_stream, bool, const char*, const char*, ICEDB_file_open_flags);
//ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_fs_plugin,
//	open_stream, ICEDB_handle_inner*, const char*, const char*, ICEDB_file_open_flags);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_fs_plugin,
	get_open_flags, ICEDB_file_open_flags, ICEDB_FS_HANDLE_p);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_fs_plugin,
	destroy, void, ICEDB_FS_HANDLE_p);

ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_fs_plugin,
	move, ICEDB_error_code, ICEDB_FS_HANDLE_p, const char*, const char*);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_fs_plugin,
	copy, ICEDB_error_code, ICEDB_FS_HANDLE_p, const char*, const char*, bool);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_fs_plugin,
	unlink, ICEDB_error_code, ICEDB_FS_HANDLE_p, const char*);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_fs_plugin,
	create_hard_link, ICEDB_error_code, ICEDB_FS_HANDLE_p, const char*, const char*);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_fs_plugin,
	create_sym_link, ICEDB_error_code, ICEDB_FS_HANDLE_p, const char*, const char*);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_fs_plugin,
	follow_sym_link, ICEDB_error_code, ICEDB_FS_HANDLE_p,
	const char*, size_t, size_t*, char**);

ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_fs_plugin,
	path_exists, bool, ICEDB_FS_HANDLE_p, const char*);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_fs_plugin,
	path_info, ICEDB_error_code, ICEDB_FS_HANDLE_p, const char*, ICEDB_FS_PATH_CONTENTS*);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_fs_plugin,
	readobjs, ICEDB_error_code, ICEDB_FS_HANDLE_p, const char*, size_t*, ICEDB_FS_PATH_CONTENTS***);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_fs_plugin,
	free_objs, ICEDB_error_code, ICEDB_FS_HANDLE_p, ICEDB_FS_PATH_CONTENTS**);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_fs_plugin,
	readobjattrs, ICEDB_error_code, ICEDB_FS_HANDLE_p, ICEDB_FS_attr_CONTENTS**);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_fs_plugin,
	free_objattrs, ICEDB_error_code, ICEDB_FS_HANDLE_p, ICEDB_FS_attr_CONTENTS**);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_fs_plugin,
	attr_remove, ICEDB_error_code, ICEDB_FS_HANDLE_p, const char*);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_fs_plugin,
	attr_insert, ICEDB_error_code, ICEDB_FS_HANDLE_p, const char*, const char*, size_t, ICEDB_attr_types);

ICEDB_DLL_INTERFACE_IMPLEMENTATION_END(ICEDB_fs_plugin);


#endif