#include "../../libicedb/icedb/defs.h"
#include "../../libicedb/icedb/fs/fs.h"
#include "../../libicedb/icedb/misc/os_functions.h"
#include "../../libicedb/icedb/dlls/plugins.h"
#include "../../libicedb/icedb/misc/util.h"
#include <string>
#include "fs_win.hpp"
#include <windows.h>
#include "Shlwapi.h"

using namespace icedb::plugins::fs_win;
extern "C" {
	std::wstring makeEffPath(ICEDB_FS_HANDLE_p p, const wchar_t* path) {
		std::wstring effpath;
		if (p) {
			if (!isValidHandle(p))
				hnd->_vtable->_raiseExcept(hnd,
					__FILE__, (int)__LINE__, ICEDB_DEBUG_FSIG);
			effpath = p->h->cwd;
			effpath.append(L"\\");
		}
		if (path)
			effpath.append(path);
		if (!effpath.length())
			hnd->_vtable->_raiseExcept(hnd,
				__FILE__, (int)__LINE__, ICEDB_DEBUG_FSIG);
		return effpath;
	}
	SHARED_EXPORT_ICEDB bool fs_path_exists(ICEDB_FS_HANDLE_p p, const wchar_t* path) {
		// The base filesystem plugins do not require the path handle. If it is not specified, then
		// just assube that the base directory is specified in path. If it is specified, then 
		// prepend p's base directory to path.
		std::wstring effpath = makeEffPath(p, path);
		bool res = PathFileExistsW(effpath.data());
		return res;
	}

	SHARED_EXPORT_ICEDB ICEDB_error_code fs_path_info(ICEDB_FS_HANDLE_p p, const wchar_t* path, ICEDB_FS_PATH_CONTENTS* data) {
		std::wstring effpath = makeEffPath(p, path);
		if (!data) hnd->_vtable->_raiseExcept(hnd,
			__FILE__, (int)__LINE__, ICEDB_DEBUG_FSIG);
		data->base_handle = p;
		data->base_path = p->h->cwd;
		data->idx = -1;
		data->p_name = effpath;
		data->p_type;
		data->p_obj_type;
	}

	SHARED_EXPORT_ICEDB size_t fs_can_open_path(const wchar_t* p, const char* t, ICEDB_file_open_flags flags) {
		// Can open directories and any file.
		// Overall priority is quite low, as more specialized plugins are more useful here. Still,
		// this can be used for viewing directory contents and querying file metadata / attributes.
		const size_t valid_pri = 10;

		if (!fs_path_exists(nullptr, p)) return 0;

		return 0;
	}
}