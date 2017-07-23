#include "../../libicedb/icedb/fs/fs_dll_impl.hpp"
#include "../../libicedb/icedb/defs.h"
#include "../../libicedb/icedb/misc/os_functions.h"
#include "../../libicedb/icedb/dlls/plugins.h"
#include "../../libicedb/icedb/misc/util.h"
#include "../../libicedb/icedb/misc/utilInterfaceImpl.hpp"
#include "../../libicedb/icedb/misc/memInterfaceImpl.hpp"
#include "../../libicedb/icedb/error/error_contextInterfaceImpl.hpp"
#include "../../libicedb/icedb/error/errorInterfaceImpl.hpp"
#include "fs_win.hpp"

ICEDB_DLL_PLUGINS_COMMON(fs_win);

namespace icedb {
	namespace plugins {
		namespace fs_win {
			ICEDB_DLL_BASE_HANDLE* hnd;
			const uint64_t pluginMagic = 74920938403;
			const char* pluginName = "fs_win";
			ICEDB_fs_plugin_capabilities caps;
			std::map<std::string, std::string> libprops;
			std::shared_ptr<interface_ICEDB_core_util> i_util;
			std::shared_ptr<interface_ICEDB_core_mem> i_mem;
			std::shared_ptr<interface_ICEDB_core_error> i_error;
			std::shared_ptr<interface_ICEDB_core_error_context> i_error_context;
		}
	}
}
using namespace icedb::plugins::fs_win;
ICEDB_handle_inner::ICEDB_handle_inner() : magic(pluginMagic) {}

extern "C" {
	
	SHARED_EXPORT_ICEDB bool isValidHandleInner(ICEDB_handle_inner* p) {
		if (!p) return false;
		if (p->magic != pluginMagic) return false;
		return true;
	}
	SHARED_EXPORT_ICEDB bool isValidHandle(ICEDB_FS_HANDLE_p p) {
		if (!p) return false;
		if (p->magic != pluginMagic) return false;
		if (!p->d) return false;
		if (!p->h) return false;
		if (!p->h_dest) return false;
		if (!p->i) return false;
		if (!isValidHandleInner(p->h.get())) return false;
		return true;
	}

	SHARED_EXPORT_ICEDB void fs_get_capabilities(ICEDB_fs_plugin_capabilities* p) {
		caps.can_copy = true;
		caps.can_delete = true;
		caps.can_hard_link = true;
		caps.can_move = true;
		caps.can_soft_link = false; // If enabled, code elsewhere in plugin needs to change.
		caps.fs_has_cyclic_links = true;
		caps.has_external_links = true;
		caps.has_folders = true;
		caps.has_xattrs = true;
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
		p->h_dest = nullptr;
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

	SHARED_EXPORT_ICEDB bool Register(ICEDB_register_interface_f fReg, ICEDB_get_module_f fMod, ICEDB_DLL_BASE_HANDLE* h) {
		const size_t sz = 2048;
		char buf[sz] = "";
		fReg("fs", 1000, fMod((void*)Register, sz, buf));
		hnd = h;

		i_util = std::shared_ptr<interface_ICEDB_core_util>(create_ICEDB_core_util(h), destroy_ICEDB_core_util);
		i_mem = std::shared_ptr<interface_ICEDB_core_mem>(create_ICEDB_core_mem(h), destroy_ICEDB_core_mem);
		i_error = std::shared_ptr<interface_ICEDB_core_error>(create_ICEDB_core_error(h), destroy_ICEDB_core_error);
		i_error_context = std::shared_ptr<interface_ICEDB_core_error_context>(create_ICEDB_core_error_context(h), destroy_ICEDB_core_error_context);
		return true;
	}

	SHARED_EXPORT_ICEDB void Unregister(ICEDB_register_interface_f fUnReg, ICEDB_get_module_f fMod) {
		const size_t sz = 2048;
		char buf[sz] = "";
		fUnReg("fs", 1000, fMod((void*)Unregister, sz, buf));
		i_util = nullptr;
		i_mem = nullptr;
		i_error = nullptr;
		i_error_context = nullptr;
	}
}
