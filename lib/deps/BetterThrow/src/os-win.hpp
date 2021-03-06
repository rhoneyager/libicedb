#pragma once
#include <Windows.h>

namespace BT {
	/// This is the namespace containing all internal functions that use WINAPI to query the os.
	namespace win {
		::BT::Error_Res_t convertStr(LPCTSTR instr, std::string &out);
		::BT::Error_Res_t convertStr(LPCTSTR instr, std::wstring &out);

		::BT::Error_Res_t getPathWIN32(DWORD pid, ::BT::native_path_string_t& modPath);
		::BT::Error_Res_t getCWD(::BT::native_path_string_t& cwd);
		::BT::Error_Res_t getEnvironment(std::map<::BT::native_path_string_t, ::BT::native_path_string_t> &envs);
		::BT::Error_Res_t getCmdLine(std::vector<::BT::native_path_string_t> &cmdline);
		::BT::Error_Res_t getElevated(bool &elevated);
		::BT::Error_Res_t getStartTime(DWORD pid, ::BT::native_path_string_t &startTime);
		//::BT::Error_Res_t GetCurrentModule(hModule_t &hModule);
		//::BT::Error_Res_t getModulePath(hModule_t &mod, ::BT::native_path_string_t &modPath);
		::BT::Error_Res_t getModulePath(void* ptr, ::BT::native_path_string_t &modPath);
		::BT::Error_Res_t getLoadedModules(::BT::ModuleInfo_t<> &out);
		::BT::Error_Res_t getUserName(::BT::native_path_string_t &username);
		::BT::Error_Res_t getComputerName(::BT::native_path_string_t &username);
		::BT::Error_Res_t getAppConfigDir(::BT::native_path_string_t &appConfigDir);
		::BT::Error_Res_t getHomeDir(::BT::native_path_string_t &homeDir);
	}
}
