#include <string>
#include <iostream>
#include <memory>
#include <exception>
#define BOOST_TEST_MODULE icedb-psu-shapes
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

BOOST_AUTO_TEST_CASE(read_psu_gmm)
{
	using namespace std;
	string sShare = icedb::os_functions::getSystemString(icedb::os_functions::System_String::SHARE_DIR);
	const string sfile = sShare + "/examples/shapes/PSU-GMM/"
		+ "psuaydinetal_geometry_aggregate_00004_GMM.nc";

	auto opts = icedb::registry::options::generate()->filename(sfile)->filetype("psu");
	std::vector<std::shared_ptr<icedb::Shapes::NewShapeProperties> > fileShapes;
	icedb::Shapes::NewShapeProperties::readVector(nullptr, opts, fileShapes);

	BOOST_TEST_REQUIRE(fileShapes.size() == 1);
	const auto s = fileShapes[0];
	const size_t numPoints = 580;
	BOOST_TEST(s->particle_id == "Aggregate 00004");
	BOOST_TEST(s->scattering_method == "GMM");
	BOOST_TEST(s->particle_scattering_element_number.size() == numPoints);
	BOOST_TEST(s->particle_scattering_element_coordinates_as_floats.size() == numPoints*3);
	BOOST_TEST(s->particle_scattering_element_coordinates_as_ints.size() == 0);
	BOOST_TEST(s->particle_scattering_element_composition_whole.size() == numPoints);
	BOOST_TEST(s->particle_scattering_element_composition_whole[0] == 1);
	BOOST_TEST(s->particle_scattering_element_composition_fractional.size() == 0);

	BOOST_TEST(s->particle_scattering_element_radius.size() == numPoints);
	BOOST_TEST(s->particle_scattering_element_radius[40] > 0.0122f);
	BOOST_TEST(s->particle_scattering_element_radius[40] < 0.013f);


}

BOOST_AUTO_TEST_CASE(write_psu_as_hdf5)
{
	using namespace std;
	string sShare = icedb::os_functions::getSystemString(icedb::os_functions::System_String::SHARE_DIR);
	const string sfile = sShare + "/examples/shapes/PSU-GMM/"
		+ "psuaydinetal_geometry_aggregate_00004_GMM.nc";

	auto opts = icedb::registry::options::generate()->filename(sfile)->filetype("psu");
	std::vector<std::shared_ptr<icedb::Shapes::NewShapeProperties> > fileShapes;
	icedb::Shapes::NewShapeProperties::readVector(nullptr, opts, fileShapes);
	BOOST_TEST_REQUIRE(fileShapes.size() == 1);

	string sBuild = icedb::os_functions::getSystemString(icedb::os_functions::System_String::BUILD_DIR);
	const string sOut = sBuild + "/write_psu_shape_as_hdf5.h5";

	HH::File out = HH::File::createFile(sOut, H5F_ACC_TRUNC);
	auto res = icedb::Shapes::Shape::createShape(
		out.create("Shape_psuaydinetal_geometry_aggregate_00004_GMM"), *(fileShapes[0].get()));
	BOOST_TEST_REQUIRE(res.isGroup() == true);
}

BOOST_AUTO_TEST_CASE(psu_generate_ppp_block)
{
	using namespace std;
	string sBuild = icedb::os_functions::getSystemString(icedb::os_functions::System_String::BUILD_DIR);
	const string sIn = sBuild + "/write_psu_shape_as_hdf5.h5";
	const string sOut = sBuild + "/psu_generate_ppp_block.h5";
	HH::File in = HH::File::openFile(sIn, H5F_ACC_RDONLY);
	HH::File out = HH::File::createFile(sOut, H5F_ACC_TRUNC);

	icedb::Shapes::Shape shp(in.open("Shape_psuaydinetal_geometry_aggregate_00004_GMM"));
	icedb::PPP::PPP ppp = icedb::PPP::PPP::createPPP(out.create("psu_geom_agg_4_GMM"), shp);
	BOOST_TEST_REQUIRE(ppp.isGroup() == true);

	// Apply a "dummy" algorithm.
	BOOST_TEST_REQUIRE(icedb::ShapeAlgs::Algorithms::common_algorithms.count("dummy") > 0);
	ppp.apply(icedb::ShapeAlgs::Algorithms::common_algorithms.at("dummy"), shp);

	BOOST_TEST_REQUIRE(ppp.isGroup() == true);
}
