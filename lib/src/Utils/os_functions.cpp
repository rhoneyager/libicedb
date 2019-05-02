#include "icedb/defs.h"
#include <mutex>
#include <map>
#include <boost/filesystem.hpp>
#include "BetterThrow/Error.hpp"
#include "BetterThrow/Info.hpp"
#include "icedb/Errors/error.hpp"
#include "icedb/misc/os_functions.h"
#include "icedb/misc/os_functions.hpp"
#include "icedb/Utils/dlls.hpp"
#ifdef _WIN32
# include <Windows.h>
#endif

namespace icedb {
	namespace os_functions {
		namespace vars {
			std::mutex m_sys;
			bool _consoleTerminated = false;
			/// Private flag that determines if the app waits for the user to press 'Enter' to terminate it at the end of execution.
			bool doWaitOnExit = false;
			bool doWaitOnExitQueriedDefault = false;
			std::map<System_String, std::string> sys_strings;
		}
	}
}
using namespace icedb::os_functions::vars;

ICEDB_BEGIN_DECL_C

bool ICEDB_waitOnExitGetDefault() {
	// windows
	// cmd.exe, bash.exe, tcsh.exe - no
	// others: yes
	// other os: no
	return false;
}
void ICEDB_waitOnExitSet(bool val) {
	std::lock_guard<std::mutex> lock(m_sys);
	doWaitOnExitQueriedDefault = true;
	doWaitOnExit = val;
}
bool ICEDB_waitOnExitGet() {
	std::lock_guard<std::mutex> lock(m_sys);
	if (!doWaitOnExitQueriedDefault) {
		doWaitOnExitQueriedDefault = true;
		doWaitOnExit = ICEDB_waitOnExitGetDefault();
	}
	return doWaitOnExit;
}

/**
* \brief Entry function that gets called when a debugged application first loads
*
* This function gets called at the beginning of an application's execution
* (generally). It:
* - determines if the app should wait on exit (to keep the console open)
* - resets the console title in case any other library overrides it.
*   A good example of this is the CERN ROOT image lobraries.
* - Overrides the console control key handlers on Windows. This lets a user
*   exit with CTRL-C without the debug code causing the app to crash.
*/
void ICEDB_libEntry() {}

void ICEDB_libExit() {}

void ICEDB_writeDebugString(const char* c) {
#ifdef _WIN32
	OutputDebugStringA(c);
#else
	// Just write to CERR
	ICEDB_COMPAT_fprintf_s(stderr, "%s", c);
#endif
}

ICEDB_DL void ICEDB_load()
{
	icedb::load();
}
ICEDB_DL void ICEDB_load_with_args(int argc, const char* const* argv)
{
	icedb::load(argc, argv);
}

ICEDB_END_DECL_C

namespace icedb {
	namespace os_functions {
		void libEntry() { ICEDB_libEntry(); }
		void waitOnExit(bool val) { ICEDB_waitOnExitSet(val); }
		bool waitOnExit() { return ICEDB_waitOnExitGet(); }
		void writeDebugString(const char* m) { ICEDB_writeDebugString(m); }

		std::string getSystemString(System_String name)
		{
			std::lock_guard<std::mutex> lock(m_sys);
			if (!vars::sys_strings.count(name)) throw BT_throw;
			return vars::sys_strings.at(name);
		}
		bool hasSystemString(System_String name)
		{
			std::lock_guard<std::mutex> lock(m_sys);
			if (!vars::sys_strings.count(name)) return false;
			return true;
		}
		void setSystemString(System_String name, const std::string& in)
		{
			std::lock_guard<std::mutex> lock(m_sys);
			vars::sys_strings[name] = in;
		}

		std::string getLibPath() {
			return BT::getModule<std::string>((void*)libEntry);
		}
		std::string getLibDir() {
			boost::filesystem::path libPath(getLibPath());
			return libPath.remove_filename().string();
		}
		std::string getAppPath() {
			auto pi = BT::ProcessInfo<std::string>::get<std::string>(BT::getPID());
			return pi.path;
		}
		std::string getAppDir() {
			boost::filesystem::path appPath(getAppPath());
			return appPath.remove_filename().string();
		}
	}
}

