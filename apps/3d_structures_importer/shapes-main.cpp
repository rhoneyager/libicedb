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
#include "shape.hpp"
#include "shapeIOtext.hpp"

// A list of valid shapefile output formats
const std::map<std::string, std::set<sfs::path> > file_formats = {
	{"text", {".dat", ".shp", ".txt", ".shape", ".geom", ".adda"} },
	{"icedb", {".hdf5", ".nc", ".h5", ".cdf", ".hdf"} },
	{"psu", {".nc"}}
};

// These get set in main(int,char**).
float resolution_um = 0; ///< The resolution of each shape lattice, in micrometers.

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
		po::options_description desc("General options"), mdata("Shape metadata"), input_matching("Input options");
		desc.add_options()
			("help,h", "produce help message")
			("config-file", po::value<string>(), "Read a file containing program options, such as metadata. Options are specified, once per line, as OPTION=VALUE pairs.")
			("to", po::value<string>(), "The path where the shape is written to")
			("db-path", po::value<string>()->default_value("shape"), "The path within the database to write to")
			("create", "Create the output database if it does not exist")
			("resolution", po::value<float>(), "Lattice spacing for the shape, in um")
			("truncate", "Instead of opening existing output files in read-write mode, truncate them.")
			;
		input_matching.add_options()
			("from", po::value<vector<string> >()->multitoken(), "The paths where shapes are read from")
			("from-format", po::value<string>()->default_value("text"), "The format of the input files. Options: text, psu.")
			("from-nosearch", po::value<bool>()->default_value(false),
				"Set this option if you want to read in a set of files whose paths are exactly specified on the command line. "
				"This option allows for far greater control of input file selection.")
			("from-matching-extensions", po::value<vector<string> >()->multitoken(), 
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
		desc.add(mdata);
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
		if (!vm.count("from") || !vm.count("to")) doHelp("Need to specify to/from locations.");

		using namespace icedb;

		// namespace sfs defined for compatability. See <icedb/fs_backend.hpp>
		vector<string> vsFromRaw = vm["from"].as<vector<string> >();
		string sToRaw = vm["to"].as<string>();
		
		sfs::path pToRaw(sToRaw);
		string dbpath = vm["db-path"].as<string>();
		//if (vm.count("resolution")) resolution_um = vm["resolution"].as<float>();
		string informat = vm["from-format"].as<string>();
		bool from_nosearch = vm["from-nosearch"].as<bool>();
		vector<string> vCustomFileFormats;
		if (vm.count("from-matching-extensions"))
			vCustomFileFormats = vm["from-matching-extensions"].as<vector<string>>();
		std::set<sfs::path> customFileFormats;
		for (const auto &c : vCustomFileFormats) customFileFormats.insert(c);

		// Metadata
		string sAuthor, sContact, sScattMeth, sSFunits, sDatasetID;
		float sSFfactor = 1.0f;
		array<unsigned int, 3> version = { 1, 0, 0 };
		if (vm.count("author")) sAuthor = vm["author"].as<string>();
		if (vm.count("contact-information")) sContact = vm["contact-information"].as<string>();
		if (vm.count("scattering-method")) sScattMeth = vm["scattering-method"].as<string>();
		if (vm.count("dataset-id")) sDatasetID = vm["dataset-id"].as<string>();
		if (vm.count("dataset-version-major")) version[0] = vm["dataset-version-major"].as<unsigned int>();
		if (vm.count("dataset-version-minor")) version[1] = vm["dataset-version-minor"].as<unsigned int>();
		if (vm.count("dataset-version-revision")) version[2] = vm["dataset-version-revision"].as<unsigned int>();
		sSFfactor = vm["scattering_element_coordinates_scaling_factor"].as<float>();
		sSFunits = vm["scattering_element_coordinates_units"].as<string>();

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
		if (vm.count("create"))
			file = HH::File::createFile(pToRaw.string().c_str(), H5F_ACC_CREAT);
		else if (vm.count("truncate")) 
			file = HH::File::createFile(pToRaw.string().c_str(), H5F_ACC_TRUNC);
		else {
			if (!sfs::exists(pToRaw))
				file = HH::File::openFile(pToRaw.string().c_str(), H5F_ACC_CREAT);
			else file = HH::File::openFile(pToRaw.string().c_str(), H5F_ACC_RDWR);
		}
		//Databases::Database::Database_ptr db = Databases::Database::openDatabase(pToRaw.string(), iof);
		std::cout << "Creating base group " << dbpath << std::endl;
		// TODO: Make this call easier to invoke.
		// TODO: property list passing should not need the final () - change signature and required type.
		HH::Group basegrp = file.create(dbpath.c_str(),
			HH::PL::PL::createLinkCreation().setLinkCreationPList(
				HH::Tags::PropertyLists::t_LinkCreationPlist(true))());
		//basegrp = db->createGroupStructure(dbpath);
	
		for (const auto &sFromRaw : vsFromRaw)
		{
			sfs::path pFromRaw(sFromRaw);
			icedb::fs::impl::CollectedFilesRet_Type files;
			if (from_nosearch)
				files.push_back(std::pair<sfs::path, std::string>(sfs::path(sFromRaw), sFromRaw));
			else {
				if (customFileFormats.size())
					files = icedb::fs::impl::collectDatasetFiles(pFromRaw, customFileFormats);
				else
					files = icedb::fs::impl::collectDatasetFiles(pFromRaw, file_formats.at(informat));
			}
			for (const auto &f : files)
			{
				// Reading the shape from the text file
				icedb::Examples::Shapes::ShapeDataBasic data;
				if (informat == "text")
					data = icedb::Examples::Shapes::readTextFile(f.first.string());
				else if (informat == "psu")
					data = icedb::Examples::Shapes::readPSUfile(f.first.string());
				else ICEDB_throw(icedb::error::error_types::xBadInput)
					.add("Description", "Unknown input file format. See program help for a list of valid formats.")
					.add("Current-format", informat);

				// Set a basic particle id. This id is used when writing the shape to the output file.
				// In this example, objects in the output file are named according to their ids.
				if (data.required.particle_id.size() == 0)
					data.required.particle_id = f.first.filename().string();
				if (data.optional.particle_constituent_name.size() == 0)
					data.optional.particle_constituent_name = { "ice" };
				if (data.optional.particle_constituent_number.size() == 0)
					data.optional.particle_constituent_number = { 1 };
				data.optional.scattering_element_coordinates_scaling_factor = sSFfactor;
				data.optional.scattering_element_coordinates_units = sSFunits;
				data.required.version = version;
				data.required.dataset_id = sDatasetID;
				data.required.author = sAuthor;
				data.required.contact = sContact;
				data.optional.scattering_method = sScattMeth;
				
				// Writing the shape to the HDF5/netCDF file

				std::cout << "Creating group " << data.required.particle_id << std::endl;
				//auto shpgrp = basegrp.create(data.required.particle_id.c_str());
				std::cout << "Writing shape " << data.required.particle_id << std::endl;
				icedb::Shapes::Shape shp = data.toShape(basegrp.get(), data.required.particle_id);

				shp.atts.add<std::string>("date_of_icedb_ingest", { sIngestTime });
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
