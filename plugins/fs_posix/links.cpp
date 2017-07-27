#include "../../libicedb/icedb/fs/fs_dll_impl.hpp"
#include "../../libicedb/icedb/defs.h"
#include "../../libicedb/icedb/error/error.h"
#include "../../libicedb/icedb/misc/os_functions.h"
#include "../../libicedb/icedb/dlls/plugins.h"
#include "../../libicedb/icedb/misc/util.h"
#include "fs_win.hpp"

extern "C" {

	using namespace icedb::plugins::fs_win;
	SHARED_EXPORT_ICEDB ICEDB_error_code fs_move(ICEDB_handle_inner* p, const char* src, const char* dest) {
		if (!isValidHandle(p))
			hnd->_vtable->_raiseExcept(hnd,
				__FILE__, (int)__LINE__, ICEDB_DEBUG_FSIG);
		// TODO: Export the error subsystem to dlls.
		return ICEDB_ERRORCODES_NONE;
	}

}

/*
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(ICEDB_fs_plugin,
	move, "fs_move", ICEDB_error_code, ICEDB_FS_HANDLE_p, const char*, const char*);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(ICEDB_fs_plugin,
	copy, "fs_copy", ICEDB_error_code, ICEDB_FS_HANDLE_p, const char*, const char*);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(ICEDB_fs_plugin,
	unlink, "fs_unlink", ICEDB_error_code, ICEDB_FS_HANDLE_p, const char*);

ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(ICEDB_fs_plugin,
	create_hard_link, "fs_create_hard_link", ICEDB_error_code, ICEDB_FS_HANDLE_p, const char*, const char*);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(ICEDB_fs_plugin,
	create_sym_link, "fs_create_sym_link", ICEDB_error_code, ICEDB_FS_HANDLE_p, const char*, const char*);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(ICEDB_fs_plugin,
	follow_sym_link, "fs_follow_sym_link", ICEDB_error_code,
	ICEDB_FS_HANDLE_p, const char*, size_t, size_t*, char**);
*/
