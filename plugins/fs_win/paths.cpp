#define __STDC_WANT_LIB_EXT1__
#include "../../libicedb/icedb/defs.h"
#include "../../libicedb/icedb/fs/fs.h"
#include "../../libicedb/icedb/misc/os_functions.h"
#include "../../libicedb/icedb/dlls/plugins.h"
#include "../../libicedb/icedb/misc/util.h"
#include <string>
#include <cwchar>
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
		wcsncpy_s(data->base_path, p->h->cwd.data(), ICEDB_FS_PATH_CONTENTS_PATH_MAX);
		data->idx = -1;
		if (path)
			wcsncpy_s(data->p_name, ICEDB_FS_PATH_CONTENTS_PATH_MAX, path, ICEDB_FS_PATH_CONTENTS_PATH_MAX);
		else wcsncpy_s(data->p_name, ICEDB_FS_PATH_CONTENTS_PATH_MAX, L"", ICEDB_FS_PATH_CONTENTS_PATH_MAX);

		data->p_type = ICEDB_path_types::ICEDB_type_unknown;
		if (fs_path_exists(p, path)) {
			DWORD winatts = GetFileAttributesW(effpath.data());
			if (FILE_ATTRIBUTE_DIRECTORY & winatts) {
				data->p_type = ICEDB_path_types::ICEDB_type_folder;
				strncpy_s(data->p_obj_type, ICEDB_FS_PATH_CONTENTS_PATH_MAX, "folder", ICEDB_FS_PATH_CONTENTS_PATH_MAX);
			} else if (FILE_ATTRIBUTE_REPARSE_POINT & winatts) {
				data->p_type = ICEDB_path_types::ICEDB_type_symlink;
				strncpy_s(data->p_obj_type, ICEDB_FS_PATH_CONTENTS_PATH_MAX, "symlink", ICEDB_FS_PATH_CONTENTS_PATH_MAX);
			}else {
				data->p_type = ICEDB_path_types::ICEDB_type_normal_file;
				strncpy_s(data->p_obj_type, ICEDB_FS_PATH_CONTENTS_PATH_MAX, "file", ICEDB_FS_PATH_CONTENTS_PATH_MAX);
			}
		} else {
			data->p_type = ICEDB_path_types::ICEDB_type_nonexistant;
			strncpy_s(data->p_obj_type, ICEDB_FS_PATH_CONTENTS_PATH_MAX, "", ICEDB_FS_PATH_CONTENTS_PATH_MAX);
		}
		return 0;
	}

	SHARED_EXPORT_ICEDB ICEDB_error_code fs_copy(ICEDB_FS_HANDLE_p p,
		const wchar_t* from, const wchar_t* to, bool overwrite) {
		std::wstring effpathFrom = makeEffPath(p, from);
		std::wstring effpathTo = makeEffPath(p, to);

		ICEDB_error_code res = NULL;
		bool opres = CopyFileW(effpathFrom.data(), effpathTo.data(), !overwrite);
		if (!opres) {
			size_t err = GetLastError();
			// TODO: Add dll support for the error subsystem!
		}
		return res;
	}

	SHARED_EXPORT_ICEDB ICEDB_error_code fs_move(ICEDB_FS_HANDLE_p p,
		const wchar_t* from, const wchar_t* to, bool overwrite) {
	}
	
	SHARED_EXPORT_ICEDB ICEDB_error_code fs_unlink(ICEDB_FS_HANDLE_p p, const wchar_t* path) {

	}
	ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(ICEDB_fs_plugin,
		create_hard_link, ICEDB_error_code, ICEDB_FS_HANDLE_p, const wchar_t*, const wchar_t*);
	ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(ICEDB_fs_plugin,
		create_sym_link, ICEDB_error_code, ICEDB_FS_HANDLE_p, const wchar_t*, const wchar_t*);
	ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(ICEDB_fs_plugin,
		follow_sym_link, ICEDB_error_code, ICEDB_FS_HANDLE_p,
		const wchar_t*, size_t, size_t*, wchar_t**);

	SHARED_EXPORT_ICEDB size_t fs_can_open_path(const wchar_t* p, const char* t, ICEDB_file_open_flags flags) {
		// Can open directories and any file.
		// Overall priority is quite low, as more specialized plugins are more useful here. Still,
		// this can be used for viewing directory contents and querying file metadata / attributes.
		const size_t valid_pri = 10;

		if (!fs_path_exists(nullptr, p)) return 0;
		ICEDB_FS_PATH_CONTENTS finfo;
		fs_path_info(nullptr, p, &finfo);

		// This plugin currently does not resolve symbolic links.]
		// If file type is a file or a directory, then return valid_pri.
		if (finfo.p_type == ICEDB_path_types::ICEDB_type_folder) return valid_pri;
		if (finfo.p_type == ICEDB_path_types::ICEDB_type_normal_file) return valid_pri;

		return 0;
	}
}