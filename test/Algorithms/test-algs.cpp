#include <string>
#include <iostream>
#include <memory>
#include <exception>
#define BOOST_TEST_MODULE icedb-algorithms
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/program_options.hpp>
#include "icedb/misc/os_functions.hpp"
#include "icedb/IO/Shapes.hpp"
#include "icedb/Algorithms/ShapeAlgs.hpp"
//#include <icedb/A>
#include "icedb/IO/PPP.hpp"
#include "HH/Files.hpp"
#include "HH/Groups.hpp"
#include "IntegratedTesting.hpp"

HH::File in;
std::string sBuild;
struct icedb_algTestingFixture : protected icedb_GlobalTestingFixture {
	icedb_algTestingFixture() {}
	void setup()
	{
		using namespace std;
		icedb_GlobalTestingFixture::setup();
		sBuild = icedb::os_functions::getSystemString(icedb::os_functions::System_String::BUILD_DIR);
		const string sIn = sBuild + "/write_psu_shape_as_hdf5.h5";
		in = HH::File::openFile(sIn, H5F_ACC_RDONLY);
	}
	void teardown() { icedb_GlobalTestingFixture::teardown(); }
	~icedb_algTestingFixture() {}
};

BOOST_TEST_GLOBAL_FIXTURE(icedb_algTestingFixture);

#if (BOOST_VERSION / 100 % 1000) < 59
#define BOOST_TEST BOOST_CHECK
#define BOOST_TEST_REQUIRE BOOST_REQUIRE
#endif

BOOST_AUTO_TEST_CASE(algs_basic_invocation)
{
	using namespace std;
	const string sOut = sBuild + "/algs_basic_invocation.h5";
	HH::File out = HH::File::createFile(sOut, H5F_ACC_TRUNC);

	icedb::Shapes::Shape shp(in.open("Shape_psuaydinetal_geometry_aggregate_00004_GMM"));
	icedb::PPP::PPP ppp = icedb::PPP::PPP::createPPP(out.create("psu_geom_agg_4_GMM"), shp);
	BOOST_TEST_REQUIRE(ppp.isGroup() == true); // Checkpoint

	// Apply a "dummy" algorithm.
	BOOST_TEST_REQUIRE(icedb::ShapeAlgs::Algorithms::common_algorithms.count("dummy") > 0);
	ppp.apply(icedb::ShapeAlgs::Algorithms::common_algorithms.at("dummy"), shp);
	BOOST_TEST_REQUIRE(ppp.isGroup() == true); // Checkpoint

	// Apply a second "dummy2" algorithm.
	BOOST_TEST_REQUIRE(icedb::ShapeAlgs::Algorithms::common_algorithms.count("dummy2") > 0);
	ppp.apply(icedb::ShapeAlgs::Algorithms::common_algorithms.at("dummy2"), shp);
	BOOST_TEST_REQUIRE(ppp.isGroup() == true); // Checkpoint

	//
	//BOOST_TEST_REQUIRE(icedb::ShapeAlgs::Algorithms::common_algorithms.count("copy_to_ppp") > 0);
	//ppp.apply(icedb::ShapeAlgs::Algorithms::common_algorithms.at("copy_to_ppp"), shp);
	//BOOST_TEST_REQUIRE(ppp.isGroup() == true); // Checkpoint

	//
	BOOST_TEST_REQUIRE(icedb::ShapeAlgs::Algorithms::common_algorithms.count("ConvexHull") > 0);
	ppp.apply(icedb::ShapeAlgs::Algorithms::common_algorithms.at("ConvexHull"), shp);
	BOOST_TEST_REQUIRE(ppp.isGroup() == true); // Checkpoint

	//
	BOOST_TEST_REQUIRE(icedb::ShapeAlgs::Algorithms::common_algorithms.count("MaxDistanceTwoPoints") > 0);
	ppp.apply(icedb::ShapeAlgs::Algorithms::common_algorithms.at("MaxDistanceTwoPoints"), shp);
	BOOST_TEST_REQUIRE(ppp.isGroup() == true); // Checkpoint

	//
	//BOOST_TEST_REQUIRE(icedb::ShapeAlgs::Algorithms::common_algorithms.count("SmallestCircumscribingSphere_Points") > 0);
	//ppp.apply(icedb::ShapeAlgs::Algorithms::common_algorithms.at("SmallestCircumscribingSphere_Points"), shp);
	//BOOST_TEST_REQUIRE(ppp.isGroup() == true); // Checkpoint
}
