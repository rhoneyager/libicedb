#if defined(_WIN32)
#define _BIND_TO_CURRENT_VCLIBS_VERSION 1
#include <Windows.h>
#include <ShlObj.h>
#include <TlHelp32.h>
#include <Psapi.h>
#pragma comment(lib, "Psapi")
#pragma comment(lib, "Ws2_32")
#pragma comment(lib, "Advapi32")
#pragma comment(lib, "Shell32")
#endif
#include <cstdint>
#include <memory>
#include <mutex>

#include "../include/BetterThrow/Error.hpp"
#include "../include/BetterThrow/Info.hpp"
#include "os-win.hpp"

namespace BT {
	namespace win {
		/// Windows Unicode paths can be much longer than the old 260 character limit.
		const int MAX_UNICODE_PATH = 32767;
		const bool throw_on_win_function_failures = true; ///< For debugging.
#define BT_WIN_CHECK(x) \
			if ((x).first && !throw_on_win_function_failures) return BT::getOSerror(); \
			else if ((x).first && throw_on_win_function_failures) BT_throw \
				.add("OS-Error", x.second) \
				.add("errno", x.first)
#define BT_WIN_CHECK_ERRNO(x) \
			if ((x) && !throw_on_win_function_failures) return BT::getOSerrno(x); \
			else if ((x) && throw_on_win_function_failures) BT_throw \
				.add("OS-Error", ::BT::getOSerrno(x).second) \
				.add("errno", x)
#define BT_WIN_CHECK_OSERROR(x) \
			if ((x) && !throw_on_win_function_failures) return BT::getOSerror(); \
			else if ((x) && throw_on_win_function_failures) BT_throw \
				.add("OS-Error", ::BT::getOSerror().second) \
				.add("OS-Error-Number", ::BT::getOSerror().first)
#define BT_WIN_SUCCESS ::BT::getOSerrno(0);

		/// \brief This is a class to support RAII for WinAPI handles. They now get 
		/// automatically released when we leave scope.
		template <typename T, typename Deleter_t = BOOL(*)(T)>
		class win_handle
		{
			T _val;
			//typedef void(*Deleter_t)(T);
			Deleter_t *_deleter;
			typedef bool(*Validator_t)(T);
			Validator_t _validator;
		public:
			static bool valid_general(T vv) { return vv != nullptr; }
			static bool valid_handle(T vv) {
				if (!vv) return false;
				if (vv == INVALID_HANDLE_VALUE) return false;
				return true;
			}

			win_handle() : _val(nullptr), _deleter(nullptr) {}
			win_handle(T v, Deleter_t *d, Validator_t val = valid_general) : _val(v), _deleter(d), _validator(val) {}
			win_handle(win_handle<T> &&src) : _val(src._val), _deleter(src._deleter), _validator(src._validator) { src.release(); }
			win_handle<T, Deleter_t> & operator=(const win_handle<T, Deleter_t>&) = delete;
			win_handle<T, Deleter_t>(const win_handle<T, Deleter_t>&) = delete;

			void reset(T v) {
				if (valid() && *_deleter) (*_deleter)(_val);
				_val = v;
			}
			~win_handle() {
				if (valid() && *_deleter) (*_deleter)(_val);
			}
			bool valid() const { return _validator(_val); }
			//operator !() const { return _val == nullptr; }
			T release() { T sv = _val; _val = nullptr; return sv; }
			T get() const { return _val; }
			void swap(win_handle<T, Deleter_t> &src) {
				auto sd = src._deleter;
				T sv = src._val;
				Validator_t svvt = src._validator;
				src._deleter = _deleter;
				src._val = _val;
				src._validator = _validator;
				_val = sv;
				_deleter = sd;
				_validator = svvt;
			}


		};

		/// A specialization of win_handle for HMODULEs.
		typedef win_handle<HMODULE, decltype(FreeLibrary)> hModule_t;

		/// Consistently convert a Windows-internal string representation into a std::string.
		::BT::Error_Res_t convertStr(LPCTSTR instr, std::string &out)
		{
#ifdef UNICODE
			size_t origsize = wcslen(instr) + 1;

			const size_t newsize = origsize * 8;
			size_t convertedChars = 0;
			std::unique_ptr<char[]> nstring(new char[newsize]);
			memset(nstring.get(), 0, newsize);
			errno_t err = wcstombs_s(&convertedChars, nstring.get(), origsize, instr, _TRUNCATE);
			BT_WIN_CHECK_ERRNO(err);
			// Destination string was always null-terminated!
			out = std::string(nstring.get());
			return BT_WIN_SUCCESS;
#else
			out = std::string(instr);
			return BT_WIN_SUCCESS;
#endif
		}

		/// Consistently convert a Windows-internal string representation into a std::wstring.
		::BT::Error_Res_t convertStr(LPCTSTR instr, std::wstring &out)
		{
#ifdef UNICODE
			out = std::wstring(instr);
			return BT_WIN_SUCCESS;
#else
			size_t origsize = strlen(instr) + 1;

			const size_t newsize = origsize * 8;
			size_t convertedChars = 0;
			std::unique_ptr<wchar_t[]> nstring(new wchar_t[newsize]);
			memset(nstring.get(), 0, newsize);
			errno_t err = mbstowcs_s(&convertedChars, nstring.get(), origsize, instr, _TRUNCATE);
			BT_WIN_CHECK_ERRNO(err);
			// Destination string was always null-terminated!
			out = std::wstring(nstring.get());
			return BT_WIN_SUCCESS;
#endif
		}

		/// Reads pid, determines the path of the executable, and writes the filename.
		::BT::Error_Res_t getPathWIN32(DWORD pid, ::BT::native_path_string_t& modPath)
		{
			try {
				// Get parent process name
				HANDLE h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION
					//| PROCESS_VM_READ
					, FALSE, pid);
				BT_WIN_CHECK_OSERROR(NULL == h);
				WCHAR szModName[MAX_UNICODE_PATH]; // defined in this namespace
				DWORD success = 0;
				DWORD sz = sizeof(szModName) / sizeof(WCHAR);
				success = QueryFullProcessImageNameW(h, 0, szModName, &sz);
				CloseHandle(h);

				BT_WIN_CHECK_OSERROR(success);
				return convertStr(szModName, modPath); // See convertStr above
			}
			catch (...) {
				std::throw_with_nested(
					BT_throw
					.add("Reason", "Failure in getting the path for a process.")
					.add("PID", pid)
				);
			}
		}

		/// Gets the CWD of the current process.
		::BT::Error_Res_t getCWD(::BT::native_path_string_t& cwd)
		{
			DWORD sz = GetCurrentDirectoryW(0, NULL);
			BT_WIN_CHECK_OSERROR(!sz);
			std::unique_ptr<wchar_t[]> cd(new WCHAR[sz]);
			DWORD result = GetCurrentDirectoryW(2500, cd.get());
			BT_WIN_CHECK_OSERROR(!result);
			cwd = ::BT::native_path_string_t(cd.get());
			return BT_WIN_SUCCESS;
		}

		/// Gets the environment of the current process.
		::BT::Error_Res_t getEnvironment(std::map<::BT::native_path_string_t, ::BT::native_path_string_t> &envs)
		{
			win_handle<LPWCH, decltype(FreeEnvironmentStringsW)> penv(
				GetEnvironmentStringsW(), FreeEnvironmentStringsW);
			BT_WIN_CHECK_OSERROR(!penv.valid());

			LPWCH pcur = penv.get();
			do {
				std::wstring tosplit(pcur);
				size_t pos = tosplit.find_first_of(L"=");
				if ((pos != ::BT::native_path_string_t::npos) && (pcur[0] != '='))
				{
					std::wstring name, val;
					name = tosplit.substr(0, pos);
					val = tosplit.substr(pos + 1); // string is null-terminated, so I can do this.
					envs[name] = val;
				}
				pcur += tosplit.size() + 1;
			} while (pcur[0]);

			return BT_WIN_SUCCESS;
		}

		/// Gets the command line of the current process.
		::BT::Error_Res_t getCmdLine(std::vector<::BT::native_path_string_t> &cmdline)
		{
			// These are in the windows path type
			int pNumArgs = 0;
			win_handle<LPWSTR*, decltype(LocalFree)> szArglist(
				CommandLineToArgvW(GetCommandLineW(), &pNumArgs),
				LocalFree);
			BT_WIN_CHECK_OSERROR(!szArglist.valid());

			for (int i = 0; i < pNumArgs; ++i)
			{
				::BT::native_path_string_t ps(szArglist.get()[i]);
				cmdline.push_back(ps);
			}
			return BT_WIN_SUCCESS;
		}

		/// Are we running in an elevated session?
		::BT::Error_Res_t getElevated(bool &elevated)
		{
			BOOL fIsRunAsAdmin = FALSE;
			DWORD dwError = ERROR_SUCCESS;
			PSID pAdministratorsGroup = NULL;

			// Allocate and initialize a SID of the administrators group.
			SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
			if (!AllocateAndInitializeSid(
				&NtAuthority,
				2,
				SECURITY_BUILTIN_DOMAIN_RID,
				DOMAIN_ALIAS_RID_ADMINS,
				0, 0, 0, 0, 0, 0,
				&pAdministratorsGroup))
			{
				dwError = GetLastError();
				goto Cleanup;
			}

			// Determine whether the SID of administrators group is enabled in 
			// the primary access token of the process.
			if (!CheckTokenMembership(NULL, pAdministratorsGroup, &fIsRunAsAdmin))
			{
				dwError = GetLastError();
				goto Cleanup;
			}

		Cleanup:
			// Centralized cleanup for all allocated resources.
			if (pAdministratorsGroup)
			{
				FreeSid(pAdministratorsGroup);
				pAdministratorsGroup = NULL;
			}

			// Throw the error if something failed in the function.
			if (ERROR_SUCCESS != dwError)
				return BT::getOSerror();

			if (fIsRunAsAdmin) elevated = true;
			else elevated = false;

			return BT_WIN_SUCCESS;
		}

		/// Get start time
		::BT::Error_Res_t getStartTime(DWORD pid, ::BT::native_path_string_t &startTime)
		{
			try {
				// Get parent process name
				win_handle<HANDLE, decltype(CloseHandle)> h(
					OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION
						//| PROCESS_VM_READ
						, FALSE, pid),
					CloseHandle, win_handle<HANDLE, decltype(CloseHandle)>::valid_handle);
				BT_WIN_CHECK_OSERROR(!h.valid());

				// Get process times
				FILETIME pCreation, pExit, pKernel, pUser;
				BT_WIN_CHECK_OSERROR(
					!GetProcessTimes(h.get(), &pCreation, &pExit, &pKernel, &pUser));

				std::wostringstream outCreation;
				SYSTEMTIME pCreationSystem, pCreationLocal;
				BT_WIN_CHECK_OSERROR(
					!FileTimeToSystemTime(&pCreation, &pCreationSystem));

				BT_WIN_CHECK_OSERROR(
					!SystemTimeToTzSpecificLocalTime(NULL, &pCreationSystem, &pCreationLocal));
				outCreation << pCreationLocal.wYear << "-" << pCreationLocal.wMonth << "-" << pCreationLocal.wDay << " "
					<< pCreationLocal.wHour << ":" << pCreationLocal.wMinute << ":" << pCreationLocal.wSecond << "."
					<< pCreationLocal.wMilliseconds;
				startTime = outCreation.str();

				return BT_WIN_SUCCESS;
			}
			catch (...) {
				std::throw_with_nested(
					BT_throw
					.add("Reason", "Failure in getting the start time for a process.")
					.add("PID", pid)
				);
			}
		}

		/// Get current module
		::BT::Error_Res_t GetCurrentModule(hModule_t &hModule)
		{
			try {
				/** \brief Get the current module that a function is executing from.
				*
				* Used because sxs loading means that multiple copies may be lying around,
				* and we want to figure out who is using which (to indicate what needs to be recompiled).
				*
				* \note Borrowed from http://stackoverflow.com/questions/557081/how-do-i-get-the-hmodule-for-the-currently-executing-code
				**/
				// Get parent process name
				HMODULE tmp_hModule = NULL;
				BOOL res = GetModuleHandleEx(
					GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
					(LPCTSTR)GetCurrentModule,
					&tmp_hModule);
				BT_WIN_CHECK_OSERROR(res);
				hModule.swap(win_handle<HMODULE, decltype(FreeLibrary)>(
					tmp_hModule,
					FreeLibrary,
					win_handle<HMODULE, decltype(FreeLibrary)>::valid_handle));
				BT_WIN_CHECK_OSERROR(!hModule.valid());

				return BT_WIN_SUCCESS;
			}
			catch (...) {
				std::throw_with_nested(
					BT_throw
					.add("Reason", "Failure in determining the current module.")
				);
			}
		}

		/// Get module path
		::BT::Error_Res_t getModulePath(hModule_t &mod, ::BT::native_path_string_t &modPath) // = nullptr
		{
			try {
				bool freeAtEnd = false;
				if (!mod.valid())
				{
					::BT::Error_Res_t e1 = GetCurrentModule(mod);
					BT_WIN_CHECK_OSERROR(GetCurrentModule(mod).first);
				}
				WCHAR filename[MAX_UNICODE_PATH];
				DWORD sz = GetModuleFileNameW(mod.get(), filename, MAX_UNICODE_PATH);
				// WinAPI has a terrible method for checking for errors here.
				BT_WIN_CHECK_OSERROR(!sz);
				BT_WIN_CHECK_OSERROR(::BT::getOSerror().first == ERROR_INSUFFICIENT_BUFFER);

				BT_WIN_CHECK(convertStr(filename, modPath));
				return BT_WIN_SUCCESS;
			}
			catch (...) {
				std::throw_with_nested(
					BT_throw
					.add("Reason", "Failure in getting a module's path")
				);
			}
		}
		/// Get module path
		::BT::Error_Res_t getModulePath(void* ptr, ::BT::native_path_string_t &modPath)
		{
			try {
				std::string modpath;
				BOOL success = false;
				if (ptr)
				{
					LPCTSTR lpModuleName = (LPCTSTR)ptr;
					HMODULE tmp_hModule;
					success = GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, lpModuleName, &tmp_hModule);
					BT_WIN_CHECK_OSERROR(!success);

					hModule_t hModule(tmp_hModule, FreeLibrary);

					BT_WIN_CHECK_OSERROR(getModulePath(hModule, modPath).first);
					return BT_WIN_SUCCESS;
				}
				return getModulePath(GetCurrentModule, modPath);
			}
			catch (...) {
				std::throw_with_nested(
					BT_throw
					.add("Reason", "Failure in getting the path for a module.")
					.add("ptr", ptr)
				);
			}
		}

		/// Get loaded modules
		::BT::Error_Res_t getLoadedModules(::BT::ModuleInfo_t<> &out)
		{
			// Get parent process name
			win_handle<HANDLE, decltype(CloseHandle)> h(
				OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION
					//| PROCESS_VM_READ
					, FALSE, ::BT::getPID()),
				CloseHandle,
				win_handle<HANDLE>::valid_handle);
			BT_WIN_CHECK_OSERROR(!h.valid());

			win_handle<HANDLE, decltype(CloseHandle)> snapshot(
				CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, ::BT::getPID()),
				CloseHandle,
				win_handle<HANDLE>::valid_handle);
			BT_WIN_CHECK_OSERROR(!snapshot.valid());

			std::shared_ptr<MODULEENTRY32> mod(new MODULEENTRY32);

			mod->dwSize = sizeof(MODULEENTRY32); // Annoying requirement
			BT_WIN_CHECK_OSERROR(!Module32First(snapshot.get(), mod.get()));
			do {
				// mod->szModule - restricted to 260 chars
				// mod->szExePath - restricted to 260 chars
				out.push_back(BT::native_path_string_t(mod->szExePath));

				// mod->hModule - TODO: use this instead? Might hit an access rights problem.
				//LPWSTR fname;
				//DWORD res = GetModuleFileNameExW(h.get(), mod->hModule, fname, )
			} while (Module32Next(snapshot.get(), mod.get()));
			return BT_WIN_SUCCESS;
		}

		/// Get user name
		::BT::Error_Res_t getUserName(::BT::native_path_string_t &username) {
			BOOL res = false;
			const DWORD clen = 256;
			DWORD len = clen;
			WCHAR hname[clen];
			BT_WIN_CHECK_OSERROR(GetUserNameW(hname, &len));
			return convertStr(hname, username);
		}

		/// Get computer name
		::BT::Error_Res_t getComputerName(::BT::native_path_string_t &username) {
			BOOL res = false;
			const DWORD clen = 256;
			DWORD len = clen;
			WCHAR hname[clen];
			BT_WIN_CHECK_OSERROR(GetComputerNameW(hname, &len));
			return convertStr(hname, username);
		}

		/// Get app config dir
		::BT::Error_Res_t getAppConfigDir(::BT::native_path_string_t &appConfigDir) {
			HRESULT resl = false;
			// The allocation-release is just awkward.
			wchar_t* happname = nullptr;
			resl = SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &happname);
			if (resl == S_OK) {
				auto e = convertStr(happname, appConfigDir);
				CoTaskMemFree(static_cast<void*>(happname));
				happname = nullptr;
				BT_WIN_CHECK_OSERROR(e.first);
			}
			BT_WIN_CHECK_OSERROR(resl != S_OK);
			return BT_WIN_SUCCESS;
		}

		/// Get user home dir
		::BT::Error_Res_t getHomeDir(::BT::native_path_string_t &homeDir) {
			HRESULT resl = false;
			// The allocation-release is just awkward.
			wchar_t* hhome = nullptr;
			resl = SHGetKnownFolderPath(FOLDERID_Profile, 0, NULL, &hhome);
			if (resl == S_OK) {
				auto e = convertStr(hhome, homeDir);
				CoTaskMemFree(static_cast<void*>(hhome));
				hhome = nullptr;
				BT_WIN_CHECK_OSERROR(e.first);
			}
			BT_WIN_CHECK_OSERROR(resl != S_OK);
			return BT_WIN_SUCCESS;
		}


#undef BT_WIN_CHECK
#undef BT_WIN_CHECK_ERRNO
#undef BT_WIN_CHECK_OSERROR
#undef BT_WIN_SUCCESS
	}
}
