#include "../../libicedb/icedb/fs/fs_dll_impl.hpp"
#include "../../libicedb/icedb/defs.h"
#include "../../libicedb/icedb/misc/os_functions.h"
#include "../../libicedb/icedb/misc/util.h"
#include "../../libicedb/icedb/dlls/plugins.h"
#include "../../libicedb/icedb/dlls/dllsInterface.hpp"
#include "../../libicedb/icedb/dlls/dllsInterfaceImpl.hpp"
#include "plugin_netcdf.hpp"
#include "plugin_netcdf_interface.h"
#include "plugin_netcdf_impl.hpp"

#include "c:/Program Files/netCDF 4.4.1.1/include/netcdf.h"

ICEDB_DLL_PLUGINS_COMMON(fs_netcdf);
ICEDB_core_mem_impl;
ICEDB_core_util_impl;

namespace icedb {
	namespace plugins {
		namespace fs_netcdf {
			ICEDB_DLL_BASE_HANDLE* hnd = nullptr;
			const uint64_t pluginMagic = 378274376432;
			const char* pluginName = "fs_netcdf";
			ICEDB_fs_plugin_capabilities caps;
			
			std::shared_ptr<ICEDB_DLL_BASE_HANDLE> hndNetCDF = nullptr;
			std::shared_ptr<interface_dlls> i_dlls;
			std::function<void(ICEDB_DLL_BASE_HANDLE*)> hndDestroyer;
			std::shared_ptr<interface_netcdf> i_nc;
		}
	}
}
using namespace icedb::plugins::fs_netcdf;
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
		caps.fs_has_cyclic_links = false;
		caps.has_external_links = false;
		caps.has_folders = true;
		caps.has_xattrs = true;
		if (!p) hnd->_vtable->_raiseExcept(hnd,
			__FILE__, (int)__LINE__, ICEDB_DEBUG_FSIG);
		*p = caps;
	}

	SHARED_EXPORT_ICEDB void fs_destroy(ICEDB_handle_inner* p) {
		if (!isValidHandle(p))
			hnd->_vtable->_raiseExcept(hnd,
				__FILE__, (int)__LINE__, ICEDB_DEBUG_FSIG);
		delete p;
	}

	SHARED_EXPORT_ICEDB bool Register(ICEDB_register_interface_f fReg, ICEDB_get_module_f fMod, ICEDB_DLL_BASE_HANDLE* h) {
		const size_t sz = 2048;
		char buf[sz] = "";
		fReg("fs", 1000, fMod((void*)Register, sz, buf));
		hnd = h;
		i_dlls = std::shared_ptr<interface_dlls>(create_dlls(hnd), destroy_dlls);
		auto hNC = i_dlls->DLL_BASE_HANDLE_create(i_dlls.get(), "C:\\Program Files\\netCDF 4.4.1.1\\bin\\netcdf.dll");
		if (!hNC) {
			return false;
		}
		hndDestroyer = std::bind(i_dlls->DLL_BASE_HANDLE_destroy, i_dlls.get(), std::placeholders::_1);
		auto hndNetCDF = std::shared_ptr<ICEDB_DLL_BASE_HANDLE>(hNC, hndDestroyer);
		ICEDB_error_code dlerr = hndNetCDF->_vtable->open(hndNetCDF.get());
		if (dlerr) hnd->_vtable->_raiseExcept(hnd,
			__FILE__, (int)__LINE__, ICEDB_DEBUG_FSIG);
		bool isOpen = hndNetCDF->_vtable->isOpen(hndNetCDF.get());
		auto i_nc = std::shared_ptr<interface_netcdf>(create_netcdf(hndNetCDF.get()), destroy_netcdf);

		int ncfp = 0;
		auto b = i_nc->Bind_nc_create(i_nc.get());
		int err = i_nc->nc_create(i_nc.get(), "C:\\Users\\ryan\\Documents\\Visual Studio 2017\\Projects\\germany_api_3\\build\\test.nc", NC_NETCDF4, &ncfp);
		if (err != NC_NOERR) hnd->_vtable->_raiseExcept(hnd,
			__FILE__, (int)__LINE__, ICEDB_DEBUG_FSIG);
		i_nc->nc_close(i_nc.get(), ncfp);
		return true;
	}

	SHARED_EXPORT_ICEDB void Unregister(ICEDB_register_interface_f fUnReg, ICEDB_get_module_f fMod) {
		const size_t sz = 2048;
		char buf[sz] = "";
		fUnReg("fs", 1000, fMod((void*)Unregister, sz, buf));
		i_nc = nullptr;
		hndNetCDF = nullptr;
		hndDestroyer = nullptr;
		i_dlls = nullptr;
	}
}
