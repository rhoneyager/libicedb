/** \brief scattering_data_importer program - Imports scattering data
 *
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
#include <icedb/exv.hpp>

#include <icedb/plugin.hpp>

#include <HH/Files.hpp>
#include <HH/Groups.hpp>

int main(int argc, char** argv) {
	try {
		using namespace std;
		namespace po = boost::program_options;
		po::options_description desc("General options"), mdata("Shape metadata"), input_matching("Input options"), constits("Constituents"), hidden("Hidden");
		desc.add_options()
			("help,h", "produce help message")
			("output,o", po::value<string>(), "The path where the shape is written to")
			("db-path", po::value<string>()->default_value("run"), "The path within the database to write to")
			("create", "Create the output database if it does not exist")
			("truncate", "Instead of opening existing output files in read-write mode, truncate them.")
			;
		input_matching.add_options()
			("input,i", po::value<vector<string> >()->multitoken(), "The paths where shapes are read from")
			("in-format", po::value<string>()->default_value("ddscat"), "The format of the input files. Options: ddscat, adda, psu.")
			;
		mdata.add_options()
			("author", po::value<string>(), "Name(s) of the person/group who generated the shape.")
			("contact-information", po::value<string>(), "Affiliation, Contact information including email of the person/group who generated the scattering data.")
			("scattering-method", po::value<string>(), "Method applied to the shape to calculate the scattering properties.")
			("dataset-id", po::value<string>(), "The dataset id")
			("dataset-version-major", po::value<unsigned int>(), "Dataset version (major number)")
			("dataset-version-minor", po::value<unsigned int>(), "Dataset version (minor number)")
			("dataset-version-revision", po::value<unsigned int>(), "Dataset version (revision number)")
			//("scattering_element_coordinates_scaling_factor", po::value<float>()->default_value(1.0f), "Scaling factor")
			("scattering_element_coordinates_units", po::value<string>()->default_value("m"), "Scaling factor units")
			;
		constits.add_options()
			("constituent-names", po::value<string>(), "Set the constituents. Pass in the format of \"NUM1-NAME1,NUM2-NAME2,NUM3-NAME3\"")
			;
		desc.add(mdata);
		desc.add(input_matching);
		desc.add(constits);
		icedb::add_options(desc, desc, hidden);
		desc.add(hidden);
		po::variables_map vm;
		po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
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

		if (!vm.count("input") || !vm.count("output")) doHelp("Need to specify input/output locations.");

		using namespace icedb;

		// namespace sfs defined for compatability. See <icedb/fs_backend.hpp>
		vector<string> vsFromRaw = vm["input"].as<vector<string> >();
		string sToRaw = vm["output"].as<string>();

		namespace sfs = icedb::fs::sfs;
		sfs::path pToRaw(sToRaw);
		string dbpath = vm["db-path"].as<string>();
		//if (vm.count("resolution")) resolution_um = vm["resolution"].as<float>();
		string informat = vm["in-format"].as<string>();
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
		//sSFfactor = vm["scattering_element_coordinates_scaling_factor"].as<float>();
		sSFunits = vm["scattering_element_coordinates_units"].as<string>();

		// Read in the constituents
		std::map<int, std::string> constit_ids;
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
		//basegrp = db->createGroupStructure(dbpath);
	

		auto opts = icedb::registry::IO_options::generate();
		opts->filename(sToRaw);
		for (const auto &sFromRaw : vsFromRaw)
		{
			sfs::path pFromRaw(sFromRaw);
			std::string sFn = pFromRaw.filename().string();
			/*auto ddrun = icedb::io::ddscat::ddOutput::generate(sFromRaw);
			ddrun->version = version;
			ddrun->author = sAuthor;
			ddrun->contact = sContact;
			ddrun->scattMeth = sScattMeth;
			ddrun->datasetID = sDatasetID;
			ddrun->ingest_timestamp = sIngestTime;
			*/
			// The handle is set the first time we attempt to write a shape.
			//handle = ddrun->write(handle, opts->clone()->set("ID", sFn));
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
