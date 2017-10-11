#include <iostream>
#include <cmath>
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
	std::shared_ptr<ICEDB_error_context> err(ICEDB_ct_error.getContextThreadLocal(), ICEDB_ct_error.contextFree);
	size_t errLen = ICEDB_ct_error.contextToCstrSize(err.get());
	char *msg = new char[errLen];
	ICEDB_ct_error.contextToCstr(err.get(), errLen, msg);
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
	map<uint64_t, shared_ptr<ICEDB_shape> > shapes;
	auto attrFuncs = ICEDB_funcs_attr_container; //ICEDB_attr_getContainerFunctions();
	auto tblFuncs = ICEDB_funcs_tbl_container; //ICEDB_tbl_getContainerFunctions();
	auto fsFuncs = ICEDB_funcs_fs;// ICEDB_fs_getContainerFunctions();
	auto shpFuncs = ICEDB_funcs_fs_shp; //ICEDB_shape_getContainerFunctions();

	if (vm.count("output")) sOutput = vm["output"].as<string>();
	vector<string> sInputs = vm["input"].as<vector<string>>();
	if (vm.count("print-id")) printID = true;
	if (vm.count("output-type")) sOutputType = vm["output-type"].as<string>();
	if (vm.count("output-plugin")) sOutputPlugin = vm["output-plugin"].as<string>();

	// Program options have been read. Time to loop over the input files and read them.

	for (const auto & in : sInputs) {
		cout << "File " << in << endl;
		size_t nShapes = 0;
		shared_ptr<ICEDB_shape** const> fileshapes(
			shpFuncs.openPathAll(
				in.c_str(), // This is the base path - every shape contained within this path will be read.
				ICEDB_path_iteration_recursive, // Read every shape
				ICEDB_flags_readonly, // No modifying the source files.
				&nShapes // Number of shapes read
			), shpFuncs.openPathAllFree); // Automatic deallocation
		if (!fileshapes) processError();
		// Iterate over all read shapes. For all unique (non-repeated) shapes, store pointers to them.
		for (size_t i = 0; i < nShapes; ++i) {
			uint64_t id = 0;
			if (!(id==(*fileshapes)[i]->funcs->getID((*fileshapes)[i]))) processError();
			ICEDB_shape* sshp = (*fileshapes)[i];
			if (printID) cout << "\t" << id << endl;
			if (!shapes.count(id)) {
				// COPY the shape into the map. fileshapes is managed by a separate memory manager.
				// The same backend can be used. No problems here.
				shared_ptr<ICEDB_shape> spsshp(
					sshp->funcs->clone(sshp),
					sshp->funcs->close);
				shapes[id] = spsshp;
				// Get the number of attributes and tables
				shared_ptr<ICEDB_fs_hnd> parentFS(
					sshp->funcs->getFSself(sshp), fsFuncs.closeHandle);
				if (!parentFS) processError();
				size_t numAtts = attrFuncs.count(parentFS.get(), &err);
				if (err) processError();
				size_t numTbls = tblFuncs.count(parentFS.get(), &err);
				if (err) processError();
				cout << "\t\tHas " << numAtts << " attributes and " << numTbls << " tables." << endl;
			}
			else if (printID) cout << "\t\tRepeated shape" << endl;
		}
	}

	// Now, process the unique files and execute an algorithm


	// If an output path is specified, write all unique shapes to this path.
	// TODO: handle any errors that occur!
	if (sOutput.size()) {
		// Open the output path. 
		shared_ptr<ICEDB_fs_hnd> p( // Encapsulating the opened file handle in a C++ shared_ptr that automatically closes the path when done.
			fsFuncs.openHandle(
				sOutput.c_str(), // Output file / folder name
				sOutputType.c_str(), // Type of output
				sOutputPlugin.c_str(), // Do not force any particular output plugin
				NULL, // No base handle
				ICEDB_flags_none), // No special i/o flags
			fsFuncs.closeHandle);
		if (!p) processError();
		// Copy each shape to the output file / folder.
		for (const auto & shp : shapes) {
			shared_ptr<ICEDB_shape> outshp(
				shp.second->funcs->copy_open(shp.second.get(), p.get()),
				ICEDB_funcs_shp_obj.close);
			if (!outshp) processError();

			// Examine the shape, and get the RMS distance from the center of each element of volume. Also determine the center of mass.
			size_t numPts = outshp->funcs->getNumPoints(outshp.get());
			shared_ptr<ICEDB_fs_hnd> fsobj(
				outshp->funcs->getFSself(outshp.get()), fsFuncs.closeHandle);
			bool ptsTblExists = tblFuncs.exists(fsobj.get(), "particle_scattering_element_coordinates", &err);
			if (err) processError();
			if (!ptsTblExists) continue;
			shared_ptr<ICEDB_tbl> tblPts(tblFuncs.open(fsobj.get(), "particle_scattering_element_coordinates"));
			if (!tblPts) processError();
			unique_ptr<float[]> ptArray(new float[numPts * 3]);
			if (!tblPts->funcs->readFull(tblPts.get(), ptArray.get())) processError();
			// The points are written x1, y1, z1, x2, y2, z2, ...
			// First, get the center of mass
			float means[3] = { 0,0,0 };
			float rms = 0;
			for (size_t i = 0; i < numPts; ++i) {
				means[0] += ptArray[(3 * i) + 0];
				means[1] += ptArray[(3 * i) + 1];
				means[2] += ptArray[(3 * i) + 2];
			}
			means[0] /= (float)numPts;
			means[1] /= (float)numPts;
			means[2] /= (float)numPts;

			for (size_t i = 0; i < numPts; ++i) {
				rms += std::pow<float>(ptArray[(3 * i)] - means[0], 2.f)
					+ std::pow<float>(ptArray[(3 * i) + 1] - means[1], 2.f)
					+ std::pow<float>(ptArray[(3 * i) + 2] - means[2], 2.f);
				means[0] += ptArray[(3 * i) + 0];
				means[1] += ptArray[(3 * i) + 1];
				means[2] += ptArray[(3 * i) + 2];
			}
			rms = std::sqrt(rms);

			// Write the rms value as a single-valued float.
			size_t sdims = 1;
			shared_ptr<ICEDB_attr> aRMS(
				attrFuncs.create(fsobj.get(), "RMS_mean", ICEDB_DATA_TYPES::ICEDB_TYPE_FLOAT, 1, &sdims),
				attrFuncs.close);
			// The following two lines are really the same. 
			*(aRMS->data.ft) = rms;
			//aRMS->_vptr->setData(aRMS.get(), &rms);
			aRMS->funcs->write(aRMS.get());

			size_t dims = 3;

			// Write the means as an table with 3 rows and 1 column.
			shared_ptr<ICEDB_tbl> tMeans(
				tblFuncs.create(fsobj.get(), "Means", ICEDB_DATA_TYPES::ICEDB_TYPE_FLOAT,
				1, &dims), tblFuncs.close);
			tMeans->funcs->writeFull(tMeans.get(), means);
		}
	}

	return 0;
}
