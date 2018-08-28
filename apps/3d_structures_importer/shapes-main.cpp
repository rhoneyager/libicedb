/** \brief 3d_structures_example program - Shows you how to write a shape
 *
 * This program reads shape files (in ADDA or DDSCAT formats) and writes
 * an HDF5/netCDF file as an output. The program demonstrates how to
 * create shapes using the library's C++ interface.
 *
 * Here's how the program works:
 * 1. Read the user options.
 * 2. Find valid shape files from the provided paths.
 * 2. Read each shape, then write to the output file.
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
#include <icedb/dlls.hpp>

namespace sfs = icedb::fs::sfs;

// A list of valid shape file extensions. These are used when matching valid files to read.
const std::map<std::string, std::set<sfs::path> > file_formats = {
	{"text", {".dat", ".shp", ".txt", ".shape", ".geom", ".adda"} },
	{"icedb", {".hdf5", ".nc", ".h5", ".cdf", ".hdf"} },
	{"psu", {".nc"}}
};

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
		po::options_description desc("General options"), mdata("Shape metadata"), input_matching("Input options"), constits("Constituents"), hidden("Hidden options");
		desc.add_options()
			("help,h", "produce help message")
			("out,o", po::value<string>(), "The path where the shape is written to")
			("db-path,p", po::value<string>()->default_value("shape"), "The path within the database to write to")
			("create", "Create the output database if it does not exist")
			("truncate", "Instead of opening existing output files in read-write mode, truncate them.")
			;
		input_matching.add_options()
			("in,i", po::value<vector<string> >()->multitoken(), "The paths where shapes are read from")
			// TODO: from-format currently only used for file selection. Pass the option to the plugin code to
			// select the right code path for each particular file format.
			("in-format,f", po::value<string>()->default_value("text"), "The format of the input files. Options: text, psu.")
			("in-nosearch", po::value<bool>()->default_value(false),
				"Set this option if you want to read in a set of files whose paths are exactly specified on the command line. "
				"This option allows for far greater control of input file selection.")
			("in-matching-extensions", po::value<vector<string> >()->multitoken(), 
				"Specify the extensions of files to match (e.g. .adda .shp) when searching for valid shapes. If not "
				"specified, then the defaults are: "
				"(text: .dat, .shp, .txt, .shape) (psu: .nc).")
			;
		mdata.add_options()
			("author", po::value<string>(), "Name(s) of the person/group who generated the shape.")
			("contact-information", po::value<string>(), "Affiliation, Contact information including email of the person/group who generated the scattering data.")
			("scattering-method", po::value<string>(), "Method applied to the shape to calculate the scattering properties.")
			("dataset-id", po::value<string>(), "The dataset id")
			("dataset-version-major", po::value<unsigned int>(), "Dataset version (major number)")
			("dataset-version-minor", po::value<unsigned int>(), "Dataset version (minor number)")
			("dataset-version-revision", po::value<unsigned int>(), "Dataset version (revision number)")
			("scattering_element_coordinates_scaling_factor", po::value<float>()->default_value(1.0f), "Scaling factor")
			("scattering_element_coordinates_units", po::value<string>()->default_value("m"), "Scaling factor units")
			;
		constits.add_options()
			("constituent-names", po::value<string>(), "Set the constituents. Pass in the format of \"NUM1-NAME1,NUM2-NAME2,NUM3-NAME3\"")
			;
		desc.add(mdata);
		desc.add(input_matching);
		desc.add(constits);
		icedb::add_options(desc, desc, hidden); // The library has its own options.
		desc.add(hidden);
		po::variables_map vm;
		po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
		po::notify(vm);
		icedb::handle_config_file_options(desc, vm); // Parse any options in a config file.
		

		auto doHelp = [&](const string& s)->void
		{
			cout << s << endl;
			cout << desc << endl;
			exit(1);
		};
		if (vm.count("help")) doHelp("");
		icedb::process_static_options(vm); // The library has its own options.
		if (!vm.count("in") || !vm.count("out")) doHelp("Need to specify in/out locations.");

		using namespace icedb;

		// namespace sfs defined for compatability. See <icedb/fs_backend.hpp>
		vector<string> vsFromRaw = vm["in"].as<vector<string> >();
		string sToRaw = vm["out"].as<string>();
		
		sfs::path pToRaw(sToRaw);
		string dbpath = vm["db-path"].as<string>();
		string informat = vm["in-format"].as<string>();
		bool from_nosearch = vm["in-nosearch"].as<bool>();
		vector<string> vCustomFileFormats;
		if (vm.count("in-matching-extensions"))
			vCustomFileFormats = vm["in-matching-extensions"].as<vector<string>>();
		std::set<sfs::path> customFileFormats;
		for (const auto &c : vCustomFileFormats) customFileFormats.insert(c);

		// Metadata
		string sAuthor, sContact, sScattMeth, sSFunits, sDatasetID;
		float sSFfactor = 1.0f;
		array<unsigned int, 3> version = { 1, 0, 0 };
		bool specifiedVersion = false;
		if (vm.count("author")) sAuthor = vm["author"].as<string>();
		if (vm.count("contact-information")) sContact = vm["contact-information"].as<string>();
		if (vm.count("scattering-method")) sScattMeth = vm["scattering-method"].as<string>();
		if (vm.count("dataset-id")) sDatasetID = vm["dataset-id"].as<string>();
		if (vm.count("dataset-version-major")) version[0] = vm["dataset-version-major"].as<unsigned int>();
		if (vm.count("dataset-version-minor")) version[1] = vm["dataset-version-minor"].as<unsigned int>();
		if (vm.count("dataset-version-revision")) version[2] = vm["dataset-version-revision"].as<unsigned int>();
		if (vm.count("dataset-version-major") || vm.count("dataset-version-minor") || vm.count("dataset-version-revision")) specifiedVersion = true;
		sSFfactor = vm["scattering_element_coordinates_scaling_factor"].as<float>();
		sSFunits = vm["scattering_element_coordinates_units"].as<string>();

		// Read in the constituents
		std::map<decltype(icedb::Shapes::NewShapeProperties::particle_constituents)::value_type::first_type, std::string> constit_ids;
		if (vm.count("constituent-names"))
		{
			string sConstits = vm["constituent-names"].as<std::string>();
			std::vector<std::string> vsconstits;
			icedb::splitSet::splitVector(sConstits, vsconstits, ',');
			for (const auto &c : vsconstits) {
				std::vector<std::string> vc;
				icedb::splitSet::splitVector(c, vc, '-');
				if (vc.size() != 2) ICEDB_throw(icedb::error::error_types::xBadInput)
					.add("Reason", "Trying to construct the constituent map, but encountered ill-formatted input")
					.add("Constituent-Names", sConstits)
					.add("Problem-Field", c);
				try {
					int c_id = boost::lexical_cast<int>(vc[0]);
					constit_ids[c_id] = vc[1];
				}
				catch (boost::bad_lexical_cast) {
					ICEDB_throw(icedb::error::error_types::xBadInput)
						.add("Reason", "Trying to construct the constituent map, but encountered ill-formatted input")
						.add("Constituent-Names", sConstits)
						.add("Problem-Field", c);
				}
			}
		}

		auto now = std::chrono::system_clock::now();
		auto in_time_t = std::chrono::system_clock::to_time_t(now);
		std::ostringstream ssIngestTime;
		// TODO: Replace gmtime with gmtime_s, but add the checks to see if it is supported on different compilers.
		// gmtime is in UTC. %Z would give the local timezone... Ingest time assumes UTC.
		ssIngestTime << std::put_time(std::gmtime(&in_time_t), "%Y-%m-%d %H:%M:%S"); // or "%c %Z"
		string sIngestTime = ssIngestTime.str();

		if (!sAuthor.size() || !sContact.size() || !sScattMeth.size() || !sDatasetID.size())
			cout << "Note: it is recommended that you set the metadata that describes the shapes that you are importing!" << endl;


		// Create the output file if it does not exist
		HH::File file(HH::HH_hid_t::dummy()); // Dummy parameter gets replaced always.
		if (vm.count("create")) {
			if (sfs::exists(pToRaw))
				ICEDB_throw(icedb::error::error_types::xFileExists)
				.add("Reason", "User specified to create a new file, but a file already exists at the specified path")
				.add("Filename", pToRaw.string());
			file = HH::File::createFile(pToRaw.string().c_str(), H5F_ACC_TRUNC);
		}
		else if (vm.count("truncate")) 
			file = HH::File::createFile(pToRaw.string().c_str(), H5F_ACC_TRUNC);
		else {
			if (!sfs::exists(pToRaw))
				file = HH::File::createFile(pToRaw.string().c_str(), H5F_ACC_TRUNC);
			else file = HH::File::openFile(pToRaw.string().c_str(), H5F_ACC_RDWR);
		}
		//Databases::Database::Database_ptr db = Databases::Database::openDatabase(pToRaw.string(), iof);
		std::cout << "Using base group " << dbpath << std::endl;
		HH::Group basegrp = HH::Handles::HH_hid_t::dummy();
		if (file.exists(dbpath.c_str())) basegrp = file.open(dbpath.c_str());
		else basegrp = file.create(dbpath.c_str(),
			HH::PL::PL::createLinkCreation().setLinkCreationPList(
				HH::Tags::PropertyLists::t_LinkCreationPlist(true))());

		// Read the input files and write to the output.
		for (const auto &sFromRaw : vsFromRaw)
		{
			sfs::path pFromRaw(sFromRaw);
			icedb::fs::impl::CollectedFilesRet_Type files;
			if (from_nosearch)
				files.push_back(std::pair<sfs::path, std::string>(sfs::path(sFromRaw), sFromRaw));
			else {
				if (customFileFormats.size() || !file_formats.count(informat))
					files = icedb::fs::impl::collectDatasetFiles(pFromRaw, customFileFormats);
				else
					files = icedb::fs::impl::collectDatasetFiles(pFromRaw, file_formats.at(informat));
			}

			for (const auto &f : files)
			{
				std::cerr << "Reading file " << f.first.string() << std::endl;
				std::vector<std::shared_ptr<icedb::Shapes::NewShapeProperties> > fileShapes;

				auto opts = registry::options::generate();
				opts->filename(f.first.string());
				if (informat.size()) opts->filetype(informat);
				// This function will open the file and read all valid shapes into the fileShapes structure.
				// The handling code is in io.hpp and registry.hpp.
				// The function automatically recognizes different file types.
				icedb::Shapes::NewShapeProperties::readVector(nullptr, opts, fileShapes);

				int shapeNum = 0;
				for (const auto &s : fileShapes) {
					// Set properties, but only if they are forced on the command line and are 
					// not provided by the readers.

					// Set a basic particle id. This id is used when writing the shape to the output file.
					// In this example, objects in the output file are named according to their ids.
					// TODO: What about numbered objects
					shapeNum++;
					std::cerr << "\tProcessing shape #" << shapeNum << " of " << fileShapes.size() << std::endl;
					if (!s->particle_id.size()) {
						std::ostringstream oParticle_Id;
						oParticle_Id << f.first.filename().string() << "-particle-" << shapeNum;
						s->particle_id = oParticle_Id.str();
					}

					// Set other shape properties based on program options.
					if (constit_ids.size()) {
						s->particle_constituents.clear();
						for (const auto &c : constit_ids) {
							s->particle_constituents.push_back(std::make_pair(c.first, c.second));
						}
					}

					if (!s->scattering_element_coordinates_scaling_factor)
						s->scattering_element_coordinates_scaling_factor = sSFfactor;
					if (!s->scattering_element_coordinates_units.size())
						s->scattering_element_coordinates_units = sSFunits;
					if (specifiedVersion) s->version = version;
					if (sDatasetID.size()) s->dataset_id = sDatasetID;
					if (sAuthor.size()) s->author = sAuthor;
					if (sContact.size()) s->contact = sContact;
					if (sScattMeth.size()) s->scattering_method = sScattMeth;

					// Write to the output file.
					std::cout << "\t\tWriting shape " << s->particle_id << std::endl;
					if (basegrp.exists(s->particle_id.c_str())) {
						std::cerr << "Warning: this shape already exists in the output file!!!!!! "
							"Skipping this shape's write step, as per-shape overwriting is not handled. If you meant to overwrite the "
							"output file, then provide the --truncate option to the program." << std::endl;
					}
					else {
						auto shp = icedb::Shapes::Shape::createShape(basegrp.get(), s->particle_id.c_str(), s.get());
						shp.atts.add<std::string>("date_of_icedb_ingest", { sIngestTime });
					}
				}
			}
		}
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
