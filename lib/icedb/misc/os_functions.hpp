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
		ICEDB_DL bool pidExists(int pid);
		/// \see ICEDB_getPID
		ICEDB_DL int getPID();
		/// \see ICEDB_getPPID
		ICEDB_DL int getPPID(int pid);
		/// \see ICEDB_libEntry
		ICEDB_DL void libEntry(int argc, char** argv);
		/// \see ICEDB_waitOnExitSet
		ICEDB_DL void waitOnExit(bool val);
		/// \see ICEDB_waitOnExitGet
		ICEDB_DL bool waitOnExit();
		/// \see ICEDB_getUserName
		ICEDB_DL std::string getUserName();
		/// \see ICEDB_getHostName
		ICEDB_DL std::string getHostName();
		/// \see ICEDB_getAppConfigDir
		ICEDB_DL std::string getAppConfigDir();
		/// \see ICEDB_getHomeDir
		ICEDB_DL std::string getHomeDir();
		ICEDB_DL std::string getLibDir();
		ICEDB_DL std::string getLibPath();
		ICEDB_DL std::string getAppDir();
		ICEDB_DL std::string getAppPath();
		ICEDB_DL std::string getPluginDir();
		ICEDB_DL std::string getCWD();
		/// Get a the static share directory
		ICEDB_DL std::string getShareDir();

		struct processInfo;
		typedef const processInfo* hProcessInfo;
		struct moduleInfo;
		typedef const moduleInfo* hModuleInfo;

		ICEDB_DL hModuleInfo getModuleInfoP(void* func = nullptr);
		ICEDB_DL void freeModuleInfoP(hModuleInfo);
		inline std::shared_ptr<const moduleInfo> getModuleInfo(void* func = nullptr) {
			return std::shared_ptr<const moduleInfo>(getModuleInfoP(func), freeModuleInfoP);
		}

		ICEDB_DL hProcessInfo getInfoP(int pid);
		ICEDB_DL void freeProcessInfoP(hProcessInfo);
		inline std::shared_ptr<const processInfo> getInfo(int pid) {
			return std::shared_ptr<const processInfo>(getInfoP(pid), freeProcessInfoP);
		}

		ICEDB_DL std::string getPath(const hModuleInfo);
		ICEDB_DL std::string getName(const hProcessInfo);
		ICEDB_DL std::string getPath(const hProcessInfo);
		ICEDB_DL std::string getCwd(const hProcessInfo);
		ICEDB_DL std::string getEnviron(const hProcessInfo, size_t &sz);
		ICEDB_DL std::string getEnviron(const hProcessInfo, const std::string &varname);
		ICEDB_DL std::string getCmdline(const hProcessInfo, size_t &sz);
		ICEDB_DL std::string getStartTime(const hProcessInfo);
	}
}
ICEDB_END_DECL_CPP

#endif
