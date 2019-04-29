#pragma once
#ifndef ICEDB_HPP_OS_FUNCTIONS
#define ICEDB_HPP_OS_FUNCTIONS
#include "../defs.h"
#include <string>

namespace boost {
	namespace program_options {
		class options_description;
		class variables_map;
	}
}

ICEDB_BEGIN_DECL_CPP

namespace icedb {
	namespace os_functions {
		ICEDB_DL void libEntry(); ///< \see ICEDB_libEntry
		ICEDB_DL void waitOnExit(bool val); ///< \see ICEDB_waitOnExitSet
		ICEDB_DL bool waitOnExit(); ///< \see ICEDB_waitOnExitGet
		ICEDB_DL void writeDebugString(const char*);

		enum class System_String {
			SHARE_DIR,
			BUILD_DIR ///< Only set in testing code
		};
		ICEDB_DL bool hasSystemString(System_String name);
		ICEDB_DL std::string getSystemString(System_String name);
		ICEDB_DL void setSystemString(System_String name, const std::string& in);
		ICEDB_DL std::string getLibDir();
		ICEDB_DL std::string getAppDir();
		//ICEDB_DL std::string getPluginDir();
		ICEDB_DL std::string getLibPath();
		ICEDB_DL std::string getAppPath();
	}

	// There are two ways to initialize icedb.
	// Either just call load() or do the
	// add_options, handle_config_file_options, process_static options loop.

	/// Parse program options and initialize the library.
	ICEDB_DL void load();
	ICEDB_DL void load(int argc, const char* const* argv);
	/// Add program options
	ICEDB_DL void add_options(
		boost::program_options::options_description& cmdline,
		boost::program_options::options_description& config,
		boost::program_options::options_description& hidden);
	/// Handle config file options
	ICEDB_DL void handle_config_file_options(
		boost::program_options::options_description&,
		boost::program_options::variables_map&);
	/// Parse program options and initialize the library.
	ICEDB_DL void process_static_options(
		boost::program_options::variables_map& vm);
}
ICEDB_END_DECL_CPP

#endif
