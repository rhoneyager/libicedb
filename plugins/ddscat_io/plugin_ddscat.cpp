#include "plugin_ddscat.hpp"
#include "../../germany_api/germany/defs.h"
#include "../../germany_api/germany/misc/os_functions.h"
#include "../../germany_api/germany/misc/util.h"
#include "../../germany_api/germany/dlls/plugins.h"
#include "../../germany_api/germany/dlls/dllsImpl.hpp"

ICEDB_DLL_PLUGINS_COMMON(ddscat_io);
ICEDB_core_util_impl;

extern "C" {

	SHARED_EXPORT_ICEDB bool Register(ICEDB_register_interface_f fReg, ICEDB_get_module_f fMod, ICEDB_DLL_BASE_HANDLE*) {
		const size_t sz = 2048;
		char buf[sz] = "";
		fReg("level_0_ddscat_io_shape", 0, fMod((void*)Register, sz, buf));
		fReg("level_0_ddscat_io_dieltab", 0, fMod((void*)Register, sz, buf));
		fReg("level_0_ddscat_io_fml", 0, fMod((void*)Register, sz, buf));
		fReg("level_0_ddscat_io_sca", 0, fMod((void*)Register, sz, buf));
		fReg("level_0_ddscat_io_avg", 0, fMod((void*)Register, sz, buf));
		fReg("level_0_ddscat_io_ddlog", 0, fMod((void*)Register, sz, buf));
		fReg("level_0_ddscat_io_ddrun", 0, fMod((void*)Register, sz, buf));
		return true;
	}

	SHARED_EXPORT_ICEDB void Unregister(ICEDB_register_interface_f fUnReg, ICEDB_get_module_f fMod) {
		const size_t sz = 2048;
		char buf[sz] = "";
		fUnReg("level_0_ddscat_io_shape", 0, fMod((void*)Unregister, sz, buf));
		fUnReg("level_0_ddscat_io_dieltab", 0, fMod((void*)Unregister, sz, buf));
		fUnReg("level_0_ddscat_io_fml", 0, fMod((void*)Unregister, sz, buf));
		fUnReg("level_0_ddscat_io_sca", 0, fMod((void*)Unregister, sz, buf));
		fUnReg("level_0_ddscat_io_avg", 0, fMod((void*)Unregister, sz, buf));
		fUnReg("level_0_ddscat_io_ddlog", 0, fMod((void*)Unregister, sz, buf));
		fUnReg("level_0_ddscat_io_ddrun", 0, fMod((void*)Unregister, sz, buf));
	}
}
