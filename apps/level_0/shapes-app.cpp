#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <boost/program_options.hpp>
#include "../../libicedb/icedb/misc/os_functions.h"
#include "../../libicedb/icedb/data/attrs.h"
#include "../../libicedb/icedb/data/tables.h"
#include "../../libicedb/icedb/level_0/shape.h"

/// This small block of code is called if an error is encountered by the library.
/// It reads the error message, prints it, and terminates the program.
void processError() {
	auto errFuncs = ICEDB_error_getContainerFunctions();
	std::shared_ptr<ICEDB_error_context> err(errFuncs->getContextThreadLocal(), errFuncs->contextFree);
	size_t errLen = errFuncs->contextToCstrSize(err.get());
	char *msg = new char[errLen];
	errFuncs->contextToCstr(err.get(), errLen, msg);
	std::cerr << msg << std::endl;
	delete[] msg;
	exit(4);
};

int main(int argc, char** argv) {
	using namespace std;
	// This function always should be called on application startup. The library needs to perform some startup tasks.
	ICEDB_libEntry(argc, argv);

	// The C++ boost::program_options library is used here because it makes it really easy to parse command-line options.
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

	auto doHelp = [&](const std::string& s)->void
	{
		cout << s << endl;
		cout << desc << endl;
		exit(3);
	};
	
	if (vm.count("help") || argc <= 1) doHelp("");
	if (!vm.count("input") || !vm.count("output")) doHelp("Must specify input file(s).");

	string sOutput, sOutputType, sOutputPlugin;
	ICEDB_error_code err;
	bool printID = false;
	map<uint64_t, shared_ptr<ICEDB_SHAPE> > shapes;
	auto attrFuncs = ICEDB_ATTR_getContainerFunctions();
	auto tblFuncs = ICEDB_TBL_getContainerFunctions();
	auto fsFuncs = ICEDB_fs_getContainerFunctions();
	auto shpFuncs = ICEDB_SHAPE_getContainerFunctions();

	if (vm.count("output")) sOutput = vm["output"].as<string>();
	vector<string> sInputs = vm["input"].as<vector<string>>();
	if (vm.count("print-id")) printID = true;
	if (vm.count("output-type")) sOutputType = vm["output-type"].as<string>();
	if (vm.count("output-plugin")) sOutputPlugin = vm["output-plugin"].as<string>();

	// Program options have been read. Time to loop over the input files and read them.

	for (const auto & in : sInputs) {
		cout << "File " << in << endl;
		size_t nShapes = 0;
		// Each input file might contain zero, one or more shapes. openPathAll reads them all.
		shared_ptr<ICEDB_SHAPE** const> fileshapes(
			shpFuncs->openPathAll(
				in.c_str(), // This is the base path - every shape contained within this path will be read.
				ICEDB_path_iteration_recursive, // Read every shape
				ICEDB_flags_readonly, // No modifying the source files.
				&nShapes // Number of shapes read
				), shpFuncs->openPathAllFree); // Automatic deallocation
		if (!fileshapes) processError();
		// Iterate over all read shapes. For all unique (non-repeated) shapes, store pointers to them.
		for (size_t i = 0; i < nShapes; ++i) {
			uint64_t id = 0;
			if (!(*fileshapes)[i]->funcs->getID((*fileshapes)[i], &id)) processError();
			ICEDB_SHAPE* sshp = (*fileshapes)[i];
			//shared_ptr<ICEDB_SHAPE> sshp((*fileshapes)[i], shpFuncs->close); // Auto-destructs shapes if not needed.
			if (printID) cout << "\t" << id << endl;
			if (!shapes.count(id)) {
				// COPY the shape into the map. fileshapes is managed by a separate memory manager.
				// The same backend can be used. No problems here.
				shared_ptr<ICEDB_SHAPE> spsshp(sshp->funcs->copy_open(sshp, sshp->funcs->getParent(sshp)));
				shapes[id] = spsshp;
				// Get the number of attributes and tables
				shared_ptr<ICEDB_fs_hnd> parentFS(sshp->funcs->getParent(sshp), fsFuncs->close);
				if (!parentFS) processError();
				size_t numAtts = attrFuncs->count(parentFS.get(), &err);
				if (err) processError();
				size_t numTbls = tblFuncs->count(parentFS.get(), &err);
				if (err) processError();
				cout << "\t\tHas " << numAtts << " attributes and " << numTbls << " tables." << endl;
			}
			else if (printID) cout << "\t\tRepeated shape" << endl;
		}
	}
	

	// If an output path is specified, write all unique shapes to this path.
	// TODO: handle any errors that occur!
	if (sOutput.size()) {
		// Open the output path. 
		shared_ptr<ICEDB_fs_hnd> p( // Encapsulating the opened file handle in a C++ shared_ptr that automatically closes the path when done.
			fsFuncs->open(
				sOutput.c_str(), // Output file / folder name
				sOutputType.c_str(), // Type of output
				sOutputPlugin.c_str(), // Do not force any particular output plugin
				NULL, // No base handle
				ICEDB_flags_none), // No special i/o flags
			fsFuncs->close);
		if (!p) processError();
		// Copy each shape to the output file / folder.
		for (const auto & shp : shapes) {
			if (!shp.second->funcs->copy(shp.second.get(), p.get())) processError();
		}
	}

	return 0;
}
