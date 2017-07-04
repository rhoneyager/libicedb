#pragma once
#ifndef ICEDB_H_OS_FUNCTIONS
#define ICEDB_H_OS_FUNCTIONS
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
		/// \see ICEDB_getConcurrentThreadsSupported
		DL_ICEDB int getConcurrentThreadsSupported();
		/// \see ICEDB_appEntry
		DL_ICEDB void appEntry();
		/// \see ICEDB_waitOnExitSet
		DL_ICEDB void waitOnExit(bool val);
		/// \see ICEDB_waitOnExitGet
		DL_ICEDB bool waitOnExit();
		/// \see ICEDB_waitOnExitForce
		DL_ICEDB bool waitOnExitForce();
		/// \see ICEDB_appExit
		DL_ICEDB void appExit();
		/// \brief Enumerate loaded libraries for a process/
		/// \param pid is the process id. On many systems, the
		/// only valid value is the current process, when not root.
		/// \param out is the output stream.
		DL_ICEDB void enumModules(int pid, std::ostream &out = std::cerr);


	}
}
ICEDB_END_DECL_CPP

#endif
