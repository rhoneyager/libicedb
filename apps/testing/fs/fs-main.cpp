#include <iostream>
#include <memory>
#include <string>
#include "../../../libicedb/icedb/misc/os_functions.h"
#include "../../../libicedb/icedb/units/units.hpp"
#include "../../../libicedb/icedb/dlls/dlls.hpp"
#include "../../../libicedb/icedb/fs/fs.h"
int main(int argc, char** argv) {
	using namespace std;
	ICEDB_libEntry(argc, argv);
	string pDir(ICEDB_getPluginDirC());
	char libncp[50] = "";
	ICEDB_dll_name_mangle_simple("netcdf_impl", libncp, 50);
	string pncp = pDir + "/" + string(libncp);
	ICEDB_load_plugin(pncp.c_str());

	string basepath = pDir;
	if (argc > 1) {
		basepath = string(argv[1]);
	}
	ICEDB_FS_HANDLE_p f = ICEDB_file_handle_create(basepath.c_str(), nullptr, ICEDB_flags_rw);

	ICEDB_FS_PATH_CONTENTS **fc = nullptr;
	size_t numObjs = 0;
	ICEDB_error_code err = ICEDB_file_handle_readobjs(f, nullptr, &numObjs, &fc);

	cout << "Listing contents of " << pDir << endl;
	cout << "Using filesystem plugin: " << ICEDB_file_handle_get_name(f) << endl;
	for (size_t i = 0; i < numObjs; ++i) {
		cout << i << " - " << (*fc)[i].p_name << " - "
			<< ((*fc)[i].p_obj_type) << endl;
	}

	err = ICEDB_file_handle_free_objs(f, fc);
	ICEDB_file_handle_destroy(f);

	//const size_t mangledNameSz = 200;
	//char mangledName[mangledNameSz] = "";
	//ICEDB_dll_name_mangle_simple("units-simple", mangledName, mangledNameSz);
	//std::string testP = pDir + "/" + std::string(mangledName);
	//ICEDB_load_plugin(testP.c_str());


	return 0;
}
