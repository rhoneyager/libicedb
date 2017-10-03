#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <boost/program_options.hpp>
#include "../../libicedb/icedb/misc/os_functions.h"
#include "../../libicedb/icedb/data/attrs.h"
#include "../../libicedb/icedb/data/tables.h"
#include "../../libicedb/icedb/level_0/shape.h"

int main(int argc, char** argv) {
	using namespace std;
	ICEDB_libEntry(argc, argv);

	namespace po = boost::program_options;
	po::options_description desc("Allowed options");

	desc.add_options()
		("help,h", "produce help message")
		("input,i", po::value<vector<string> >()->multitoken(), "Input shape file(s)")
		("output,o", po::value<string>(), "Output file / folder")
		("output-type", po::value<string>()->default_value(""), "Type of output (optional / autodetected)")
		("output-plugin", po::value<string>()->default_value(""), "Plugin id used for output (optional)")
		;

	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).
		options(desc).run(), vm);
	po::notify(vm);

	auto doHelp = [&](const std::string& s)
	{
		cout << s << endl;
		cout << desc << endl;
		exit(3);
	};
	if (vm.count("help") || argc <= 1) doHelp("");
	if (!vm.count("input") || !vm.count("output")) doHelp("Must specify input file(s).");

	string sOutput, sOutputType, sOutputPlugin;
	ICEDB_error_code err;
	bool printHash = false;
	map<uint64_t, shared_ptr<ICEDB_SHAPE> > shapes;

	if (vm.count("output")) sOutput = vm["output"].as<string>();
	vector<string> sInputs = vm["input"].as<vector<string>>();
	if (vm.count("print-hash")) printHash = true;
	if (vm.count("output-type")) sOutputType = vm["output-type"].as<string>();
	if (vm.count("output-plugin")) sOutputPlugin = vm["output-plugin"].as<string>();

	for (const auto & in : sInputs) {
		cout << "File " << in << endl;
		size_t nShapes = 0;
		ICEDB_SHAPE_p **fileshapes = nullptr; // TODO: Set this?
		ICEDB_SHAPE_open_path_all(
			in.c_str(), // This is the base path - every shape contained within this path will be read.
			ICEDB_path_recursive, // Read every shape
			ICEDB_flags_readonly, // No modifying the source files.
			&nShapes, // Number of shapes read
			fileshapes, // The shapes
			&err); // Presents an error code on error.
		// Iterate over all read shapes. For all unique (non-repeated) shapes, store pointers to them.
		for (size_t i = 0; i < nShapes; ++i) {
			ICEDB_HASH_t hash;
			(*fileshapes)[i]->_vptrs->getHash((*fileshapes)[i], &hash);
			shared_ptr<ICEDB_SHAPE> sshp((*fileshapes)[i], ICEDB_SHAPE_destroy); // Auto-destructs shapes if not needed.
			if (printHash) cout << "\t" << hash.low << endl;
			if (!shapes.count(hash.low)) {
				shapes[hash.low] = sshp;
			}
			else if (printHash) cout << "\t\tRepeated shape" << endl;
		}
		ICEDB_SHAPE_open_path_all_free(fileshapes);
	}
	

	// If an output path is specified, write all unique shapes to this path.
	// TODO: handle any errors that occur!
	if (sOutput.size()) {
		// Open the output path. 
		shared_ptr<ICEDB_fs_hnd> p( // Encapsulating the opened file handle in a C++ shared_ptr that automatically closes the path when done.
			ICEDB_path_open(
				sOutput.c_str(), // Output file / folder name
				sOutputType.c_str(), // Type of output
				sOutputPlugin.c_str(), // Do not force any particular output plugin
				NULL, // No base handle
				ICEDB_flags_none, // No special i/o flags
				&err), // Write any error code to err
			[](ICEDB_fs_hnd_p p) {ICEDB_fh_close(p, NULL); });
		// Copy each shape to the output file / folder.
		for (const auto & shp : shapes) {
			shp.second->_vptrs->copy(shp.second.get(), p.get(), &err);
		}
	}

	return 0;
}
