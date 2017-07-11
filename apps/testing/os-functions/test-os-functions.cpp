#include <iostream>
#include <memory>
#include "../../../germany_api/germany/misc/os_functions.hpp"
#include "../../../germany_api/germany/misc/os_functions.h"
#include "../../../germany_api/germany/versioning/versioningForwards.h"
#include "../../../germany_api/germany/versioning/versioning.hpp"

int main(int argc, char** argv) {
	using namespace icedb::os_functions;
	libEntry(argc, argv);
	using namespace std;

	ICEDB_VersionInfo_p ver = ICEDB_getLibVersionInfo();
	const char* vstr = ICEDB_WriteLibVersionInfoCC(ver);
	cout << "Library at " << getLibPath() << endl << vstr << endl;

	auto appver = icedb::versioning::genVersionInfo();
	cout << "App at " << getAppPath() << endl;
	icedb::versioning::debug_preamble(*(appver.get()), cout);


	auto compat = icedb::versioning::compareVersions(ver->p, appver);
	std::string match;
	if (compat == ICEDB_ver_match::ICEDB_VER_EXACT_MATCH) match = "Exact match";
	if (compat == ICEDB_ver_match::ICEDB_VER_COMPATIBLE_3) match = "Level 3 compat";
	if (compat == ICEDB_ver_match::ICEDB_VER_COMPATIBLE_2) match = "Level 2 compat";
	if (compat == ICEDB_ver_match::ICEDB_VER_COMPATIBLE_1) match = "Level 1 compat";
	if (compat == ICEDB_ver_match::ICEDB_VER_INCOMPATIBLE) match = "Incompatible";
	cout << "Are app and lib compatible? " << match << endl << endl;



	ICEDB_VersionInfo_Free(ver);


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
