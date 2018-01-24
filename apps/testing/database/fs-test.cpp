// Always include this first
#include <icedb/defs.h>

#include <boost/program_options.hpp>
#include <iostream>
#include <string>
#include <vector>

#include <icedb/fs.hpp>
#include <icedb/Database.hpp>
#include <icedb/shape.hpp>

int main(int argc, char** argv) {
	using namespace std;
	// These statements create and parse program options for the test program:
	namespace po = boost::program_options;
	po::options_description desc("Allowed options");

	desc.add_options()
		("help,h", "produce help message")
		("dbpath,p", po::value<string>()->default_value("testdataset"), "Database path")
		("create", "Create a sample database at this location")
		("rw", "Specifies that the database should be opened in read-write mode")
		;

	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).
		options(desc).run(), vm);
	po::notify(vm);

	auto doHelp = [&](const std::string& s)->void
	{
		cout << s << endl;
		cout << desc << endl;
		exit(1);
	};

	if (vm.count("help")) doHelp("");

	string dbpath = vm["dbpath"].as<string>();

	// Now for the main part of the program: creating a sample database:
	if (vm.count("create")) {
		std::cout << "Trying to create a sample database at " << dbpath << std::endl;

		// BASIC NAVIGATION
		// ------------------

		// Fiest, let's call Database::createSampleDatabase.
		// The createSampleDatabase function creates several folders and files, under a 
		// base directory located at 'dbpath'. These files, combined, form a database.
		//
		// After creating the sample database files, createSampleDatabase calls
		// openDatabase. This function loops through all directories, HDF5 and netCDF
		// files and unifies them into a single virtual database. See index.hdf5 to get 
		// a view of how these files all come together.
		std::unique_ptr<icedb::Databases::Database> db
			= icedb::Databases::Database::createSampleDatabase(dbpath);
		// Here, the variable 'db' is a smart pointer to the loaded database.
		// A smart pointer is a special type of C++ class that automatically
		// frees (i.e. closes and deallocates) the database when it is no longer
		// used. 
		// The Database class definition is located in Database.hpp.

		// From here, we can start to manipulate the database structure.
		// Here is how to open a netCDF / HDF5 group.
		std::unique_ptr<icedb::Groups::Group> grpMain = db->openGroup("Testing");
		// For ease of typing, you could also just type:
		//auto grpMain = db->openGroup("Testing");
		// The createSampleDatabase function created a folder, "Testing", which
		// contains the "scratch.hdf5" file.
		// The openDatabase command represents the "Testing" folder as a Group
		// (see icedb::Groups::Group in Group.hpp).
		// The "scratch.hdf5" file is mounted in the virtual database under
		// "Testing/scratch.hdf5". All objects placed this group are written to that
		// hdf5 file.
		auto grpScratch = grpMain->openGroup("scratch.hdf5");

		// Let's create another group, this time within scratch.hdf5.
		auto grpTest1 = grpScratch->createGroup("Obj_1");

		// Groups can have Attributes, which are small blobs of data.
		// Attributes have a name, dimensions, and a datatype.
		// Here is how you can define an attribute:
		icedb::Attributes::Attribute<double> attrDouble1(
			"TestDouble1", { 1 }, { 2.0 });
		// This creates an attribute, called "TestDouble1", with a double type,
		// expressed as a one-dimensional array with a single value (1) in this dimension,
		// with a value of 2.0.
		// We can write this attribute to the Obj_1 group using:
		grpTest1->writeAttribute(attrDouble1);

		// In HDF5, attributes can be multi-dimensional, but in netCDF, they may
		// have only a single dimension. Here is how to define a single-dimensional
		// vector of four floating-point values.
		icedb::Attributes::Attribute<float> attrFloat2(
			"TestFloat2",
			{ 4 },
			{ 1.0f, 2.0f, 2.5f, 3.0f });
		grpTest1->writeAttribute(attrFloat2);

		// A multi-dimensional attribute may be defined this way:
		icedb::Attributes::Attribute<float> attrFloat3(
			"TestFloat3",
			{ 2, 3 },
			{ 1.0f, 2.0f, 3.1f, 4.2f, 5.3f, 6.0f});
		// Note: since we are sticking to the netCDF-4 standard, we wil not write this
		// variable to the file. If we did, then the file would not be intelligible
		// to the netCDF libraries and the nc_dump program. HDF5 and HDFview would
		// still be able to read it, though.
		//grpTest1->writeAttribute(attrFloat3);

		// Also note, there is a difference between a vector (a one-dimensional array)
		// and a two-dimensional array that has one of the dimension lengths set to one.
		// For example, here is how to define a 1x3 matrix. It will not work with netCDF.
		icedb::Attributes::Attribute<float> attrFloat4(
			"TestFloat4",
			{ 1, 3 },
			{ 4.2f, 5.3f, 6.0f });
		//grpTest1->writeAttribute(attrFloat4);

		// For ease of typing, we can write attributes more easily using lines like these:
		//grpTest1->writeAttribute(icedb::Attributes::Attribute<uint64_t>("TestInt5", { 1 }, { 65536 }));
		// or
		//grpTest1->writeAttribute<uint64_t>("TestInt5", {1}, {65536});

		// Attributes can be floats, doubles, signed 64-bit integers (int64_t), unsigned 64-bit integers (uint64_t),
		// and strings of characters (std::string).
		grpTest1->writeAttribute(icedb::Attributes::Attribute<std::string>("TestString6", "Test string 6"));
		// Any other types are invalid. I might extend the interface to support these later on, but there isn't
		// much need to do so.

		// Currently, there is one restriction for writing strings: only a single string can be written to
		// an attribute. So, this statement will not work:
		//grpTest1->writeAttribute(icedb::Attributes::Attribute<std::string>("TestStringSet7", { 2 }, { "Test string 7", "TS 8" }));
		// This will eventually be fixed.

		// Of course, you will also want to be able to read attributes. The syntax is similar.
		icedb::Attributes::Attribute<std::string> readTS6 = grpTest1->readAttribute<std::string>("TestString6");
		std::cout << "We just read TestString6 = " << readTS6.data[0] << std::endl;



		{
			// Groups can also have Tables (a.k.a. DataSets in HDF5 / Variables in NetCDF).
			// Unlike attributes, table objects are much larger and are not automatically
			// loaded into memory.

			// Creating a table is easy:
			auto tblTest = grpTest1->createTable<int64_t>("Test_table_1", { 2, 3 }, { -2, -1, 0, 1, 2, 3 });
			// The above command just made a table with two rows and three columns.
			// The data are inserted into the table in row-major form.
			// See Table.hpp to list the methods for adding larger data into tables.
			// For these other methods, the sizing of the table may be de-coupled
			// from writing data to the tables.

			// Under their current implementation, table dimensions are entirely fixed and are
			// set at creation. In the future, support for extendible dimensions will be added.

			// Tables support the same data types as attributes (uint64_t, int64_t, float, double).
			// Strings are on the to-do list.
			// Unlike with attributes, tables can be entirely multi-dimensional.

			// Tables can have attributes.
			tblTest->writeAttribute<int64_t>(icedb::Attributes::Attribute<int64_t>("TestInt7", 5));
			tblTest->writeAttribute<int64_t>(icedb::Attributes::Attribute<int64_t>("TestInt8", { 4 }, { 1, -1, 2, -2 }));

			// For compatability with netCDF, each data table must be associated with one
			// or more "dimension scales". To create and associate these scales with a table,
			// commands like these are needed:
			auto tblDims1 = grpTest1->createTable<int64_t>("X_axis", { 3 }, { 9, 10, 11 });
			auto tblDims2 = grpTest1->createTable<int64_t>("Y_axis", { 2 }, { 4, 6 });
			tblDims1->setDimensionScale("X_axis");
			tblDims2->setDimensionScale("Y_axis");
			tblTest->attachDimensionScale(0, tblDims2.get());
			tblTest->attachDimensionScale(1, tblDims1.get());

			// I recommend that you open scratch.hdf5 in nc_dump or HDFview to see the resulting attributes
			// and tables that we just created.
			
		}


		// SHAPE FILES AND DATA STRUCTURES
		// --------------------------------
		{
			// Shapes are special collections of tables and attributes. Let's make a naive shape,
			// with only a few dipoles and a single substance.
			auto grpTest2 = grpScratch->createGroup("Obj_Shape_2");
			// Shapes have required and optional objects
			icedb::Shapes::NewShapeRequiredProperties shpRequired;
			icedb::Shapes::NewShapeCommonOptionalProperties shpCommonOptional;

			constexpr uint64_t numPoints = 8;
			constexpr uint64_t numSubstances = 1;
			constexpr uint64_t numAxes = 3;
			//constexpr std::array<uint64_t,numPoints> point_ids = { 0, 1, 2, 3, 4, 5, 6, 7 }; // eight points
			//constexpr std::array<uint64_t,numSubstances> composition_ids = { 0 };
			constexpr std::array<float, numPoints * numAxes> points = {
			//const std::vector<float> points = {
				1, 1, 1,
				1, 1, 2,
				1, 1, 3,
				2, 2, 2,
				2, 2, 3,
				3, 2, 2,
				3, 2, 3,
				3, 3, 2
			};
			//constexpr std::array<float, numPoints * numSubstances> point_compositions = { 1, 1, 1, 1, 1, 1, 1, 1 };
			
			// Required attributes
			shpRequired.particle_id = "000001";
			// Required dimensions
			shpRequired.number_of_particle_scattering_elements = numPoints;
			shpRequired.number_of_particle_constituents = numSubstances;
			// Required variables
			shpRequired.particle_scattering_element_coordinates = points;
			
			shpCommonOptional.particle_scattering_element_spacing = static_cast<float>(40. * 1e-6);

			// Validate and write out any errors.
			shpRequired.isValid(&cerr);
			shpCommonOptional.isValid(&shpRequired, &cerr);

			auto shp = icedb::Shapes::Shape::createShape(
				*(grpTest2.get()), "Test_shape", shpRequired.particle_id,
				&shpRequired, &shpCommonOptional);
			//icedb::Shapes::Shape::createShape(grpTest2,)
		}
		
		// INDEXING THE DATABASE OBJECTS
		// -------------------------------
		// The indexDatabase command will generate an index file that points the
		// location to all of the shapes and scattering objects. To be implemented.
		icedb::Databases::Database::indexDatabase(dbpath);
	} else {
		std::cout << "Trying to open the database at " << dbpath << std::endl;
		std::cout << "Note: This will fail if the path does not exist. "
		       "If this is the case, either run with --create as an option, "
		       "or check that the database path (--dbpath) is correct." << std::endl;
		icedb::fs::IOopenFlags flags = icedb::fs::IOopenFlags::READ_ONLY;
		if (vm.count("rw")) flags = icedb::fs::IOopenFlags::READ_WRITE;
		auto db = icedb::Databases::Database::openDatabase(dbpath, flags);
	}

	return 0;
}
