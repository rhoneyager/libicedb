#pragma once
#ifndef ICEDB_HPP_OS_FUNCTIONS
#define ICEDB_HPP_OS_FUNCTIONS
#include "defs.h"
#include <iostream>

ICEDB_BEGIN_DECL_CPP

namespace icedb {
	namespace os_functions {

		/// \see ICEDB_pidExists
		DL_ICEDB bool pidExists(int pid);
		/// \see ICEDB_getPID
		DL_ICEDB int getPID();
		/// \see ICEDB_getPPID
		DL_ICEDB int getPPID(int pid);
		/// \see ICEDB_libEntry
		DL_ICEDB void libEntry(int argc, char** argv);
		/// \see ICEDB_waitOnExitSet
		DL_ICEDB void waitOnExit(bool val);
		/// \see ICEDB_waitOnExitGet
		DL_ICEDB bool waitOnExit();
		/// \see ICEDB_getUserName
		DL_ICEDB const char* getUserName();
		/// \see ICEDB_getHostName
		DL_ICEDB const char* getHostName();
		/// \see ICEDB_getAppConfigDir
		DL_ICEDB const char* getAppConfigDir();
		/// \see ICEDB_getHomeDir
		DL_ICEDB const char* getHomeDir();

	}
}
ICEDB_END_DECL_CPP

#endif
