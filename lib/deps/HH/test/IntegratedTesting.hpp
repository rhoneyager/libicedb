#pragma once
#include <string>
#include "HH/Handles.hpp"
#include "HH/Files.hpp"

namespace Testing {
	std::string getUniqueFileName();
	HH::HH_hid_t get_fapl_for_testing();
	HH::File createTestFile();
}

struct HH_GlobalTestingFixture {
	HH_GlobalTestingFixture();
	void setup();
	void teardown();
	virtual ~HH_GlobalTestingFixture();
};
