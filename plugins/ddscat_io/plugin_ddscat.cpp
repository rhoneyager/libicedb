#include "plugin_ddscat.hpp"
#include "../../libicedb/icedb/defs.h"
#include "../../libicedb/icedb/misc/os_functions.h"
#include "../../libicedb/icedb/misc/mem.h"
#include "../../libicedb/icedb/misc/memInterfaceImpl.hpp"
#include "../../libicedb/icedb/misc/util.h"
#include "../../libicedb/icedb/misc/utilInterfaceImpl.hpp"

ICEDB_DLL_PLUGINS_COMMON(level_0_ddscat_io);

namespace icedb {
	namespace plugins {
		namespace ddscat_io {
			ICEDB_DLL_BASE_HANDLE* h;
		}
	}
}




extern "C" {

	SHARED_EXPORT_ICEDB bool Register(ICEDB_register_interface_f fReg, ICEDB_get_module_f fMod, ICEDB_DLL_BASE_HANDLE* h, ICEDB_DLL_BASE_HANDLE*) {
		const size_t sz = 2048;
		char buf[sz] = "";
		fReg("level_0_ddscat_io_shape", 0, fMod((void*)Register, sz, buf));
		/*
		fReg("level_0_ddscat_io_dieltab", 0, fMod((void*)Register, sz, buf));
		fReg("level_0_ddscat_io_fml", 0, fMod((void*)Register, sz, buf));
		fReg("level_0_ddscat_io_sca", 0, fMod((void*)Register, sz, buf));
		fReg("level_0_ddscat_io_avg", 0, fMod((void*)Register, sz, buf));
		fReg("level_0_ddscat_io_ddlog", 0, fMod((void*)Register, sz, buf));
		fReg("level_0_ddscat_io_ddrun", 0, fMod((void*)Register, sz, buf));
		*/
		icedb::plugins::ddscat_io::h = h;
		return true;
	}

	SHARED_EXPORT_ICEDB void Unregister(ICEDB_register_interface_f fUnReg, ICEDB_get_module_f fMod) {
		const size_t sz = 2048;
		char buf[sz] = "";
		fUnReg("level_0_ddscat_io_shape", 0, fMod((void*)Unregister, sz, buf));
		/*
		fUnReg("level_0_ddscat_io_dieltab", 0, fMod((void*)Unregister, sz, buf));
		fUnReg("level_0_ddscat_io_fml", 0, fMod((void*)Unregister, sz, buf));
		fUnReg("level_0_ddscat_io_sca", 0, fMod((void*)Unregister, sz, buf));
		fUnReg("level_0_ddscat_io_avg", 0, fMod((void*)Unregister, sz, buf));
		fUnReg("level_0_ddscat_io_ddlog", 0, fMod((void*)Unregister, sz, buf));
		fUnReg("level_0_ddscat_io_ddrun", 0, fMod((void*)Unregister, sz, buf));
		*/
	}
}
