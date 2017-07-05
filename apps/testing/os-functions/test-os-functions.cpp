#include <iostream>
#include <memory>
#include "../../../germany_api/germany/os_functions.hpp"
#include "../../../germany_api/germany/os_functions.h"

int main(int argc, char** argv) {
	using namespace icedb::os_functions;
	libEntry(argc, argv);
	using namespace std;
	cout << "User name: " << getUserName() << endl
		<< "Host name: " << getHostName() << endl
		<< "Home dir: " << getHomeDir() << endl
		<< "App config dir: " << getAppConfigDir() << endl;
	const int sz = 500;
	char bAppDir[sz] = "", bCWD[sz] = "", bLibDir[sz] = "";
	ICEDB_getAppDir(sz, bAppDir);
	ICEDB_getCWD(sz, bCWD);
	ICEDB_getLibDir(sz, bLibDir);
	cout << "App dir: " << bAppDir << endl
		<< "CWD: " << bCWD << endl
		<< "Lib dir: " << bLibDir << endl
		<< "Loaded modules:" << endl;
	std::shared_ptr<ICEDB_enumModulesRes> r
	(ICEDB_enumModules(getPID()), ICEDB_free_enumModulesRes);
	if (r) {
		for (size_t i = 0; i < r->sz; ++i)
			cout << "\t" << r->modules[i] << endl;
	}
 	return 0;
}
