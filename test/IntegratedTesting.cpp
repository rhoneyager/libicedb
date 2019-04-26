#include "icedb/defs.h"
#include <string>
#include "IntegratedTesting.hpp"
#include "IntegratedTesting/testing-settings.h"
#include "icedb/dlls.hpp"
#include "icedb/misc/os_functions.hpp"

using namespace icedb;
using namespace icedb::os_functions;
using std::string;
void icedb_GlobalTestingFixture::setup() {
	icedb::load();
	setSystemString(System_String::SHARE_DIR,
		string(libicedb_sourcedir) + "/share/icedb");
	setSystemString(System_String::BUILD_DIR,
		string(libicedb_binarydir) + "/test");
}
icedb_GlobalTestingFixture::icedb_GlobalTestingFixture() {}
void icedb_GlobalTestingFixture::teardown() {}
icedb_GlobalTestingFixture::~icedb_GlobalTestingFixture() {}
