#include <string>
#include <iostream>
#include <memory>
#include <exception>
#define BOOST_TEST_MODULE icedb-psu-shapes
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

BOOST_AUTO_TEST_CASE(read_psu_gmm)
{
	using namespace std;
	const string sShareDir(icedb::os_functions::getShareDir());
	const string sGMMfile = sShareDir + "/examples/shapes/PSU-GMM/"
		+ "psuaydinetal_geometry_aggregate_00004_GMM.nc";

	auto opts = icedb::registry::options::generate()->filename(sGMMfile)->filetype("psu");
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


/*
const string sOutFile = "";

HH::File file = HH::File::createFile(pToRaw.string().c_str(), H5F_ACC_TRUNC);


HH::File file = HH::File::openFile(sGMMfile.c_str(), H5F_ACC_RDONLY);
std::cout << "Using base group " << dbpath << std::endl;
if (!file.exists(dbpath.c_str())) ICEDB_throw(icedb::error::error_types::xCannotFindReference)
	.add("Reason", "Cannot find the shape in the input file.")
	.add("dbpath", dbpath)
	.add("Input-file", sFromRaw);
HH::Group basegrp = file.open(dbpath.c_str());
icedb::Shapes::Shape shp(basegrp.get());
//if (!shp.isShape()) ICEDB_throw(icedb::error::error_types::xPathExistsWrongType)
//	.add("Reason", "The object in the input file is not a valid shape.")
//	.add("dbpath", dbpath)
//	.add("Input-file", sFromRaw);
auto opts = icedb::registry::IO_options::generate();
opts->filename(sToRaw);
opts->filetype(outformat);
shp.write(nullptr, opts);
*/
