#pragma once
#include "icedb/defs.h"
#include <string>
#include "icedb/dlls.hpp"

struct icedb_GlobalTestingFixture {
	icedb_GlobalTestingFixture() {}
	void setup() {
		icedb::load();
				
	}
	void teardown() {}
	~icedb_GlobalTestingFixture() {}
};
