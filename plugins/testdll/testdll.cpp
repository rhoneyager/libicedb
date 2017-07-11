#include "../../germany_api/germany/defs.h"
#include "../../germany_api/germany/misc/os_functions.h"
#include "../../germany_api/germany/dlls/plugins.h"

ICEDB_DLL_PLUGINS_COMMON(testdll);

extern "C" {
	SHARED_EXPORT_ICEDB int testfunc(int a) {
		return (a < 16) ? 2 * a : 43;
	}

	static int val = 0;
	SHARED_EXPORT_ICEDB int set(int a) {
		val = a;
		return a;
	}
	SHARED_EXPORT_ICEDB int get() {
		return val;
	}


	
	SHARED_EXPORT_ICEDB bool Register(ICEDB_register_interface_f fReg, ICEDB_get_module_f fMod) {
		const size_t sz = 2048;
		char buf[sz] = "";
		fReg("testdll", 0, fMod((void*)Register, sz, buf));
		return true;
	}

	SHARED_EXPORT_ICEDB void Unregister(ICEDB_register_interface_f fUnReg, ICEDB_get_module_f fMod) {
		const size_t sz = 2048;
		char buf[sz] = "";
		fUnReg("testdll", 0, fMod((void*)Unregister, sz, buf));
	}
}
