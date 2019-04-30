#include <string>
#include <iostream>
#include <memory>
#include <exception>
#define BOOST_TEST_MODULE icedb-plugin-miniball
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/program_options.hpp>
#include <icedb/misc/os_functions.hpp>
#include <icedb/Shapes.hpp>
#include <icedb/ShapeAlgs.hpp>
#include <icedb/PPP.hpp>
#include <HH/Files.hpp>
#include <HH/Groups.hpp>
#include "IntegratedTesting.hpp"

BOOST_TEST_GLOBAL_FIXTURE(icedb_GlobalTestingFixture);

#if (BOOST_VERSION / 100 % 1000) < 59
#define BOOST_TEST BOOST_CHECK
#define BOOST_TEST_REQUIRE BOOST_REQUIRE
#endif

BOOST_AUTO_TEST_CASE(miniball_generate_ppp_block)
{
	using namespace std;
	string sShare = icedb::os_functions::getSystemString(icedb::os_functions::System_String::SHARE_DIR);
	string sBuild = icedb::os_functions::getSystemString(icedb::os_functions::System_String::BUILD_DIR);
	const string sIn = sShare + "/examples/shapes/PSU-GMM/write_psu_shape_as_hdf5.h5";
	const string sOut = sBuild + "/miniball_generate_ppp_block.h5";
	HH::File in = HH::File::openFile(sIn, H5F_ACC_RDONLY);
	HH::File out = HH::File::createFile(sOut, H5F_ACC_TRUNC);

	icedb::Shapes::Shape shp(in.open("Shape_psuaydinetal_geometry_aggregate_00004_GMM"));
	icedb::PPP::PPP ppp = icedb::PPP::PPP::createPPP(out.create("miniball_test"), shp);
	BOOST_TEST_REQUIRE(ppp.isGroup() == true);

	// Apply a "dummy" algorithm.
	BOOST_TEST_REQUIRE(icedb::ShapeAlgs::Algorithms::common_algorithms.count("HbfMiniball1") > 0);
	ppp.apply(icedb::ShapeAlgs::Algorithms::common_algorithms.at("HbfMiniball1"), shp);

	BOOST_TEST_REQUIRE(ppp.isGroup() == true);
}
