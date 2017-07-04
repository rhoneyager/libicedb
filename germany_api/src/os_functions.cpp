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
#elif defined(__unix__)
#include <unistd.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/sysctl.h>
#include <pwd.h>
#include <dlfcn.h>
#include <link.h>
#include <dirent.h>
#endif
#include <mutex>
#include <thread>
#include <ctime>
#include <iostream>
#include <cstdio>
#include <algorithm>
#include <cstdlib>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include "../germany/error.h"
#include "../germany/error_context.h"
#include "../germany/os_functions.h"
#include "../germany/os_functions.hpp"
#include "../germany/mem.h"
#include "../germany/util.h"

namespace icedb {
	namespace os_functions {
		namespace vars {
			std::mutex m_sys_names;
			std::string hostname, username,
				homeDir, appConfigDir, moduleCallbackBuffer;
			bool _consoleTerminated = false;
			// First element is name, second is path. Gets locked with m_sys_names.
			std::vector<std::pair<std::string, std::string> > loadedModulesList;
			/// Private flag that determines if the app waits for the user to press 'Enter' to terminate it at the end of execution.
			bool doWaitOnExit = false;
			bool doWaitOnExitQueriedDefault = false;
			std::map<std::string, std::string> mmods;
		}
		namespace win {
#ifdef _WIN32
			BOOL WINAPI _CloseHandlerRoutine(DWORD dwCtrlType) {
				// Helps gracefully close console
				vars::_consoleTerminated = true;
				return false;
			}

			std::string convertStr(const LPTSTR instr)
			{
#ifdef UNICODE
				size_t origsize = wcslen(instr) + 1;

				const size_t newsize = origsize * 4;
				size_t convertedChars = 0;
				std::unique_ptr<char[]> nstring(new char[newsize]);
				wcstombs_s(&convertedChars, nstring.get(), origsize, instr, _TRUNCATE);
				// Destination string was always null-terminated!
				std::string res(nstring.get());
#else
				std::string res(instr);
#endif
				return std::move(res);
			}

			std::string convertStr(const PWSTR instr)
			{
				size_t origsize = wcslen(instr) + 1;

				const size_t newsize = origsize * 4;
				size_t convertedChars = 0;
				std::unique_ptr<char[]> nstring(new char[newsize]);
				//char nstring[newsize];
				wcstombs_s(&convertedChars, nstring.get(), origsize, instr, _TRUNCATE);
				// Destination string was always null-terminated!
				std::string res(nstring.get());

				return std::move(res);
			}
		
			HMODULE GetCurrentModule()
			{
			  /** \brief Get the current module that a Ryan_Debug function is executing from.
			  *
			  * Used because sxs loading means that multiple copies may be lying around,
			  * and we want to figure out who is using which (to indicate what needs to be recompiled).
			  *
			  * \note Borrowed from http://stackoverflow.com/questions/557081/how-do-i-get-the-hmodule-for-the-currently-executing-code
			  **/
				HMODULE hModule = NULL;
				GetModuleHandleEx(
					GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
					(LPCTSTR)GetCurrentModule,
					&hModule);

				return hModule;
			}

			std::string GetModulePath(HMODULE mod)
			{
				std::string out;
				bool freeAtEnd = false;
				if (!mod)
				{
					mod = GetCurrentModule();
					if (!mod) return std::move(out);
					freeAtEnd = true;
				}
				const DWORD nSize = MAX_PATH * 4;
				TCHAR filename[nSize];
				DWORD sz = GetModuleFileName(mod, filename, nSize);
				out = convertStr(filename);
				if (freeAtEnd)
					FreeLibrary(mod);
				return std::move(out);
			}

			bool getPathWIN32(DWORD pid, std::string &modPath, std::string &filename)
			{
				HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
				if (NULL == h) return false;
				CloseHandle(h);
				// Get parent process name
				h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION
					//| PROCESS_VM_READ
					, FALSE, pid);
				if (NULL == h) return false;
				TCHAR szModName[MAX_PATH];
				DWORD success = 0;
				DWORD sz = sizeof(szModName) / sizeof(TCHAR);
				success = QueryFullProcessImageName(h, 0, szModName, &sz);
				//success = GetModuleFileNameEx(h,NULL,szModName,sizeof(szModName) / sizeof(TCHAR));

				modPath = convertStr(szModName); // See previous function
				size_t lp = modPath.find_last_of("/\\");
				if (lp == std::string::npos) filename = modPath;
				else if (lp < modPath.size()-1) filename = modPath.substr(lp + 1);
				else filename = modPath;

				CloseHandle(h);
				if (!success)
				{
					auto err = ICEDB_error_context_create(ICEDB_ERRORCODES_OS);
					const size_t mxErrorMsg = 500;
					char es[mxErrorMsg] = "";
					success = GetLastError();
					snprintf(es, mxErrorMsg, "%d", success);
					ICEDB_error_context_add_string2(err, "WinError", es);
					return false;
				}
				return true;
			}

			bool IsAppRunningAsAdminMode()
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
				{
					auto err = ICEDB_error_context_create(ICEDB_ERRORCODES_OS);
					const size_t mxErrorMsg = 500;
					char es[mxErrorMsg] = "";
					snprintf(es, mxErrorMsg, "%d", dwError);
					ICEDB_error_context_add_string2(err, "WinError", es);
					return false;
				}

				if (fIsRunAsAdmin) return true;
				return false;
			}
#endif
		}
		namespace unix {
#ifdef __unix__
			bool dirExists(const char *p) {
				DIR *d = NULL;
				d = opendir(p);
				bool res = (d) ? true : false;
				if (d) closedir(d);
				return res;
			}
			/// \note Keeping function definition this way to preserve compatibility with gcc 4.7
			int moduleCallback(dl_phdr_info *info, size_t sz, void* data)
			{
				std::string name(info->dlpi_name);
				if (!name.size()) return 0;
				mmods[name] = name;
				return 0;
			}
#endif
		}
		bool populateOSstrings() {
			using namespace icedb::os_functions::vars;
			std::lock_guard<std::mutex> lock(m_sys_names);
			// Because of the mutex lock, all of the getlogin functions are safe.
			if (username.size()) return true;

			// Username
#if defined(ICEDB_OS_WINDOWS)
			BOOL res = false;
			const DWORD clen = 256;
			DWORD len = clen;
			TCHAR hname[clen];
			res = GetUserName(hname, &len);
			if (res) username = win::convertStr(hname);
			else goto funcErrorOS;

			res = GetComputerName(hname, &len);
			if (res) hostname = win::convertStr(hname);
			else goto funcErrorOS;

			HRESULT resl = false;
			wchar_t* happname = nullptr;
			res = SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &happname);
			if (resl == S_OK) appConfigDir = win::convertStr(happname);
			else goto funcErrorOS;

			wchar_t* hhomename = nullptr;
			res = SHGetKnownFolderPath(FOLDERID_Profile, 0, NULL, &hhomename);
			if (resl == S_OK) homeDir = win::convertStr(hhomename);
			else goto funcErrorOS;

			goto done;
			funcErrorOS:
				DWORD errn = GetLastError();
				auto err = ICEDB_error_context_create(ICEDB_ERRORCODES_OS);
				const char* osname = ICEDB_error_getOSname();
				ICEDB_error_context_add_string2(err, "OS_id", osname);
				char errns[50];
				snprintf(errns, 50, "%d", errn);
				ICEDB_error_context_add_string2(err, "OS_errno", errns);
				if (!username.size()) ICEDB_error_context_add_string2(err, "OS_func", "GetUserName");
				else if (!hostname.size()) ICEDB_error_context_add_string2(err, "OS_func", "GetComputerName");
				else if (!appConfigDir.size()) ICEDB_error_context_add_string2(err, "OS_func", "SHGetKnownFolderPath app config dir");
				else if (!homeDir.size()) ICEDB_error_context_add_string2(err, "OS_func", "SHGetKnownFolderPath profile dir");
			done:
				if (happname) CoTaskMemFree(static_cast<void*>(happname));
				if (hhomename) CoTaskMemFree(static_cast<void*>(hhomename));

#elif defined(__unix__)
			const size_t len = 65536;
			char hname[len];
			int res = 0;
			char* envres = NULL;

			if (envres = getenv("USER")) username = std::string(envres);
			else if (envres = getenv("LOGNAME")) username = std::string(envres);
			if (!username.size()) {
#if defined(_POSIX_C_SOURCE)
#if _POSIX_C_SOURCE >= 199506L
				res = getlogin_r(hname, len); // getlogin and getlogin_r have problems. Avoid.
				if (!res) username = std::string(hname);
#else
				if (envres = getlogin()) username = std::string(envres);
#endif
				// Has getpwuid_r
				if (!username.size()) {
					uid_t uid = geteuid();
					struct passwd* ps = ICEDB_malloc(sizeof passwd);
					struct passwd** pres;
					res = getpwuid_r(uid, ps, hname, len, pres);
					if ((res == 0) && pres) {
						username = std::string(ps->pw_name);
					}

					ICEDB_free(ps);
				}
#else
				// May have getpwuid
				uid_t uid = geteuid();
				struct passwd* ps = getpwuid(uid);
				if (ps) username = std::string(ps->pw_name);
#endif
			}
			
			// Hostname
			res = gethostname(hname, len); // May be empty
			if (hname) hostname = std::string(hname);

			// Home dir
			if (envres = getenv("HOME")) homeDir = std::string(envres);
			if (!homeDir.size())
			{
#if defined(_POSIX_C_SOURCE) || defined(_BSD_SOURCE) || defined(_SVID_SOURCE)
				struct passwd pw, *pwp;
				int res = getpwuid_r(geteuid(), &pw, hname, len, &pwp);
				if (res == 0) {
					const char *homedir = pw.pw_dir;
					homeDir = std::string(homedir);
#else
				struct passwd* ps = getpwuid(geteuid());
				if (ps) homeDir = std::string(ps->pw_dir);
#endif
			}
			
			// App config dir
			if (envres = getenv("XDG_CONFIG_HOME")) appConfigDir = std::string(envres);
			else {
				appConfigDir = homeDir;
				appConfigDir.append("/.config");
			}

			goto done;
		funcErrorOS:
			auto err = ICEDB_error_context_create(ICEDB_ERRORCODES_OS);
			const char* osname = ICEDB_error_getOSname();
			ICEDB_error_context_add_string2(err, "OS_id", osname);
			if (!username.size()) ICEDB_error_context_add_string2(err, "OS_func", "getlogin_r");
			else if (!hostname.size()) ICEDB_error_context_add_string2(err, "OS_func", "gethostname");
			else if (!appConfigDir.size()) ICEDB_error_context_add_string2(err, "OS_func", "getEnviron app config dir");
			else if (!homeDir.size()) ICEDB_error_context_add_string2(err, "OS_func", "getEnviron profile dir");
		done:

#endif
			auto ctxerr = ICEDB_error_context_create(ICEDB_ERRORCODES_UNIMPLEMENTED);
			const char* cosname = ICEDB_error_getOSname();
			ICEDB_error_context_add_string2(ctxerr, "OS_id", cosname);
			return false;
		}
	}
}
using namespace icedb::os_functions::vars;

bool ICEDB_pidExists(int pid, bool &res)
{
	// Function needed because Qt is insufficient, and Windows / Unix have 
	// different methods of ascertaining this.
#if defined(ICEDB_OS_WINDOWS)
	HANDLE h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
	if (h)
	{
		DWORD code = 0;
		if (GetExitCodeProcess(h, &code))
		{
			CloseHandle(h);
			if (code == STILL_ACTIVE)
			{
				res = true;
				return true;
			}
			else {
				res = false;
				return true;
			}
		}
		else {
			CloseHandle(h);
			res = false;
			return true;
		}
		CloseHandle(h);
		res = true;
		return true;
	}
	return false;
#elif defined(ICEDB_OS_LINUX)
	// Need to check existence of directory /proc/pid
	std::ostringstream pname;
	pname << "/proc/" << pid;
	if (icedb::os_functions::unix::dirExists(pname.str().c_str())) {
		res = true;
		return true;
	}
	res = false;
	return true;
#elif defined(__FreeBSD__) || defined (__APPLE__)
	// Works for both freebsd and mac os
	res = false;
	int i, mib[4];
	size_t len;
	struct kinfo_proc kp;

	/* Fill out the first three components of the mib */
	len = 4;
	sysctlnametomib("kern.proc.pid", mib, &len);
	mib[3] = pid;
	len = sizeof(kp);
	int res = sysctl(mib, 4, &kp, &len, NULL, 0);
	if (res == -1) {
		// Either the pid does not exist, or some other error
		if (errno == ENOENT) {
			res = false; return true;
		}
		auto err = ICEDB_error_context_create(ICEDB_ERRORCODES_OS);
		const char* osname = ICEDB_error_getOSname();
		ICEDB_error_context_add_string2(err, "OS_id", osname);
		const int buflen = 200;
		char strerrbuf[buflen] = "\0";
		snprintf(strerrbuf, "%d", errno);
		ICEDB_error_context_add_string2(err, "errno", strerrbuf);

		// strerror_r will always yield a null-terminated string.
		//int ebufres = strerror_r(err, strerrbuf, buflen);
	}
	else if ((res == 0) && (len > 0)) res = true;
	return true;
#endif
	auto err = ICEDB_error_context_create(ICEDB_ERRORCODES_UNIMPLEMENTED);
	const char* osname = ICEDB_error_getOSname();
	ICEDB_error_context_add_string2(err, "OS_id", osname);
	return false;
}

int ICEDB_getPID() {
#if defined(__unix__)
	return (int)getpid();
#elif defined(ICEDB_OS_WINDOWS)
	DWORD pid = 0;
	/// \note The HANDLE stuff can be removed.
	HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (NULL == h) return false;
	PROCESSENTRY32 pe = { 0 };
	pe.dwSize = sizeof(PROCESSENTRY32);
	pid = GetCurrentProcessId();
	CloseHandle(h);
	return (int)pid;
#endif
	auto err = ICEDB_error_context_create(ICEDB_ERRORCODES_UNIMPLEMENTED);
	const char* osname = ICEDB_error_getOSname();
	ICEDB_error_context_add_string2(err, "OS_id", osname);
	return -1;
}

int ICEDB_getPPID(int pid) {
#if defined(__unix__)
	return (int)getppid();
#elif defined(ICEDB_OS_WINDOWS)
	DWORD Dpid = pid, ppid = 0;
	HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (NULL == h) return false;
	PROCESSENTRY32 pe = { 0 };
	pe.dwSize = sizeof(PROCESSENTRY32);
	if (!pid) Dpid = GetCurrentProcessId();
	if (Process32First(h, &pe)) {
		do {
			if (pe.th32ProcessID == Dpid) {
				ppid = pe.th32ParentProcessID;
				//printf("PID: %i; PPID: %i\n", pid, pe.th32ParentProcessID);
			}
		} while (Process32Next(h, &pe));
	}

	//std::cout << "Pid " << pid << "\nPPID " << ppid << std::endl;
	CloseHandle(h);
	return (int)ppid;
#endif
	auto err = ICEDB_error_context_create(ICEDB_ERRORCODES_UNIMPLEMENTED);
	const char* osname = ICEDB_error_getOSname();
	ICEDB_error_context_add_string2(err, "OS_id", osname);
	return -1;
}

bool ICEDB_waitOnExitGetDefault() {
#if defined(ICEDB_OS_WINDOWS)
	// Get pid and parent pid. Duplicated getPID and getPPID, but combines the calls.
	DWORD pid = 0, ppid = 0;
	HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (NULL == h) return false;
	PROCESSENTRY32 pe = { 0 };
	pe.dwSize = sizeof(PROCESSENTRY32);
	pid = GetCurrentProcessId();
	if (Process32First(h, &pe)) {
		do {
			if (pe.th32ProcessID == pid) {
				ppid = pe.th32ParentProcessID;
			}
		} while (Process32Next(h, &pe));
	}
	CloseHandle(h);

	// Get parent process name
	std::string filepath, filename;
	icedb::os_functions::win::getPathWIN32(ppid, filepath, filename);

	//std::cout << filename.string() << std::endl;
	// If run from cmd, no need to wait
	if (filename == "cmd.exe") return false;
	// Cygwin
	if (filename == "bash.exe") return false;
	if (filename == "tcsh.exe") return false;
	// Don't need these due to end return. Just for reference.
	//if (filename == "devenv.exe") return true;
	//if (filename == "explorer.exe") return true;
	return true;
#endif
	return false;
}
void ICEDB_waitOnExitSet(bool val) {
	std::lock_guard<std::mutex> lock(m_sys_names);
	doWaitOnExitQueriedDefault = true;
	doWaitOnExit = val;
}
bool ICEDB_waitOnExitGet() {
	std::lock_guard<std::mutex> lock(m_sys_names);
	if (!doWaitOnExitQueriedDefault) {
		doWaitOnExitQueriedDefault = true;
		doWaitOnExit = ICEDB_waitOnExitGetDefault();
	}
	return doWaitOnExit;
}

const char* ICEDB_getUserName() {
	icedb::os_functions::populateOSstrings();
	return username.c_str();
}
const char* ICEDB_getHostName() {
	icedb::os_functions::populateOSstrings();
	return hostname.c_str();
}
const char* ICEDB_getAppConfigDir() {
	icedb::os_functions::populateOSstrings();
	return appConfigDir.c_str();
}
const char* ICEDB_getHomeDir() {
	icedb::os_functions::populateOSstrings();
	return homeDir.c_str();
}

void ICEDB_free_enumModulesRes(ICEDB_enumModulesRes* p) {
	if (!p) return;
	if (p->sz) {
		for (int i = 0; i < p->sz; ++i)
			ICEDB_free((void*)p->modules[i]);
	}
	ICEDB_free(p->modules);
	ICEDB_free(p);
}
void ICEDB_enumModules(int pid, ICEDB_enumModulesRes* p) {
	p = (ICEDB_enumModulesRes*)ICEDB_malloc(sizeof ICEDB_enumModulesRes);
	
#if defined(_WIN32)
	HANDLE h = NULL, snapshot = NULL;
		h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
		std::shared_ptr<MODULEENTRY32> mod(new MODULEENTRY32);
		if (!h || h == INVALID_HANDLE_VALUE) goto err;
		snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
		if (!snapshot || snapshot == INVALID_HANDLE_VALUE) goto err;
		mod->dwSize = sizeof(MODULEENTRY32); // Annoying requirement
		if (!Module32First(snapshot, mod.get())) goto err;
		do {
			std::string modName = icedb::os_functions::win::convertStr(mod->szModule);
			std::string modPath = icedb::os_functions::win::convertStr(mod->szExePath);
			mmods[modName] = modPath;
		} while (Module32Next(snapshot, mod.get()));

		goto done;
	err:
		auto ctxerr = ICEDB_error_context_create(ICEDB_ERRORCODES_OS);
		const char* cosname = ICEDB_error_getOSname();
		ICEDB_error_context_add_string2(ctxerr, "OS_id", cosname);
		if (!h || h == INVALID_HANDLE_VALUE)
			ICEDB_error_context_add_string2(ctxerr, "Description", "Error in getting handle for process!");
		else if (!snapshot || snapshot == INVALID_HANDLE_VALUE)
			ICEDB_error_context_add_string2(ctxerr, "Description", "Cannot get handle snapshot!");
		else ICEDB_error_context_add_string2(ctxerr, "Description", "Cannot list first modle!");
	done:

	if (snapshot && snapshot != INVALID_HANDLE_VALUE) CloseHandle(snapshot);
	if (h && h != INVALID_HANDLE_VALUE) CloseHandle(h);
#elif defined(__unix__)
	if (pid != getPID()) return;
	if (!moduleCallbackBuffer.size()) {
		dl_iterate_phdr(icedb::os_functions::unix::moduleCallback, NULL);
	}
#else
#endif
	// Convert the map to the resultant structure.
	p->sz = icedb::os_functions::vars::mmods.size();
	char **nmods = (char**)ICEDB_malloc(sizeof (char**));
	size_t i = 0;
	for (auto &s : mmods) {
		nmods[i] = ICEDB_COMPAT_strdup_s(s.first.c_str(), s.first.size());
		++i;
	}
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
void ICEDB_libEntry(int, char**) {
#ifdef _WIN32
	// Get PID
	DWORD pid = 0;
	HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (NULL == h) return;
	PROCESSENTRY32 pe = { 0 };
	pe.dwSize = sizeof(PROCESSENTRY32);
	pid = GetCurrentProcessId();
	CloseHandle(h);

	// Get parent process name
	h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION
		//| PROCESS_VM_READ
		, FALSE, pid);
	if (NULL == h) return;
	TCHAR szModName[600];
	DWORD success = 0;
	DWORD sz = sizeof(szModName) / sizeof(TCHAR);
	success = QueryFullProcessImageName(h, 0, szModName, &sz);

	// Set Console Title
	SetConsoleTitle(szModName);


	// Also, set the window closing routine
	// This allows for the user to click the X (or press ctrl-c)
	// without causing a fault.
	// The fault is because the window closes before the atexit 
	// functions can write output.
	SetConsoleCtrlHandler(icedb::os_functions::win::_CloseHandlerRoutine, true);
	CloseHandle(h);
#endif


	// Set appexit
	atexit(ICEDB_libExit);
	// Static registration of i/o objects
}
void ICEDB_libExit() {
	using namespace std;
	if (_consoleTerminated) return;
	if (doWaitOnExit)
	{
		cerr << endl << "Program terminated. Press return to exit." << endl;
		//std::getchar();
		// Ignore to the end of file
//#ifdef max
//#undef max
//#endif
		cin.clear();
		//cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		std::string ln;
		std::getline(cin, ln);
	}
}






namespace icedb {
	namespace os_functions {

		bool pidExists(int pid) { bool res = false; ICEDB_pidExists(pid, res); }
		int getPID() { return ICEDB_getPID(); }
		int getPPID(int pid) { return ICEDB_getPPID(pid); }
		void libEntry(int argc, char** argv) { ICEDB_libEntry(argc, argv); }
		void waitOnExit(bool val) { ICEDB_waitOnExitSet(val); }
		bool waitOnExit() { return ICEDB_waitOnExitGet(); }
	}
}
