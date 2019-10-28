#include <string>
#include <iostream>
#include <memory>
#include <exception>
#define BOOST_TEST_MODULE icedb-ddscat7-shapes
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "icedb/misc/os_functions.hpp"
#include "icedb/IO/Shapes.hpp"
#include "HH/Files.hpp"
#include "HH/Groups.hpp"
#include "IntegratedTesting.hpp"

BOOST_TEST_GLOBAL_FIXTURE(icedb_GlobalTestingFixture);

#if (BOOST_VERSION / 100 % 1000) < 59
#define BOOST_TEST BOOST_CHECK
#define BOOST_TEST_REQUIRE BOOST_REQUIRE
#endif

BOOST_AUTO_TEST_CASE(read_ddscat7_shape1)
{
	using namespace std;
	string sShare = icedb::os_functions::getSystemString(icedb::os_functions::System_String::SHARE_DIR);
	const string sfile = sShare + "/examples/shapes/DDSCAT/ddscat7ice.dat";

	auto opts = icedb::registry::options::generate()->filename(sfile)->filetype("ddscat");
	std::vector<std::shared_ptr<icedb::Shapes::NewShapeProperties> > fileShapes;
	icedb::Shapes::NewShapeProperties::readVector(nullptr, opts, fileShapes);

	BOOST_TEST_REQUIRE(fileShapes.size() == 1);
	const auto s = fileShapes[0];
	const size_t numPoints = 9;
	BOOST_TEST(s->scattering_method == "DDSCAT");
	BOOST_TEST(s->particle_scattering_element_number.size() == numPoints);
	BOOST_TEST(s->particle_scattering_element_coordinates_as_ints.size() == numPoints * 3);
	BOOST_TEST(s->particle_scattering_element_coordinates_as_floats.size() == 0);
	BOOST_TEST(s->particle_scattering_element_composition_whole.size() == numPoints);
	BOOST_TEST(s->particle_scattering_element_composition_whole[0] == 1);
	BOOST_TEST(s->particle_scattering_element_composition_fractional.size() == 0);

	const std::vector<int32_t> testpts = {
		 1,  1,  1,
		 0,  1,  1,
		-1,  1,  1,
		 1,  0,  1,
		 0,  1,  1,
		-1,  0,  0,
		 1,  1, -1,
		 0,  1,  0,
		-1,  0,  0 };
	for (size_t i = 0; i < testpts.size(); ++i) {
		BOOST_TEST(testpts[i] == s->particle_scattering_element_coordinates_as_ints[i]);
	}
}

BOOST_AUTO_TEST_CASE(read_ddscat7_shape2)
{
	using namespace std;
	string sShare = icedb::os_functions::getSystemString(icedb::os_functions::System_String::SHARE_DIR);
	const string sfile = sShare + "/examples/shapes/DDSCAT/ddscat7melting.dat";

	auto opts = icedb::registry::options::generate()->filename(sfile)->filetype("ddscat");
	std::vector<std::shared_ptr<icedb::Shapes::NewShapeProperties> > fileShapes;
	icedb::Shapes::NewShapeProperties::readVector(nullptr, opts, fileShapes);

	BOOST_TEST_REQUIRE(fileShapes.size() == 1);
	const auto s = fileShapes[0];
	const size_t numPoints = 9;
	BOOST_TEST(s->scattering_method == "DDSCAT");
	BOOST_TEST(s->particle_scattering_element_number.size() == numPoints);
	BOOST_TEST(s->particle_scattering_element_coordinates_as_ints.size() == numPoints * 3);
	BOOST_TEST(s->particle_scattering_element_coordinates_as_floats.size() == 0);
	BOOST_TEST(s->particle_scattering_element_composition_whole.size() == numPoints);
	BOOST_TEST(s->particle_scattering_element_composition_whole[0] == 1);
	BOOST_TEST(s->particle_scattering_element_composition_fractional.size() == 0);

	const std::vector<int32_t> testpts = {
		 1,  1,  1,
		 0,  1,  1,
		-1,  1,  1,
		 1,  0,  1,
		 0,  1,  1,
		-1,  0,  0,
		 1,  1, -1,
		 0,  1,  0,
		-1,  0,  0 };
	const std::vector<int32_t> testris = { 1, 2, 1, 1, 1, 2, 1, 1, 2 };
	for (size_t i = 0; i < testpts.size(); ++i) {
		BOOST_TEST(testpts[i] == s->particle_scattering_element_coordinates_as_ints[i]);
	}
	for (size_t i = 0; i < testris.size(); ++i) {
		BOOST_TEST(testris[i] == s->particle_scattering_element_composition_whole[i]);
	}
}

BOOST_AUTO_TEST_CASE(write_ddscat7_shape_as_hdf5)
{
	using namespace std;
	string sShare = icedb::os_functions::getSystemString(icedb::os_functions::System_String::SHARE_DIR);
	const string sfile = sShare + "/examples/shapes/DDSCAT/ddscat7ice.dat";

	auto opts = icedb::registry::options::generate()->filename(sfile)->filetype("ddscat");
	std::vector<std::shared_ptr<icedb::Shapes::NewShapeProperties> > fileShapes;
	icedb::Shapes::NewShapeProperties::readVector(nullptr, opts, fileShapes);
	BOOST_TEST_REQUIRE(fileShapes.size() == 1);

	fileShapes[0]->particle_id = "particle_1"; // This would normally get set in the importer program.
	string sBuild = icedb::os_functions::getSystemString(icedb::os_functions::System_String::BUILD_DIR);
	const string sOut = sBuild + "/write_ddscat7_shape_as_hdf5.h5";

	HH::File out = HH::File::createFile(sOut, H5F_ACC_TRUNC);
	auto res = icedb::Shapes::Shape::createShape(out.create("Shape_ddscat7ice"), *fileShapes.at(0).get());
	BOOST_TEST_REQUIRE(res.isGroup() == true);
}
