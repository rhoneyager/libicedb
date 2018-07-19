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
		po::options_description desc("General options"), input_matching("Input options");
		desc.add_options()
			("help,h", "produce help message")
			("config-file", po::value<string>(), "Read a file containing program options, such as metadata. Options are specified, once per line, as OPTION=VALUE pairs.")
			("to", po::value<string>(), "The path where the shape is written to")
			("to-format", po::value<string>(), "Output file format")
			("create", "Create the output database if it does not exist")
			("truncate", "Instead of opening existing output files in read-write mode, truncate them.")
			;
		input_matching.add_options()
			("from", po::value<string >(), "The file where shapes are read from")
			("db-path", po::value<string> (), "The location of the shape in the input file.")
			;
		desc.add(input_matching);
		po::variables_map vm;
		po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
		po::notify(vm);
		if (vm.count("config-file")) {
			using namespace std;
			string configfile = vm["config-file"].as<string>();
#if BOOST_VERSION < 104600
			// For RHEL's really old Boost distribution
			ifstream ifs(configfile.c_str());
			if (!ifs) {
				cout << "Could no open the response file\n";
				return 1;
			}
			// Read the whole file into a string
			stringstream ss;
			ss << ifs.rdbuf();
			// Split the file content
			using namespace boost;
			char_separator<char> sep(" \n\r");
			tokenizer<char_separator<char> > tok(ss.str(), sep);
			vector<string> args;
			copy(tok.begin(), tok.end(), back_inserter(args));
			// Parse the file and store the options
			po::store(po::command_line_parser(args).options(desc).run(), vm);
#else
			// For modern systems
			po::store(po::parse_config_file<char>(configfile.c_str(), desc, false), vm);
#endif
			po::notify(vm);
		}
		

		auto doHelp = [&](const string& s)->void
		{
			cout << s << endl;
			cout << desc << endl;
			exit(1);
		};
		if (vm.count("help")) doHelp("");
		if (!vm.count("from") || !vm.count("to") || !vm.count("db-path")
			|| !vm.count("to-format"))
			doHelp("Need to specify from, to, db-path, to-format.");

		using namespace icedb;

		// namespace sfs defined for compatability. See <icedb/fs_backend.hpp>
		string sFromRaw = vm["from"].as<string>();
		string sToRaw = vm["to"].as<string>();
		string dbpath = vm["db-path"].as<string>();
		string outformat = vm["to-format"].as<string>();

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
		icedb::registry::loadDLL("C:\\Users\\ryan.ADHONEYAGER\\source\\repos\\libicedb\\build\\Debug\\icedb-plugins\\plugin_silo.dll");
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
