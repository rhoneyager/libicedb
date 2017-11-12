// Boost bug with C++17 requires this define. See https://stackoverflow.com/questions/41972522/c2143-c2518-when-trying-to-compile-project-using-boost-multiprecision
#define _HAS_AUTO_PTR_ETC 1
#include <boost/program_options.hpp>
#include <iostream>
#include <string>
#include <vector>

#include "../../../icedb/fs.hpp"
#include "../../../icedb/hdf5_supplemental.hpp"
#include "../../../private/fs_backend.hpp"
#include "../../../icedb/Database.hpp"

int main(int argc, char** argv) {
	using namespace std;
	namespace po = boost::program_options;
	po::options_description desc("Allowed options");

	desc.add_options()
		("help,h", "produce help message")
		("dbpath,p", po::value<string>()->default_value("testdataset"), "Database path")
		("create", "Create a database at this location")
		("index", "Create index files for a database at this location")
		("rw", "Specifies that the database should be opened in read-write mode")
		("test", "Run test routines and create a database")
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
	if (vm.count("test")) {
		auto db = icedb::Databases::Database::createDatabase(dbpath);
		
		auto grpMain = db->openGroup("Testing");
		
		auto grpScratch = grpMain->openGroup("scratch.hdf5");
		auto grpTest1 = grpScratch->createGroup("Obj_1");
		icedb::Attributes::Attribute<float> attrFloat1(
			"TestFloat1",
			{ 4 },
			{ 1.0f, 2.0f, 2.5f, 3.0f });
		grpTest1->writeAttribute(attrFloat1);
		icedb::Attributes::Attribute<float> attrFloat2(
			"TestFloat2",
			{ 2, 3 },
			{ 1.0f, 2.0f, 3.1f, 4.2f, 5.3f, 6.0f});
		//grpTest1->writeAttribute(attrFloat2);

		icedb::Attributes::Attribute<float> attrFloat3(
			"TestFloat3",
			{ 1, 3 },
			{ 4.2f, 5.3f, 6.0f });
		//grpTest1->writeAttribute(attrFloat3);

		//grpTest1->writeAttribute(icedb::Attributes::Attribute<uint64_t>("TestInt1", { 1 }, { 65536 }));

		grpTest1->writeAttribute(icedb::Attributes::Attribute<std::string>("TestString1", "Test string 1"));
		//grpTest1->writeAttribute(icedb::Attributes::Attribute<std::string>("TestStringSet2", { 2 }, { "Test string 2", "TS 3" }));

		auto grpTest1HDFobj = grpTest1->getHDF5Group();

		//H5::Attribute attr = grpTest1HDFobj->createAttribute(0)
		icedb::fs::hdf5::addAttr<double, H5::Group>(grpTest1HDFobj.get(), "TestDoubleSingle1", 3.14159);
		icedb::fs::hdf5::addAttr<std::string, H5::Group>(grpTest1HDFobj.get(), "TestString4", "TS4");


		icedb::Attributes::Attribute<std::string> vTS1 = grpTest1->readAttribute<std::string>("TestString1");
		std::cout << vTS1.data[0] << std::endl;

		auto tblDims1 = grpTest1->createTable<int64_t>("X_axis", { 3 }, { 9, 10, 11 });
		auto tblDims2 = grpTest1->createTable<int64_t>("Y_axis", { 2 }, { 4, 6 });
		tblDims1->setDimensionScale("X_axis");
		tblDims2->setDimensionScale("Y_axis");
		auto tblTest = grpTest1->createTable<int64_t>("Test_table_1", { 2, 3 }, { -2, -1, 0, 1, 2, 3 });
		tblTest->attachDimensionScale(0, tblDims2.get());
		tblTest->attachDimensionScale(1, tblDims1.get());
		
		tblTest->writeAttribute<int64_t>(icedb::Attributes::Attribute<int64_t>("Test5", 5));
		tblTest->writeAttribute<int64_t>(icedb::Attributes::Attribute<int64_t>("Test6", { 4 }, { 1, -1, 2, -2 }));


		icedb::Databases::Database::indexDatabase(dbpath);
	} else if (vm.count("create") || !sfs::exists(sfs::path(dbpath))) {
		if (sfs::exists(sfs::path(dbpath))) doHelp(
			"Cannot create a database where one already exists.");
		icedb::Databases::Database::createDatabase(dbpath);
	} else {
		icedb::fs::IOopenFlags flags = icedb::fs::IOopenFlags::READ_ONLY;
		if (vm.count("rw")) flags = icedb::fs::IOopenFlags::READ_WRITE;
		auto db = icedb::Databases::Database::openDatabase(dbpath, flags);
	}
	if (vm.count("index"))
		icedb::Databases::Database::indexDatabase(dbpath);

	return 0;
}