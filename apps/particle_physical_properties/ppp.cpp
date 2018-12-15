/// \brief This is a program to calculate particle physical properties.

#include <icedb/defs.h>
#include <boost/program_options.hpp>
#include <fstream>
#include <iostream>
#include <icedb/dlls.hpp>
#include <icedb/error.hpp>
#include <icedb/fs.hpp>

int main(int argc, char** argv) { 
	try {
		using namespace std;
		// Read program options
		namespace po = boost::program_options;
		po::options_description desc("General options"), 
			input_opts("Input options"), 
			hidden("Hidden options"), 
			alg_opts("Algorithm options"),
			output_opts("Output options"),
			oall("All options");
		desc.add_options()
			("help,h", "produce help message")
			;
		input_opts.add_options()
			("in-shapes,i", po::value<vector<string>>(), "The input shape paths")
			("in-ppps", po::value<vector<string>>(), "Optional input particle physical properties. "
				"When calculating new properties, some calculations may be previously computed.")
			("recursion", po::value<string>()->default_value("one"), "Recursion level when finding shapes (none, one, subtree)")
			;
		alg_opts.add_options()
			("target-properties,t", po::value<vector<string>>(), "A space or comma-separated list of properties "
				"that you want to calculate.")
			;
		output_opts.add_options()
			("out,o", po::value<string>(), "The output base path. "
				"For each input shape, will create a child file/group with the appropriate particle id. "
				"If no output is specified, write to console.")
			("out-mode", po::value<string>()->default_value("default"), "The output mode. "
				"Default is to create a file if it does not exist, and open in "
				"read/write mode if it does exist. Options are \"default\", \"truncate\", \"create\", \"rw\".")
			("write-derived-properties", po::value<bool>()->default_value(true), "When deriving "
				"properties, should this program also write ancillary properties that are calculated in the derivation?")
			;
		desc.add(input_opts);
		desc.add(alg_opts);
		desc.add(output_opts);
		icedb::add_options(desc, desc, hidden);
		oall.add(desc);
		oall.add(hidden);
		po::variables_map vm;
		po::store(po::command_line_parser(argc, argv).options(oall).allow_unregistered().run(), vm);
		po::notify(vm);
		icedb::handle_config_file_options(oall, vm);

		auto doHelp = [&](const string& s)->void
		{
			cout << s << endl;
			cout << desc << endl;
			exit(1);
		};
		if (vm.count("help")) doHelp("");
		icedb::process_static_options(vm);

		
		
		using namespace icedb;
		if (!vm.count("in"))
			doHelp("Need to specify input file.");

		vector<string> vInputs = vm["in"].as<vector<string>>();
		auto recursionType = icedb::fs::getRecursionType(vm["recursion"].as<string>());
		if (recursionType == icedb::fs::RecursionType::UNKNOWN) doHelp("The recursion option is invalid.");
		auto outType = icedb::fs::getIOType(vm["out-mode"].as<string>());
		if (outType == icedb::fs::IOopenFlags::UNKNOWN) doHelp("Unrecognized output mode.");
		string sOutput;
		if (vm.count("out")) sOutput = vm["out"].as<string>();

		// Iterate over all of the input paths and recurse.
		// Each input path can contain two parts: a mandatory filesystem portion (a path on the disk)
		// and an optional HDF5 group (a path within the HDF5 file). The inputs will probably be 
		// manipulated by the shell globbing options, so we won't attempt to process them here.
		// What we will do, however, is expand the paths and test each path element to see if it is 
		// a symbolic link, a directory, or a file.
		// If we hit a path component that is a file and it has subpaths after it, then we assume
		// that we have an HDF5 file and that these subpaths are HDF5 groups. We do the same
		// type of processing. We go through each group and optionally follow symbolic, hard and 
		// "external" links until we find the base path.
		// Once the base path is found, we will follow the recursion policy to detect all matching 
		// particles. 
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
