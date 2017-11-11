#include "../../libicedb/icedb/fs/fs_dll_impl.hpp"
#include "../../libicedb/icedb/defs.h"
#include "../../libicedb/icedb/misc/os_functions.h"
#include "../../libicedb/icedb/dlls/plugins.h"
#include "../../libicedb/icedb/misc/util.h"
#include "../../libicedb/icedb/misc/utilInterfaceImpl.hpp"
#include "../../libicedb/icedb/misc/memInterfaceImpl.hpp"
#include "../../libicedb/icedb/error/error_contextInterfaceImpl.hpp"
#include "../../libicedb/icedb/error/errorInterfaceImpl.hpp"
#include "fs_posix.hpp"

ICEDB_DLL_PLUGINS_COMMON(fs_posix);

namespace icedb {
	namespace plugins {
		namespace fs_posix {
			ICEDB_DLL_BASE_HANDLE* hnd, *hndSelf;
			const uint64_t pluginMagic = 37392740285;
			const char* pluginName = "fs_posix";
			ICEDB_fs_plugin_capabilities caps;
			std::map<std::string, std::string> libprops;
			std::shared_ptr<interface_ICEDB_core_util> i_util;
			std::shared_ptr<interface_ICEDB_core_mem> i_mem;
			std::shared_ptr<interface_ICEDB_core_error> i_error;
			std::shared_ptr<interface_ICEDB_core_error_context> i_error_context;
			std::shared_ptr<interface_ICEDB_fs_plugin> i_fs_self;
			std::shared_ptr<interface_ICEDB_core_fs> i_fs_core;
			std::string sSelfName;
		}
	}
}
using namespace icedb::plugins::fs_posix;
ICEDB_handle_inner::ICEDB_handle_inner() : magic(pluginMagic) {}

extern "C" {
	
	bool isValidHandleInner(ICEDB_handle_inner* p) {
		if (!p) return false;
		if (p->magic != pluginMagic) return false;
		return true;
	}
	bool isValidHandle(ICEDB_FS_HANDLE_p p) {
		if (!p) return false;
		if (p->magic != pluginMagic) return false;
		if (!p->d) return false;
		if (!p->h) return false;
		//if (!p->h_dest) return false;
		if (!p->i) return false;
		if (!isValidHandleInner(p->h.get())) return false;
		return true;
	}

	ICEDB_FS_HANDLE_p makeHandle() {
		ICEDB_FS_HANDLE_p p(new ICEDB_FS_HANDLE);
		p->magic = pluginMagic;
		p->d = hndSelf;
		//p->h_dest;
		p->h = std::shared_ptr<ICEDB_handle_inner>(new ICEDB_handle_inner);
		p->i = i_fs_self;
		p->pluginName = pluginName;
		return p;
	}

	SHARED_EXPORT_ICEDB void fs_get_capabilities(ICEDB_fs_plugin_capabilities* p) {
		caps.can_copy = false;
		caps.can_delete = true;
		caps.can_hard_link = true;
		caps.can_move = true;
		caps.can_soft_link = false; // If enabled, code elsewhere in plugin needs to change.
		caps.fs_has_cyclic_links = true;
		caps.has_external_links = true;
		caps.has_folders = true;
		caps.has_xattrs = false; // If enabled, change code elsewhere
		if (!p) hnd->_vtable->_raiseExcept(hnd,
			__FILE__, (int)__LINE__, ICEDB_DEBUG_FSIG);
		*p = caps;
	}

	SHARED_EXPORT_ICEDB	ICEDB_file_open_flags fs_get_open_flags(ICEDB_FS_HANDLE_p p) {
		if (!isValidHandle(p))
			hnd->_vtable->_raiseExcept(hnd,
				__FILE__, (int)__LINE__, ICEDB_DEBUG_FSIG);
		return p->open_flags;
	}

	SHARED_EXPORT_ICEDB void fs_destroy(ICEDB_FS_HANDLE_p p) {
		if (!isValidHandle(p)) 
			hnd->_vtable->_raiseExcept(hnd,
			__FILE__, (int)__LINE__, ICEDB_DEBUG_FSIG);
		p->h = nullptr;
		//p->h_dest = nullptr;
		p->i = nullptr;
		p->d = nullptr;
		p->magic = 0;
		delete p;
	}

	SHARED_EXPORT_ICEDB void fs_set_property(ICEDB_FS_HANDLE_p p, const char* key, const char* val) {
		if (!isValidHandle(p) || !key || !val)
			hnd->_vtable->_raiseExcept(hnd,
				__FILE__, (int)__LINE__, ICEDB_DEBUG_FSIG);
		p->h->props[std::string(key)] = std::string(val);
	}

	SHARED_EXPORT_ICEDB void fs_set_global_property(const char* key, const char* val) {
		if (!key || !val)
			hnd->_vtable->_raiseExcept(hnd,
				__FILE__, (int)__LINE__, ICEDB_DEBUG_FSIG);
		libprops[std::string(key)]= std::string(val);
	}

	SHARED_EXPORT_ICEDB size_t fs_get_property(ICEDB_FS_HANDLE_p p, const char* key, size_t mxsz, char** val, size_t *sz) {
		if (!isValidHandle(p) || !key || !mxsz)
			hnd->_vtable->_raiseExcept(hnd,
				__FILE__, (int)__LINE__, ICEDB_DEBUG_FSIG);
		if (p->h->props.count(std::string(key))) {
			std::string sval = p->h->props[std::string(key)];
			size_t res = i_util->strncpy_s(i_util.get(), *val, mxsz, sval.c_str(), sval.length());
			*sz = sval.length() + 1;
			return res;
		} else {
			*val[0] = '\0';
			*sz = 0;
			return 0;
		}
	}

	SHARED_EXPORT_ICEDB size_t fs_get_global_property(const char* key, size_t mxsz, char** val, size_t *sz) {
		if (!key)
			hnd->_vtable->_raiseExcept(hnd,
				__FILE__, (int)__LINE__, ICEDB_DEBUG_FSIG);
		if (libprops.count(std::string(key))) {
			std::string sval = libprops[std::string(key)];
			size_t res = i_util->strncpy_s(i_util.get(), *val, mxsz, sval.c_str(), sval.length());
			*sz = sval.length() + 1;
			return res;
		}
		else {
			*val[0] = '\0';
			*sz = 0;
			return 0;
		}
	}

	SHARED_EXPORT_ICEDB bool Register(ICEDB_register_interface_f fReg, ICEDB_get_module_f fMod, 
		ICEDB_DLL_BASE_HANDLE* hDll, ICEDB_DLL_BASE_HANDLE* hSelf) {
		char cSelf[ICEDB_FS_PATH_CONTENTS_PATH_MAX] = "";
		fMod((void*)Register, ICEDB_FS_PATH_CONTENTS_PATH_MAX, cSelf);
		sSelfName = std::string(cSelf);
		fReg("fs", 1000, cSelf); // 1000 is a really low priority

		hnd = hDll;
		hndSelf = hSelf;
		i_fs_self = std::shared_ptr<interface_ICEDB_fs_plugin>(create_ICEDB_fs_plugin(hSelf), destroy_ICEDB_fs_plugin);

		i_util = std::shared_ptr<interface_ICEDB_core_util>(create_ICEDB_core_util(hDll), destroy_ICEDB_core_util);
		i_mem = std::shared_ptr<interface_ICEDB_core_mem>(create_ICEDB_core_mem(hDll), destroy_ICEDB_core_mem);
		i_error = std::shared_ptr<interface_ICEDB_core_error>(create_ICEDB_core_error(hDll), destroy_ICEDB_core_error);
		i_error_context = std::shared_ptr<interface_ICEDB_core_error_context>(create_ICEDB_core_error_context(hDll), destroy_ICEDB_core_error_context);
		i_fs_core = std::shared_ptr<interface_ICEDB_core_fs>(create_ICEDB_core_fs(hDll), destroy_ICEDB_core_fs);
		return true;
	}

	SHARED_EXPORT_ICEDB void Unregister(ICEDB_register_interface_f fUnReg, ICEDB_get_module_f fMod) {
		fUnReg("fs", 1000, sSelfName.c_str());
		i_util = nullptr;
		i_mem = nullptr;
		i_error = nullptr;
		i_error_context = nullptr;
		i_fs_self = nullptr;
		i_fs_core = nullptr;
		hndSelf = nullptr;
	}
}