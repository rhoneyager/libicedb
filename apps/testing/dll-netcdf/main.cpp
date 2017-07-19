#include <iostream>
#include <functional>
#include <vector>
#include "../../../libicedb/icedb/error/error.hpp"
#include "../../../libicedb/icedb/error/error.h"
#include "../../../libicedb/icedb/error/error_context.h"
#include "../../../libicedb/icedb/misc/mem.h"
#include "../../../libicedb/icedb/dlls/dlls.h"
#include "../../../libicedb/icedb/dlls/dlls.hpp"
#include "../../../libicedb/icedb/dlls/dllsImpl.hpp"
#include "../../../libicedb/icedb/misc/os_functions.hpp"
#include "../../../libicedb/icedb/misc/os_functions.h"

/*
ICEDB_DLL_CPP_INTERFACE_BEGIN(testdllcpp, testdll)
ICEDB_DLL_CPP_INTERFACE_DECLARE_FUNCTION(testdllcpp, setint, int, int)
ICEDB_DLL_CPP_INTERFACE_DECLARE_FUNCTION(testdllcpp, getint, int)
ICEDB_DLL_CPP_INTERFACE_END

ICEDB_DLL_CPP_INTERFACE_IMPLEMENTATION_BEGIN(testdllcpp, testdll)
ICEDB_DLL_CPP_INTERFACE_IMPLEMENTATION_FUNCTION(testdll, getint, int)
ICEDB_DLL_CPP_INTERFACE_IMPLEMENTATION_FUNCTION(testdll, setint, int, int)
ICEDB_DLL_CPP_INTERFACE_IMPLEMENTATION_END
*/


int main(int argc, char** argv) {
	ICEDB_libEntry(argc, argv);

	std::string pDir(ICEDB_getPluginDirC());
	const size_t mangledNameSz = 200;
	char mangledName[mangledNameSz] = "";
	ICEDB_dll_name_mangle_simple("netcdf_impl", mangledName, mangledNameSz);
	std::string testP = pDir + "/" + std::string(mangledName);
	ICEDB_load_plugin(testP.c_str());

	std::vector<std::string> paths = icedb::dll::query_interface("fs");

	if (paths.size() == 0) {
		printf("Cannot find an appropriate plugin");
		return 1;
	} else printf("Using dll %s.\n", paths[0].c_str());

	const char* dlpath = paths[0].c_str();
	auto dllInst1 = ICEDB_DLL_BASE_HANDLE_create(dlpath);
	/*
	auto td1a = create_testdll(dllInst1);
#define MUST_RETURN(x) if(x) { \
		auto cxt = icedb::error::get_error_context_thread_local(); \
		if (cxt) printf("%s", icedb::error::stringify(cxt).c_str()); \
		ICEDB_DEBUG_RAISE_EXCEPTION(); \
	}
	MUST_RETURN(td1a->_base->_vtable->open(td1a->_base));

	destroy_testdll(td1a);
	*/
	
	ICEDB_DLL_BASE_HANDLE_destroy(dllInst1);
	return 0;
}
