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

