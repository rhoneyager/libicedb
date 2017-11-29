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
#include "shape.hpp"
#include "shapeIOtext.hpp"

/// A list of valid shapefile output formats
const std::set<std::string> valid_file_formats = { "ddscat", "raw", "icedb" };
const std::map<std::string, std::set<sfs::path> > file_formats = {
	{"ddscat", {".dat", ".shp", ".txt"} },
	{"icedb", {".hdf5", ".nc", ".h5", ".cdf", ".hdf"} }
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
			("db-folder", po::value<string>()->default_value("shapes"), "The path within the database to write to")
			("create", "Create the output database if it does not exist")
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
		
		using namespace icedb;
		// namespace sfs defined for compatability. See <icedb/fs_backend.hpp>
		string sFromRaw = vm["from"].as<string>();
		string sToRaw = vm["to"].as<string>();
		sfs::path pFromRaw(sFromRaw);
		sfs::path pToRaw(sToRaw);
		string dbfolder = vm["db-folder"].as<string>();

		// Create the output database if it does not exist
		auto iof = fs::IOopenFlags::READ_WRITE;
		if (vm.count("create")) iof = fs::IOopenFlags::CREATE;
		Databases::Database::Database_ptr db = Databases::Database::openDatabase(pToRaw.string(), iof);
		auto basegrp = db->createGroupStructure(dbfolder);
		auto files = icedb::fs::impl::collectDatasetFiles(pFromRaw, file_formats.at("ddscat"));

		for (const auto &f : files) {
			auto data = icedb::Examples::Shapes::readTextFile(f.first.string());
			data.required.particle_id = f.first.filename().string();
			auto sgrp = basegrp->createGroup(f.second);
			auto shp = data.toShape(f.first.filename().string(), sgrp->getHDF5Group());
		}
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
