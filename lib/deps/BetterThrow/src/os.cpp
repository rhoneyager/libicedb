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
#elif defined(__unix__) || defined(__APPLE__)
#include <unistd.h>
extern char** environ;
#include <sys/types.h>
#include <sys/user.h>
#include <sys/sysctl.h>
#include <sys/stat.h>
#include <pwd.h>
#include <dlfcn.h>
#include <cerrno>
#ifdef __unix__
#include <link.h> // not on mac
#endif
#include <dirent.h>
#endif
#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif
#include <cstdint>
#include <memory>
#include <mutex>
#include <cstring>

#include "../include/BetterThrow/Error.hpp"
#include "../include/BetterThrow/Info.hpp"

#if defined(BT_OS_WINDOWS)
#include "os-win.hpp"
#else
#include "os-posix.hpp"
#endif

namespace BT {
	/// Cached information about the app's runtime environment.
	BT::RuntimeInfo<> rtinfo;
	/// Cached information about the app's runtime environment.
	BT::ProcessInfo<> pinfo;
	/// For debugging. Leave on.
	const bool throw_on_os_function_failures = true;

	native_path_stream_t &native_cout =
#if defined(BT_OS_WINDOWS)
		std::wcout;
#elif defined(BT_OS_LINUX) || defined(BT_OS_UNIX) || defined(BT_OS_MACOS)
		std::cout;
#else
# error "Unsupported OS"
#endif
		native_path_stream_t& native_cerr =
#if defined(BT_OS_WINDOWS)
		std::wcerr;
#elif defined(BT_OS_LINUX) || defined(BT_OS_UNIX) || defined(BT_OS_MACOS)
		std::cerr;
#else
# error "Unsupported OS"
#endif

	/// \todo Add typedefs for native_path_string_t and native_path_stream_t
	/**
	* \typedef std::string/std::wstring native_path_string_t
	* \brief The native path string type. Can be a std::wstring or std::string.
	**/
	/**
	* \typedef std::ostream/std::wostream native_path_stream_t
	* \brief The native path output stream type. Can be a std::wostream or std::ostream.
	**/


#define BT_UNIMPLEMENTED \
			if (throw_on_os_function_failures) BT_throw \
				.add("Reason", "Unimplemented code path")

	const char* getOSfamilyName() {
		const char* name =
#if defined(__FreeBSD__)
			"FreeBSD";
#elif defined(__NetBSD__)
			"NetBSD";
#elif defined(__OpenBSD__)
			"OpenBSD";
#elif defined(__bsdi__)
			"bsdi";
#elif defined(__DragonFly__)
			"DragonFly BSD";
#elif defined (__APPLE__)
			"Apple";
#elif defined(__linux__)
			"Linux";
#elif defined(_WIN32)
			"Windows";
#elif defined(__unix__)
			"Generic Unix";
#elif defined(BT_OS_WINDOWS)
			"Windows";
#elif defined(BT_OS_LINUX)
			"Linux";
#elif defined(BT_OS_UNIX)
			"Generic Unix";
#else
			"UNKNOWN";
#endif
		return name;
	}

	int getPID() {
#if defined(BT_OS_UNIX) || defined(BT_OS_LINUX)
		return (int)getpid();
#elif defined(BT_OS_WINDOWS)
		DWORD pid = 0;
		/// \note The HANDLE stuff can be removed.
		//HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		//if (NULL == h) return false;
		//PROCESSENTRY32 pe = { 0 };
		//pe.dwSize = sizeof(PROCESSENTRY32);
		pid = GetCurrentProcessId();
		//CloseHandle(h);
		return (int)pid;
#else
#ifdef BT_FORCE_OS_ERRORS
# error "Unsupported OS."
#endif
		BT_UNIMPLEMENTED;
		return -1;
#endif
	}

	int getPPID(int pid) {
		
		try {
#if defined(BT_OS_UNIX) 
			if (pid == getPID())
				return (int)getppid();
			else {
				/// \todo Complete for unix-like.
				BT_UNIMPLEMENTED;
				return -1;
			}
#elif defined(BT_OS_WINDOWS)
			DWORD Dpid = pid, ppid = 0;
			HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
			if (NULL == h) return -1;
			PROCESSENTRY32 pe = { 0 };
			pe.dwSize = sizeof(PROCESSENTRY32);
			if (!pid) Dpid = GetCurrentProcessId();
			if (Dpid < 0) return -1;
			if (Process32First(h, &pe)) {
				do {
					if (pe.th32ProcessID == Dpid) {
						ppid = pe.th32ParentProcessID;
						//printf("PID: %i; PPID: %i\n", pid, pe.th32ParentProcessID);
					}
			} while (Process32Next(h, &pe));
		}

			CloseHandle(h);
			return (int)ppid;
#else
# ifdef BT_FORCE_OS_ERRORS
#  error "Unsupported OS."
# endif
			BT_UNIMPLEMENTED.add("Reason2","Unsupported OS");
			return -1;
#endif
		}
		catch (...) {
			auto err = BT_throw
				.add("Reason", "Failure in determining the parent process id for a given process.")
				.add("PID", pid);
			std::throw_with_nested(err);
		}
	}

	::BT::PairwiseValue<bool> pidExists(int pid)
	{
		try {
			::BT::PairwiseValue<bool> result;
#if defined(BT_OS_WINDOWS)
			HANDLE h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
			if (h)
			{
				DWORD code = 0;
				if (GetExitCodeProcess(h, &code))
				{
					CloseHandle(h);
					if (code == STILL_ACTIVE)
					{
						result.value = true;
						result.success = true;
						return result;
					}
					else {
						result.value = false;
						result.success = true;
						return result;
					}
				}
				else {
					CloseHandle(h);
					result.value = false;
					result.success = true;
					return result;
				}
				CloseHandle(h);
				result.value = true;
				result.success = true;
				return result;
			}
			result.value = false;
			result.success = false;
			return result;
#elif defined(__FreeBSD__) || defined (__APPLE__) || defined(__MACH__)
			// Works for both freebsd and mac os
			//bool res = false;
			int mib[4];
			size_t len;
			struct kinfo_proc kp;

			/* Fill out the first three components of the mib */
			len = 4;
			sysctlnametomib("kern.proc.pid", mib, &len);
			mib[3] = pid;
			len = sizeof(kp);
			int sres = sysctl(mib, 4, &kp, &len, NULL, 0);
			if (sres == -1) {
				// Either the pid does not exist, or some other error
				if (errno == ENOENT) {
					result.value = false;
					result.success = true;
					return result;
				}
				result.value = false;
				result.success = false;
				return result;
			}
			else if ((sres == 0) && (len > 0)) {
				result.value = true;
				result.success = true;
				return result;
			}
			result.value = false;
			result.success = true;
			return result;

#elif defined(BT_OS_LINUX)
			// Need to check existence of directory /proc/pid
			std::ostringstream pname;
			pname << "/proc/" << pid;
			std::string spname = pname.str();
			struct stat sb;
			if (stat(spname.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode))
			{
				result.value = true;
				result.success = true;
				return result;
			}
			result.value = false;
			result.success = true;
			return result;
#else
# ifdef BT_FORCE_OS_ERRORS
#  error "Unsupported OS."
# endif
			BT_UNIMPLEMENTED;
			result.value = false;
			result.success = false;
			return result;
#endif
			}
		catch (...) {
			auto err = BT_throw
				.add("Reason", "Failure in determining if a process exists.")
				.add("PID", pid);
			std::throw_with_nested(err);
		}
	}

	::BT::Error_Res_t getOSerrno(int e)
	{
		if (e == -1) e = errno;

# if (_POSIX_C_SOURCE >= 200112L) && !  _GNU_SOURCE
		const int buflen = 1024; // http://man7.org/linux/man-pages/man3/strerror.3.html
		char strerrbuf[buflen] = "\0";
		//int strerror_r(int errnum, char *buf, size_t buflen); // XSI-compliant
		int ret = strerror_r(e, strerrbuf, buflen);
		if (ret != 0) std::abort(); // This should NEVER happen.
		return std::pair<int64_t, std::string>(e, std::string(strerrbuf));
# elif defined(_BSD_SOURCE)
		//char *strerror_r(int errnum, char *buf, size_t buflen); // GNU-specific
		strerror_r(e, strerrbuf, buflen);
		return std::pair<int64_t, std::string>(e, std::string(strerrbuf));
#else
		const char* em = strerror(e);
		return std::pair<int64_t, std::string>(e, std::string(em));
# endif
	}

	::BT::Error_Res_t getOSerror() {
#if defined(BT_OS_WINDOWS)
		LPVOID lpMsgBuf;
		DWORD dw = GetLastError();

		DWORD bufLen = FormatMessageA(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			dw,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPSTR)& lpMsgBuf,
			0, NULL);
		if (bufLen)
		{
			LPCSTR lpMsgStr = (LPCSTR)lpMsgBuf;
			std::string result(lpMsgStr, lpMsgStr + bufLen);

			LocalFree(lpMsgBuf);

			return std::pair<int64_t, std::string>(dw, result);
		}
		return std::pair<int64_t, std::string>(dw, std::string("Unknown"));

#elif defined(BT_OS_UNIX) || defined(BT_OS_LINUX)
		return getOSerrno();
#else
# ifdef BT_FORCE_OS_ERRORS
#  error "Unsupported OS."
# endif
		BT_UNIMPLEMENTED;
		return getOSerrno();
#endif
	}

	template<>
	::BT::ProcessInfo<native_path_string_t> getProcessInfo(int pid) {
		::BT::ProcessInfo<native_path_string_t> res;
		res.pid = (pid<0) ? getPID() : pid;
		pid = res.pid;

		try {
			if (pid < 0) BT_throw
				.add("Reason", "Cannot find PID.")
				();

			auto exists = pidExists(pid);
			if (!exists.success || !exists.value) {
				auto ose = getOSerror();
				BT_throw
					.add("Reason", "PID does not exist.")
					.add("pid", pid)
					.add("OS_Error_Num", ose.first)
					.add("OS_Error_String", ose.second)
					();
			}
		
			res.ppid = getPPID(pid);

#if defined(BT_OS_WINDOWS)
			/// \todo Check all of these return values. Throw on failure.
			if (win::getPathWIN32((DWORD)pid, res.path) // int always fits in DWORD
				.first) throw BT_throw;
			if (pid == getPID()) {
				// On Windows, these functions are only applicable to the current process.
				if (win::getCWD(res.cwd).first) throw BT_throw;
				if (win::getCmdLine(res.cmdline).first) throw BT_throw;
				if (win::getEnvironment(res.environment).first) throw BT_throw;
				if (win::getElevated(res.isElevated).first) throw BT_throw;
				if (win::getStartTime((DWORD)pid, res.startTime).first) throw BT_throw;
			}
#elif defined(BT_OS_UNIX) || defined(BT_OS_LINUX)
			posix::getModulePath((void*)getOSerror, res.path);
			if (pid == getPID()) {
				if (posix::getCWD(res.cwd).first) throw BT_throw;
				if (posix::getCmdLine(res.cmdline).first) throw BT_throw;
				if (posix::getEnvironment(res.environment).first) throw BT_throw;
				if (posix::getElevated(res.isElevated).first) throw BT_throw;
				if (posix::getStartTime(pid, res.startTime).first) throw BT_throw;
			}
#else
# ifdef BT_FORCE_OS_ERRORS
#  error "Unsupported OS."
# endif
		BT_throw // not BT_UNIMPLEMENTED because this ALWAYS should throw.
			.add("Reason", "Unimplemented code path.")
			.add("pid", pid)
			.add("OS_String", getOSfamilyName())
			();
#endif

		// Get name from path. Split by path separator / or \\.
		{
			size_t pos = res.path.find_last_of(BT_NATIVE("/\\"));
			if (pos != ::BT::native_path_string_t::npos)
				res.name = res.path.substr(pos + 1); // string is null-terminated, so I can do this.
		}

		}
		catch (...) {
			std::throw_with_nested(
				BT_throw
				.add("Reason", "Failure in getting information for a process.")
				.add("Actual-PID", res.pid)
				.add("Requested-PID", pid)
			);
		}

		return res;

		}

	template<>
	::BT::native_path_string_t getModule(void *address)
	{
		::BT::native_path_string_t modpath;
		if (!address) address = (void*)getPID;

#if defined(BT_OS_WINDOWS)
		auto err = win::getModulePath(address, modpath);
#elif defined(BT_OS_UNIX) || defined(BT_OS_LINUX)
		auto err = posix::getModulePath(address, modpath);
#else
# ifdef BT_FORCE_OS_ERRORS
#  error "Unsupported OS."
# endif
		BT::Error_Res_t err(-1, "Unimplemented code path");
#endif
		if (err.first) BT_throw
			.add("Reason", "Cannot get module path.")
			.add("address", address)
			.add("Error_Num", err.first)
			.add("Error_String", err.second)
			();
		return modpath;
	}
	
	template<>
	ModuleInfo_t<::BT::native_path_string_t> getLoadedModules()
	{
		ModuleInfo_t<::BT::native_path_string_t> res;
#if defined(BT_OS_WINDOWS)
		auto err = win::getLoadedModules(res);
#elif defined(BT_OS_UNIX) || defined(BT_OS_LINUX)
		auto err = posix::getLoadedModules(res);
#else
# ifdef BT_FORCE_OS_ERRORS
#  error "Unsupported OS."
# endif
		BT::Error_Res_t err(-1, "Unimplemented code path");
#endif
		if (err.first) BT_throw
			.add("Reason", "Cannot get module paths.")
			.add("Error_Num", err.first)
			.add("Error_String", err.second)
			();
		return res;
	}
	
	template<>
	RuntimeInfo<native_path_string_t> getRuntimeInfo<native_path_string_t>() {
		// This only is queried the first time it is called.
		static std::mutex rtmutex;
		{
			std::lock_guard<std::mutex> lock(rtmutex);
			if (rtinfo.computername.size() == 0) {
				pinfo = ProcessInfo<::BT::native_path_string_t>::get<::BT::native_path_string_t>(getPID());
#if defined(BT_OS_WINDOWS)
				/// \todo Check return values. Either throw or log.
				if (win::getUserName(rtinfo.username).first) throw BT_throw;
				if (win::getComputerName(rtinfo.computername).first) throw BT_throw;
				if (win::getAppConfigDir(rtinfo.appconfigdir).first) throw BT_throw;
				if (win::getHomeDir(rtinfo.homedir).first) throw BT_throw;
#elif defined(BT_OS_UNIX) || defined(BT_OS_LINUX)
				if (posix::getUserName(rtinfo.username).first) throw BT_throw;
				if (posix::getComputerName(rtinfo.computername).first) throw BT_throw;
				if (posix::getAppConfigDir(rtinfo.appconfigdir).first) throw BT_throw;
				if (posix::getHomeDir(rtinfo.homedir).first) throw BT_throw;
#else
# ifdef BT_FORCE_OS_ERRORS
#  error "Unsupported OS."
# endif
				BT_UNIMPLEMENTED;
#endif
			}
		}

		return rtinfo;
	}
	
	/// \note Conversion depends on std::setlocale!!!!!
	std::string convertStr(const std::wstring& src)
	{
		std::mbstate_t state = std::mbstate_t();
		auto data = src.data();
		std::size_t len = 1 + std::wcsrtombs(nullptr, &data, 0, &state);
		if (!len) throw BT_throw.add("Reason", getOSerrno().second);
		std::vector<char> mbstr(len);
		auto cres = std::wcsrtombs(&mbstr[0], &data, mbstr.size(), &state);
		if (cres == static_cast<std::size_t>(-1))
			throw BT_throw.add("Reason", getOSerrno().second);
		return std::string(mbstr.data());
		/*
#if defined(BT_OS_WINDOWS)
		std::string res;
		auto err = win::convertStr(src.data(), res);
		if (err.first) throw BT_throw
			.add("Reason" , err.second);
		return res;


		std::mbstate_t state = std::mbstate_t();
		auto data = src.data();
		std::size_t len = 1 + std::wcsrtombs(nullptr, &data, 0, &state);
		if (!len) throw BT_throw.add("Reason", getOSerrno().second);
		std::vector<char> mbstr(len);
		auto cres = std::wcsrtombs(&mbstr[0], &data, mbstr.size(), &state);
		if (cres == static_cast<std::size_t>(-1))
			throw BT_throw.add("Reason", getOSerrno().second);
		return res;
#else
		std::mbstate_t state = std::mbstate_t();
		auto data = src.data();
		std::size_t len = 1 + std::wcsrtombs(nullptr, &data, 0, &state);
		if (!len) throw BT_throw.add("Reason", getOSerrno().second);
		std::vector<char> mbstr(len);
		auto cres = std::wcsrtombs(&mbstr[0], &data, mbstr.size(), &state);
		if (cres == static_cast<std::size_t>(-1))
			throw BT_throw.add("Reason", getOSerrno().second);
		return res;
#endif
*/
	}

}
