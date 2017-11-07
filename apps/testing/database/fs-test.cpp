#include <boost/program_options.hpp>
#include <iostream>
#include <string>
#include <vector>

#include "../../../icedb/shape.hpp"
#include "../../../icedb/fs.hpp"
#include "../../../icedb/hdf5_supplemental.hpp"
#include "../../../private/fs_backend.hpp"

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
	if (vm.count("create")) {
		if (sfs::exists(sfs::path(dbpath))) doHelp(
			"Cannot create a database where one already exists.");
		icedb::fs::Database::createDatabase(dbpath);
	} else {
		icedb::fs::IOopenFlags flags = icedb::fs::IOopenFlags::READ_ONLY;
		if (vm.count("rw")) flags = icedb::fs::IOopenFlags::READ_WRITE;
		auto db = icedb::fs::Database::openDatabase(dbpath, flags);
	}
	if (vm.count("index"))
		icedb::fs::Database::indexDatabase(dbpath);

	return 0;
}