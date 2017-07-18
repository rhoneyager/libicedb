#include "../../libicedb/icedb/fs/fs_dll_impl.hpp"
#include "../../libicedb/icedb/defs.h"
#include "../../libicedb/icedb/misc/os_functions.h"
#include "../../libicedb/icedb/dlls/plugins.h"
#include "../../libicedb/icedb/misc/util.h"
#include "fs_win.hpp"

ICEDB_DLL_PLUGINS_COMMON(fs_win);
ICEDB_core_mem_impl;
ICEDB_core_util_impl;

namespace icedb {
	namespace plugins {
		namespace fs_win {
			ICEDB_DLL_BASE_HANDLE* hnd;
			const uint64_t pluginMagic = 74920938403;
			const char* pluginName = "fs_win";
			ICEDB_fs_plugin_capabilities caps;
		}
	}
}
using namespace icedb::plugins::fs_win;
ICEDB_handle_inner::ICEDB_handle_inner() : magic(pluginMagic) {}

extern "C" {
	
	bool isValidHandle(ICEDB_handle_inner* p) {
		if (!p) return false;
		if (p->magic != pluginMagic) return false;
		return true;
	}

	SHARED_EXPORT_ICEDB void fs_get_capabilities(ICEDB_fs_plugin_capabilities* p) {
		caps.can_copy = true;
		caps.can_delete = true;
		caps.can_hard_link = true;
		caps.can_move = true;
		caps.can_soft_link = true;
		caps.fs_has_cyclic_links = true;
		caps.has_external_links = true;
		caps.has_folders = true;
		caps.has_xattrs = true;
		if (!p) hnd->_vtable->_raiseExcept(hnd,
			__FILE__, (int)__LINE__, ICEDB_DEBUG_FSIG);
		*p = caps;
	}

	SHARED_EXPORT_ICEDB void destroy(ICEDB_handle_inner* p) {
		if (!isValidHandle(p)) 
			hnd->_vtable->_raiseExcept(hnd,
			__FILE__, (int)__LINE__, ICEDB_DEBUG_FSIG);
		delete p;
	}

	SHARED_EXPORT_ICEDB bool Register(ICEDB_register_interface_f fReg, ICEDB_get_module_f fMod, ICEDB_DLL_BASE_HANDLE* h) {
		const size_t sz = 2048;
		char buf[sz] = "";
		fReg("fs", -1, fMod((void*)Register, sz, buf));
		hnd = h;
		return true;
	}

	SHARED_EXPORT_ICEDB void Unregister(ICEDB_register_interface_f fUnReg, ICEDB_get_module_f fMod) {
		const size_t sz = 2048;
		char buf[sz] = "";
		fUnReg("fs", -1, fMod((void*)Unregister, sz, buf));
	}
}
