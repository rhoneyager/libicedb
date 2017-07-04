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
		<< "App config dir: " << getAppConfigDir() << endl
		<< "Loaded modules:" << endl;

	std::shared_ptr<ICEDB_enumModulesRes> r
	(ICEDB_enumModules(getPID()), ICEDB_free_enumModulesRes);
	if (r) {
		for (size_t i = 0; i < r->sz; ++i)
			cout << "\t" << r->modules[i] << endl;
	}
 	return 0;
}
