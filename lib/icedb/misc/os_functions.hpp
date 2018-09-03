#pragma once
#ifndef ICEDB_HPP_OS_FUNCTIONS
#define ICEDB_HPP_OS_FUNCTIONS
#include "../defs.h"
#include <iostream>
#include <memory>

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
		DL_ICEDB const char* getLibDir();
		DL_ICEDB std::string getLibPath();
		DL_ICEDB const char* getAppDir();
		DL_ICEDB const char* getAppPath();
		DL_ICEDB const char* getPluginDir();
		DL_ICEDB const char* getCWD();
		/// Get a the static share directory
		DL_ICEDB const char* getShareDir();

		struct processInfo;
		typedef const processInfo* hProcessInfo;
		struct moduleInfo;
		typedef const moduleInfo* hModuleInfo;

		DL_ICEDB hModuleInfo getModuleInfoP(void* func = nullptr);
		DL_ICEDB void freeModuleInfoP(hModuleInfo);
		inline std::shared_ptr<const moduleInfo> getModuleInfo(void* func = nullptr) {
			return std::shared_ptr<const moduleInfo>(getModuleInfoP(func), freeModuleInfoP);
		}

		DL_ICEDB hProcessInfo getInfoP(int pid);
		DL_ICEDB void freeProcessInfoP(hProcessInfo);
		inline std::shared_ptr<const processInfo> getInfo(int pid) {
			return std::shared_ptr<const processInfo>(getInfoP(pid), freeProcessInfoP);
		}

		DL_ICEDB const char* getPath(const hModuleInfo);
		DL_ICEDB const char* getName(const hProcessInfo);
		DL_ICEDB const char* getPath(const hProcessInfo);
		DL_ICEDB const char* getCwd(const hProcessInfo);
		DL_ICEDB const char* getEnviron(const hProcessInfo, size_t &sz);
		DL_ICEDB const char* getEnviron(const hProcessInfo, const char* varname);
		DL_ICEDB const char* getCmdline(const hProcessInfo, size_t &sz);
		DL_ICEDB const char* getStartTime(const hProcessInfo);
	}
}
ICEDB_END_DECL_CPP

#endif
