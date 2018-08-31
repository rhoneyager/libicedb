#include <string>
#include <iostream>
#include <memory>
#include <exception>
#define BOOST_TEST_MODULE icedb-ddscat6-shapes
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include <icedb/misc/os_functions.hpp>
#include <icedb/shape.hpp>
#include <HH/Files.hpp>
#include <HH/Groups.hpp>

#if (BOOST_VERSION / 100 % 1000) < 59
#define BOOST_TEST BOOST_CHECK
#define BOOST_TEST_REQUIRE BOOST_REQUIRE
#endif

BOOST_AUTO_TEST_CASE(read_ddscat6_shape1)
{
	using namespace std;
	const string sShareDir(icedb::os_functions::getShareDir());
	const string sfile = sShareDir + "/examples/shapes/DDSCAT/ddscat6ice.dat";

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

BOOST_AUTO_TEST_CASE(read_ddscat6_shape2)
{
	using namespace std;
	const string sShareDir(icedb::os_functions::getShareDir());
	const string sfile = sShareDir + "/examples/shapes/DDSCAT/ddscat6melting.dat";

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

int BOOST_TEST_CALL_DECL
main(int argc, char* argv[])
{
	try {
		// The icedb library needs to process its own options, and 
		// it needs to load its file-handling plugins.
		namespace po = boost::program_options;
		po::options_description desc("General options");
		icedb::add_options(desc, desc, desc); // Icedb has its own options.
		po::variables_map vm;
		po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
		po::notify(vm);
		icedb::process_static_options(vm);

		return ::boost::unit_test::unit_test_main(&init_unit_test, argc, argv);
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}
	catch (...)
	{
		std::cerr << "An unhandled exception has occurred." << std::endl;
		return 2;
	}
}
