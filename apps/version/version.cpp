/** \brief 3d_structures program
**/

#include <icedb/defs.h>
#include <boost/program_options.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <icedb/error.hpp>
#include <icedb/dlls.hpp>
#include <icedb/versioning/versioning.hpp>
#include <icedb/misc/os_functions.hpp>
#include <BetterThrow/Info.hpp>

int main(int argc, char** argv) {
	try {
		using namespace std;
		// Read program options
		//H5Eset_auto(H5E_DEFAULT, my_hdf5_error_handler, NULL); // For HDF5 error debugging
		namespace po = boost::program_options;
		po::options_description desc("General options"), input_matching("Input options"), hidden("Hidden options"), oall("All options");
		desc.add_options()
			;
		icedb::add_options(desc, desc, hidden);
		oall.add(desc);
		oall.add(hidden);
		po::variables_map vm;
		po::store(po::command_line_parser(argc, argv).options(oall).allow_unregistered().run(), vm);
		po::notify(vm);
		icedb::handle_config_file_options(oall, vm);
		icedb::process_static_options(vm);

		using namespace icedb::versioning;
		auto libver = getLibVersionInfo();
		cout << "icedb\n--------------------------------\n";
		debug_preamble(*(libver.get()), std::cout);
		using namespace icedb::os_functions;
		auto pi = BT::ProcessInfo<std::string>::get<std::string>(BT::getPID());
		auto rt = BT::RuntimeInfo<std::string>::get<std::string>();
		cout << "App dir: " << getAppDir() << endl;
		cout << "Lib dir: " << getLibDir() << endl;
		//cout << "Plugin dir: " << getPluginDir() << endl;
		cout << "CWD: " << pi.cwd << endl;
		//cout << "Share dir: " << getShareDir() << endl;
		//cout << "User config dir: " << getAppConfigDir() << endl;
		cout << "icedb lib path: " << getLibPath() << endl << endl;
		cout << "Loaded modules:" << endl;

		icedb::registry::list_loaded_modules(cout);
	}
	catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}
	catch (const char* e) {
		std::cerr << "Exception: " << e << std::endl;
		return 1;
	}
	catch (...) {
		std::cerr << "Unknown exception caught." << std::endl;
		return 1;
	}
	return 0;
}
