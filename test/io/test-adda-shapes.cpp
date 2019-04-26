#include <string>
#include <iostream>
#include <memory>
#include <exception>
#define BOOST_TEST_MODULE icedb-adda-shapes
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

BOOST_AUTO_TEST_CASE(read_adda_rawtext_nocomments)
{
	using namespace std;
	string sShare = icedb::os_functions::getSystemString(icedb::os_functions::System_String::SHARE_DIR);
	const string sfile = sShare + "/examples/shapes/ADDA/"
		+ "rawtext_nocomments.adda";

	auto opts = icedb::registry::options::generate()->filename(sfile)->filetype("adda");
	std::vector<std::shared_ptr<icedb::Shapes::NewShapeProperties> > fileShapes;
	icedb::Shapes::NewShapeProperties::readVector(nullptr, opts, fileShapes);

	BOOST_TEST_REQUIRE(fileShapes.size() == 1);
	const auto s = fileShapes[0];
	const size_t numPoints = 2176;
	BOOST_TEST(s->scattering_method == "ADDA");
	BOOST_TEST(s->particle_scattering_element_number.size() == numPoints);
	BOOST_TEST(s->particle_scattering_element_coordinates_as_ints.size() == numPoints * 3);
	BOOST_TEST(s->particle_scattering_element_coordinates_as_floats.size() == 0);
	BOOST_TEST(s->particle_scattering_element_composition_whole.size() == numPoints);
	BOOST_TEST(s->particle_scattering_element_composition_whole[0] == 1);
	BOOST_TEST(s->particle_scattering_element_composition_fractional.size() == 0);
}

BOOST_AUTO_TEST_CASE(read_adda_sphere_geom)
{
	using namespace std;
	string sShare = icedb::os_functions::getSystemString(icedb::os_functions::System_String::SHARE_DIR);
	const string sfile = sShare + "/examples/shapes/ADDA/"
		+ "sphere.geom";

	auto opts = icedb::registry::options::generate()->filename(sfile)->filetype("adda");
	std::vector<std::shared_ptr<icedb::Shapes::NewShapeProperties> > fileShapes;
	icedb::Shapes::NewShapeProperties::readVector(nullptr, opts, fileShapes);

	BOOST_TEST_REQUIRE(fileShapes.size() == 1);
	const auto s = fileShapes[0];
	const size_t numPoints = 2176;
	BOOST_TEST(s->scattering_method == "ADDA");
	BOOST_TEST(s->particle_scattering_element_number.size() == numPoints);
	BOOST_TEST(s->particle_scattering_element_coordinates_as_ints.size() == numPoints * 3);
	BOOST_TEST(s->particle_scattering_element_coordinates_as_floats.size() == 0);
	BOOST_TEST(s->particle_scattering_element_composition_whole.size() == numPoints);
	BOOST_TEST(s->particle_scattering_element_composition_whole[0] == 1);
	BOOST_TEST(s->particle_scattering_element_composition_fractional.size() == 0);
}

