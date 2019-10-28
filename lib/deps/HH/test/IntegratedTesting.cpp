#include <string>
#include <sstream>
#include <iostream>
#include "IntegratedTesting.hpp"
#include "HH/Files.hpp"
//#include "HH/PropertyLists.hpp"

using std::string;

namespace Testing {
	std::string getUniqueFileName()
	{
		static int i = 0;
		std::ostringstream out;
		out << "HH-tests-file-" << i << ".h5"; // Change this if debugging to file.
		++i;
		return out.str();
	}

	HH::HH_hid_t get_fapl_for_testing()
	{
		using namespace HH;
		hid_t plid = H5Pcreate(H5P_FILE_ACCESS);
		Expects(plid >= 0);
		HH_hid_t pl(plid, Handles::Closers::CloseHDF5PropertyList::CloseP);

		size_t increment = 1 * 1024 * 1024; /*1MB*/
		const bool flush_on_close = true; // Change this if debugging to file.
		H5Pset_fapl_core(pl.get(), increment, flush_on_close);

		return pl;
	}

	HH::File createTestFile() {
		return HH::File::createFile(
			Testing::getUniqueFileName(), H5F_ACC_TRUNC, 0, Testing::get_fapl_for_testing());
	}
}

void HH_GlobalTestingFixture::setup() {
	//HH::File::
	//HH::load();
}

HH_GlobalTestingFixture::HH_GlobalTestingFixture() {}
void HH_GlobalTestingFixture::teardown() {}
HH_GlobalTestingFixture::~HH_GlobalTestingFixture() {}
