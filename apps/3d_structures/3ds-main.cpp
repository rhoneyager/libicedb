/** \brief 3d_structures program
**/

#include <icedb/defs.h>
#include <boost/program_options.hpp>
#include <iostream>

#include <fstream>
#include <boost/tokenizer.hpp>

#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <iomanip>
#include <icedb/shape.hpp>
#include <icedb/error.hpp>
#include <icedb/fs_backend.hpp>
#include <icedb/compat/HH/Groups.hpp>
#include <icedb/compat/HH/Files.hpp>
#include <icedb/splitSet.hpp>
#include <icedb/shape.hpp>
#include <icedb/dlls.hpp>

herr_t my_hdf5_error_handler(hid_t, void *)
{
	fprintf(stderr, "An HDF5 error was detected.Bye.\n");
	throw;
	exit(1);
	return 1;
}

int main(int argc, char** argv) {
	try {
		using namespace std;
		// Read program options
		//H5Eset_auto(H5E_DEFAULT, my_hdf5_error_handler, NULL); // For HDF5 error debugging
		namespace po = boost::program_options;
		po::options_description desc("General options"), input_matching("Input options"), hidden("Hidden options");
		desc.add_options()
			("help,h", "produce help message")
			("out,o", po::value<string>(), "The path where the shape is written to")
			("out-format,f", po::value<string>(), "Output file format")
			("create", "Create the output database if it does not exist")
			("truncate", "Instead of opening existing output files in read-write mode, truncate them.")
			;
		input_matching.add_options()
			("in,i", po::value<string >(), "The file where shapes are read from")
			("db-path,p", po::value<string> (), "The location of the shape in the input file.")
			;
		desc.add(input_matching);
		icedb::add_options(desc, desc, hidden);
		desc.add(hidden);
		po::variables_map vm;
		po::store(po::command_line_parser(argc, argv).options(desc).allow_unregistered().run(), vm);
		po::notify(vm);
		icedb::handle_config_file_options(desc, vm);

		auto doHelp = [&](const string& s)->void
		{
			cout << s << endl;
			cout << desc << endl;
			exit(1);
		};
		if (vm.count("help")) doHelp("");
		icedb::process_static_options(vm);

		if (!vm.count("in") || !vm.count("out") || !vm.count("db-path")
			|| !vm.count("out-format"))
			doHelp("Need to specify in, out, db-path, out-format.");

		using namespace icedb;

		// namespace sfs defined for compatability. See <icedb/fs_backend.hpp>
		string sFromRaw = vm["in"].as<string>();
		string sToRaw = vm["out"].as<string>();
		string dbpath = vm["db-path"].as<string>();
		string outformat = vm["out-format"].as<string>();

		HH::File file = HH::File::openFile(sFromRaw.c_str(), H5F_ACC_RDONLY);
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
	}
	// Ensure that unhandled errors are displayed before the application terminates.
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
