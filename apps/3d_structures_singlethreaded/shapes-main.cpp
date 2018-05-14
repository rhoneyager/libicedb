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
		po::options_description desc("Allowed options");
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
                /*std::cout<<data.required.number_of_particle_scattering_elements<<std::endl;
                //std::cout<<data.required.number_of_particle_constituents<<std::endl;
                //std::cout<<data.required.particle_scattering_element_coordinates_are_integral<<std::endl;
                //for (std::vector<float>::const_iterator i = data.required.particle_scattering_element_coordinates.begin(); i != data.required.particle_scattering_element_coordinates.end(); ++i) std::cout << *i << " ";
                //std::cout<<data.required.particle_id<<std::endl<<"Number constituent ";
                //for (std::vector<uint8_t>::const_iterator i = data.optional.particle_constituent_number.begin(); i != data.optional.particle_constituent_number.end(); ++i) std::cout << static_cast<int>(*i) << " ";
                //std::cout<<std::endl<<"whole: ";
                //for (std::vector<uint8_t>::const_iterator i = data.optional.particle_scattering_element_composition_whole.begin(); i != data.optional.particle_scattering_element_composition_whole.end(); ++i) std::cout << static_cast<int>(*i) << " ";
                //std::cout<<std::endl<<"num: ";
                //for (std::vector<uint64_t>::const_iterator i = data.optional.particle_scattering_element_number.begin(); i != data.optional.particle_scattering_element_number.end(); ++i) std::cout << static_cast<int>(*i) << " ";
                std::cout<<std::endl<<"End particle properties"<<std::endl;*/
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
