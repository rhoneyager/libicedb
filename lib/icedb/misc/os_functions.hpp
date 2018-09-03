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
		DL_ICEDB std::string getUserName();
		/// \see ICEDB_getHostName
		DL_ICEDB std::string getHostName();
		/// \see ICEDB_getAppConfigDir
		DL_ICEDB std::string getAppConfigDir();
		/// \see ICEDB_getHomeDir
		DL_ICEDB std::string getHomeDir();
		DL_ICEDB std::string getLibDir();
		DL_ICEDB std::string getLibPath();
		DL_ICEDB std::string getAppDir();
		DL_ICEDB std::string getAppPath();
		DL_ICEDB std::string getPluginDir();
		DL_ICEDB std::string getCWD();
		/// Get a the static share directory
		DL_ICEDB std::string getShareDir();

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

		DL_ICEDB std::string getPath(const hModuleInfo);
		DL_ICEDB std::string getName(const hProcessInfo);
		DL_ICEDB std::string getPath(const hProcessInfo);
		DL_ICEDB std::string getCwd(const hProcessInfo);
		DL_ICEDB std::string getEnviron(const hProcessInfo, size_t &sz);
		DL_ICEDB std::string getEnviron(const hProcessInfo, const std::string &varname);
		DL_ICEDB std::string getCmdline(const hProcessInfo, size_t &sz);
		DL_ICEDB std::string getStartTime(const hProcessInfo);
	}
}
ICEDB_END_DECL_CPP

#endif
