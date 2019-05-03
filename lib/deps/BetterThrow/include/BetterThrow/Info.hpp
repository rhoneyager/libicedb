#pragma once
#include "defs.hpp"
#include <cerrno>
#include <string>
#include <vector>

namespace BT {
	// Path types

#if defined(BT_OS_WINDOWS)
//# if defined(UNICODE)
	/// The native path string type
	typedef std::wstring native_path_string_t;
	/// The native path stream type
	typedef std::wostream native_path_stream_t;
	/// Convenience macro to convert a string to the native representation.
# define BT_NATIVE(x) (L##x)
	//inline auto &native_cout = std::wcout;
	//# else
	//	typedef std::wstring native_path_string_t;
	//# endif
#elif defined(BT_OS_LINUX) || defined(BT_OS_UNIX) || defined(BT_OS_MACOS)
	/// The native path string type
	typedef std::string native_path_string_t;
	//auto &native_cout = std::cout;
	/// The native path stream type
	typedef std::ostream native_path_stream_t;
	/// Convenience macro to convert a string to the native representation.
# define BT_NATIVE(x) (x)
#else
# error "Unsupported OS"
#endif

	/// Links to cout or wcout; the split is needed for the Windows API functions that return wide characters.
	extern native_path_stream_t& native_cout;
	extern native_path_stream_t& native_cerr;

	/// Conversion function to take native string type and put it into a std::string.
	std::string convertStr(const std::wstring& src);
	inline std::string convertStr(const std::string& src) { return src; }

	/// Basic return type in lieu of C++17's std::optional.
	template<class T> struct PairwiseValue
	{
		bool success = false;
		T value{};
	};
	typedef std::pair<int64_t, std::string> Error_Res_t; ///< Get OS-specific error information.
	
	/// \brief Get OS-specific error information.
	/// \note errno is a signed integer, dwords are an unsigned integer. use 64-bit int type to hold them all.
	Error_Res_t getOSerror();
	/// \brief Get errno information.
	/// \param e is the error number. If <0, then it is set to errno.
	Error_Res_t getOSerrno(int e = -1);
	/// Get a human-readable OS family name (Unix, Linux, FreeBSD, macOS, Windows, ...)
	const char* getOSfamilyName();

	/// \brief Get process id number
	/// \returns the pid, or -1 on failure.
	int getPID();

	/// \brief Get the parent process id number for a given process id.
	/// \returns the parent's id number, or -1 on failure.
	int getPPID(int pid = getPID());

	/// \brief Does a process exist with the specified id?
	/// \returns false on function error, true on success.
	/// \param res is true if the process exists, false if it does not exist or if an error occurred.
	PairwiseValue<bool> pidExists(int pid);

	/// Contains information about a loaded code module (i.e. a DLL or an executable)
	template <class T = native_path_string_t>
	using ModuleInfo_t = std::vector<T>;
	//typedef std::vector<native_path_string_t> ModuleInfo_t; 

	/// List all loaded modules.
	template <class T = native_path_string_t>
	ModuleInfo_t<T> getLoadedModules()
	{
		ModuleInfo_t<native_path_string_t> mods = getLoadedModules();
		ModuleInfo_t<T> res;
		for (const auto& src : mods) res.push_back(convertStr(src));
		return res;
	}
	template<> ModuleInfo_t<native_path_string_t> getLoadedModules();

	/// Get the name of the module containing the specified address
	template <class T = native_path_string_t>
	T getModule(void* address = nullptr)
	{
		native_path_string_t res = getModule<native_path_string_t>(address);
		return convertStr(res);
	}
	template<> native_path_string_t getModule(void *address);

	template <class T> struct ProcessInfo;

	template <class U>
	ProcessInfo<U> getProcessInfo(int pid);

	/// Contains information about a process
	template <class T = native_path_string_t>
	struct ProcessInfo 	{
		T name;							///< Executable name
		T path;							///< Executable path
		T cwd;							///< Current working directory
		T startTime;					///< Process start time (OS-dependent)
		int pid = -1;					///< Process ID
		int ppid = -1;					///< Process ID of parent
		bool isElevated = false;		///< Is this app running with elevated privileges (Windows).

		std::map<T, T> environment;		///< The process' environment (OS-restricted).
		std::vector<T> cmdline;			///< The process' command line (OS-restricted).

		/// Get information about a process.
		ProcessInfo() {}
		/// Get information about a process.
		template <class U> static ProcessInfo<U> get(int pid = -1) {
			return getProcessInfo<U>(pid);
		}
	};
	template <class U>
	ProcessInfo<U> getProcessInfo(int pid)
	{
		auto p = getProcessInfo<native_path_string_t>(pid);
		ProcessInfo<U> res;
		res.name = convertStr(p.name);
		res.path = convertStr(p.path);
		res.cwd = convertStr(p.cwd);
		res.startTime = convertStr(p.startTime);

		res.pid = p.pid;
		res.ppid = p.ppid;
		res.isElevated = p.isElevated;

		for (const auto& s : p.environment)
			res.environment.emplace(std::make_pair(convertStr(s.first), convertStr(s.second)));
		for (const auto& s : p.cmdline)
			res.cmdline.push_back(convertStr(s));

		return res;
	}
	/// Get information about a process.
	template<> ProcessInfo< native_path_string_t> getProcessInfo(int pid);

	/// Write processInfo to an output stream of the correct type.
	template <class Stream, class String>
	Stream& operator<<(Stream& out, const ProcessInfo<String>& p)
	{
		out << "Name: " << p.name
			<< "\nPath: " << p.path
			<< "\nCWD: " << p.cwd
			<< "\nPID: " << p.pid
			<< "\nPPID: " << p.ppid
			<< "\nAdmin? " << p.isElevated
			<< "\nStart: " << p.startTime
			<< "\nCmdline: \n";
		for (const auto& c : p.cmdline)
			out << "\t" << c << "\n";
		out << "Environment: \n";
		for (const auto& e : p.environment)
			out << "\t" << e.first << "\t=\t" << e.second << "\n";
		out << std::endl;
		return out;
	}
	//native_path_stream_t & operator<<(native_path_stream_t&, const ProcessInfo&);

	template <class T> struct RuntimeInfo;
	template <class U> RuntimeInfo<U> getRuntimeInfo();

	/// Information about the runtime environment.
	template <class T = native_path_string_t>
	struct RuntimeInfo
	{
		T username; ///< The active user's name
		T computername; ///< The computer name
		T homedir; ///< User's home directory
		T appconfigdir; ///< Application config directory.
		//native_path_string_t sharedir;
		//native_path_string_t libdir;
		//native_path_string_t appdir;
		template <class U> static RuntimeInfo<U> get() { return getRuntimeInfo<U>(); }
	};
	template <class U>
	RuntimeInfo<U> getRuntimeInfo() {
		auto p = getRuntimeInfo<native_path_string_t>();
		RuntimeInfo<U> res;
		res.username = convertStr(p.username);
		res.computername = convertStr(p.computername);
		res.homedir = convertStr(p.homedir);
		res.appconfigdir = convertStr(p.appconfigdir);

		return res;
	}
	template<> RuntimeInfo< native_path_string_t> getRuntimeInfo();

	/// Write RuntimeInfo to an output stream of the correct type.
	template <class Stream, class String>
	Stream& operator<<(Stream& out, const RuntimeInfo<String>& r)
	{
		out << "User name: " << r.username
			<< "\nComputer name: " << r.computername
			<< "\nApp config dir: " << r.appconfigdir
			<< "\nHome directory: " << r.homedir
			<< std::endl;
		return out;
	}

}

