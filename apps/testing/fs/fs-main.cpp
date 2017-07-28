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

	ICEDB_FS_HANDLE_p f = ICEDB_file_handle_create(pDir.c_str(), nullptr, ICEDB_flags_rw);

	ICEDB_FS_PATH_CONTENTS **fc = nullptr;
	
	ICEDB_error_code err = ICEDB_file_handle_readobjs(f, nullptr, &fc);



	err = ICEDB_file_handle_free_objs(f, fc);
	ICEDB_file_handle_destroy(f);

	//const size_t mangledNameSz = 200;
	//char mangledName[mangledNameSz] = "";
	//ICEDB_dll_name_mangle_simple("units-simple", mangledName, mangledNameSz);
	//std::string testP = pDir + "/" + std::string(mangledName);
	//ICEDB_load_plugin(testP.c_str());


	return 0;
}
