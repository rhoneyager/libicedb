/** 3d_structures program - A program to read and write shape files
**/
// Boost bug with C++17 requires this define. See https://stackoverflow.com/questions/41972522/c2143-c2518-when-trying-to-compile-project-using-boost-multiprecision
#define _HAS_AUTO_PTR_ETC 1
#include <boost/program_options.hpp>
#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <vector>
#include <icedb/shape.hpp>
#include <icedb/Database.hpp>
#include <icedb/fs_backend.hpp>

/// A list of valid shapefile output formats
const std::set<std::string> valid_file_formats = { "ddscat", "raw", "icedb" };
const std::map<std::string, std::set<std::string> > file_formats = {
	{"ddscat", {".dat", ".shp"} },
	{"icedb", {".hdf5", ".nc", ".h5", ".cdf", ".hdf"} }
};

struct SplitPath {
	SplitPath(const sfs::path &src);
	sfs::path base;
	std::vector<sfs::path> splitOuter;
	sfs::path inner;
	std::vector<sfs::path> splitInner;
	bool exists() const;
	bool isFile() const;
	bool isDir() const;
};

int main(int argc, char** argv) {
	try {
		using namespace std;
		// Read program options
		
		namespace po = boost::program_options;
		po::options_description desc("Allowed options");
		desc.add_options()
			("help,h", "produce help message")
			("from", po::value<string>(), "The path where shapes are read from")
			("to", po::value<string>(), "The path where shapes are written to")
			//("input-format", po::value<string>()->default_value("ddscat"), "Specifies the input format");
			//("output-format", po::value<string>()->default_value("ddscat"), "Specifies the output format");
			;
		po::variables_map vm;
		po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
		po::notify(vm);

		auto doHelp = [&](const string& s)->void
		{
			cout << s << endl;
			cout << desc << endl;
			exit(1);
		};
		if (vm.count("help")) doHelp("");
		if (!vm.count("from") || !vm.count("to")) doHelp("Need to specify to/from locations.");
		string inFormat = vm["input-format"].as<string>();
		string outFormat = vm["output-format"].as<string>();
		if (!valid_file_formats.count(inFormat)) doHelp("Need to specify a valid input format.");
		if (!valid_file_formats.count(outFormat)) doHelp("Need to specify a valid output format.");

		using namespace icedb;
		// namespace sfs defined for compatability. See <icedb/fs_backend.hpp>
		sfs::path pFromRaw(vm["from"].as<string>());
		sfs::path pToRaw(vm["to"].as<string>());
		SplitPath splitFrom(pFromRaw);
		SplitPath splitTo(pToRaw);
		if (!splitFrom.exists()) doHelp("Cannot open the 'from' location for reading.");
		if (!splitTo.exists()) doHelp("Cannot open the 'to' location. It does not exist.");
		auto isDB = [](const std::string &format) {if (format == "icedb") return true; return false; };

		// I am reading the shape objects in sequence - read and write one object at a time.


		// For input files, check the base input type.
		// If database, call openDatabase and collect shape objects under sub path.
		// If not a database, collect valid files and open each separately.

		Databases::Database::Database_ptr db;
		if (isDB(inFormat)) { db = Databases::Database::openDatabase(splitFrom.base.string()); }
		else { db = Databases::Database::openVirtualDatabase(1024 * 1024 * 100); }

		//Databases::Database::openDatabase(splitFrom.base.string());
		//auto files = icedb::fs::impl::collectDatasetFiles()


		// For output file, check base output path.
		// If database and file:	create the file if it does not exist.
		//							If file does exist, open in either RW or TRUNCATE mode.
		// If database and folder:	run openDatabase. Virtual db root is read-only,
		//							but any valid sub-files are opened in either RW or TRUNCATE mode.
		// If not a database, must be a folder. CREATE (maybe TRUNCATE) output files.
		// For other conditions, fail with an error.
	}
	catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}
	catch (...) {
		std::cerr << "Unknown exception caught." << std::endl;
		return 1;
	}
	return 0;
}
