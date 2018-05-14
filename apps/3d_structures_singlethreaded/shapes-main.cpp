/** \brief 3d_structures_example program - Shows you how to write a shape
 *
 * This program reads shape files (in ADDA or DDSCAT formats) and writes
 * an HDF5/netCDF file as an output. The program demonstrates how to
 * create shapes using the library's C++ interface.
 *
 * Here's how the program works:
 * 1. Read the user options.
 * 2. Read the shape.
 * 3. Write the shape.
**/

#include <icedb/defs.h>
#include <boost/program_options.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <iomanip>
#include <icedb/shape.hpp>
#include <icedb/Database.hpp>
#include <icedb/error.hpp>
#include <icedb/fs_backend.hpp>
#include "shape.hpp"
#include "shapeIOtext.hpp"

// A list of valid shapefile output formats
const std::map<std::string, std::set<sfs::path> > file_formats = {
	{"text", {".dat", ".shp", ".txt", ".shape"} },
	{"icedb", {".hdf5", ".nc", ".h5", ".cdf", ".hdf"} },
	{"psu", {".nc"}}
};

// These get set in main(int,char**).
float resolution_um = 0; ///< The resolution of each shape lattice, in micrometers.

icedb::Groups::Group::Group_ptr basegrp; ///< Shapes get written to this location in the output database.

int main(int argc, char** argv) {
	try {
		using namespace std;
		// Read program options

		namespace po = boost::program_options;
		po::options_description desc("General options"), mdata("Shape metadata");
		desc.add_options()
			("help,h", "produce help message")
			("from", po::value<vector<string> >()->multitoken(), "The paths where shapes are read from")
			("to", po::value<string>(), "The path where the shape is written to")
			("db-path", po::value<string>()->default_value("shape"), "The path within the database to write to")
			("create", "Create the output database if it does not exist")
			("resolution", po::value<float>(), "Lattice spacing for the shape, in um")
			("truncate", "Instead of opening existing output files in read-write mode, truncate them.")
			("from-format", po::value<string>()->default_value("text"), "The format of the input files. Options: text, psu.")
			;
		mdata.add_options()
			("author", po::value<string>(), "Name(s) of the person/group who generated the shape.")
			("contact-information", po::value<string>(), "Affiliation, Contact information including email of the person/group who generated the scattering data.")
			("scattering-method", po::value<string>(), "Method applied to the shape to calculate the scattering properties.")
			;
		desc.add(mdata);
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
		vector<string> vsFromRaw = vm["from"].as<vector<string> >();
		string sToRaw = vm["to"].as<string>();
		
		sfs::path pToRaw(sToRaw);
		string dbpath = vm["db-path"].as<string>();
		if (vm.count("resolution")) resolution_um = vm["resolution"].as<float>();
		string informat = vm["from-format"].as<string>();

		// Metadata
		string sAuthor, sContact, sScattMeth;
		if (vm.count("author")) sAuthor = vm["author"].as<string>();
		if (vm.count("contact-information")) sContact = vm["contact-information"].as<string>();
		if (vm.count("scattering-method")) sScattMeth = vm["scattering-method"].as<string>();
		auto now = std::chrono::system_clock::now();
		auto in_time_t = std::chrono::system_clock::to_time_t(now);
		std::ostringstream ssIngestTime;
		// TODO: Replace gmtime with gmtime_s, but add the checks to see if it is supported on different compilers.
		ssIngestTime << std::put_time(std::gmtime(&in_time_t), "%Y-%m-%d %H:%M:%S %Z"); // or "%c %Z"
		string sIngestTime = ssIngestTime.str();

		if (!sAuthor.size() || !sContact.size() || !sScattMeth.size())
			cout << "Note: it is recommended that you set the metadata that describes the shapes that you are importing!" << endl;


		// Create the output database if it does not exist
		auto iof = fs::IOopenFlags::READ_WRITE;
		if (vm.count("create")) iof = fs::IOopenFlags::CREATE;
		if (vm.count("truncate")) iof = fs::IOopenFlags::TRUNCATE;
		if (!sfs::exists(pToRaw)) iof = fs::IOopenFlags::CREATE;
		Databases::Database::Database_ptr db = Databases::Database::openDatabase(pToRaw.string(), iof);
		std::cout << "Creating base group " << dbpath << std::endl;
		basegrp = db->createGroupStructure(dbpath);
	
		// Changes start here
		for (const auto &sFromRaw : vsFromRaw)
		{
			sfs::path pFromRaw(sFromRaw);
			auto files = icedb::fs::impl::collectDatasetFiles(pFromRaw, file_formats.at(informat));
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
				if (resolution_um)
					data.optional.particle_scattering_element_spacing = resolution_um / 1.e6f;

				// Writing the shape to the HDF5/netCDF file

				std::cout << "Creating group " << data.required.particle_id << std::endl;
				auto shpgrp = basegrp->createGroup(data.required.particle_id);
				std::cout << "Writing shape " << data.required.particle_id << std::endl;
				auto shp = data.toShape(data.required.particle_id, shpgrp->getHDF5Group());

				// Apply metadata
				if (sAuthor.size()) shp->writeAttribute<std::string>("author", { 1 }, { sAuthor });
				if (sContact.size()) shp->writeAttribute<std::string>("contact_information", { 1 }, { sContact });
				if (sScattMeth.size()) shp->writeAttribute<std::string>("scattering_method", { 1 }, { sScattMeth });
				shp->writeAttribute<std::string>("date_of_icedb_ingest", { 1 }, { sIngestTime });
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
