#if defined(__unix__) || defined(__APPLE__)
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
#include <ctime>
#include <sstream>
#include <fstream>

#include "../include/BetterThrow/Error.hpp"
#include "../include/BetterThrow/Info.hpp"
#include "os-posix.hpp"

namespace BT {
	namespace posix {
		/// \brief Maximum path length.
		/// \note See http://insanecoding.blogspot.com/2007/11/pathmax-simply-isnt.html
		const int my_path_max = (PATH_MAX > 32767) ? PATH_MAX : 32767;
		const bool throw_on_posix_function_failures = true; ///< For debugging. Leave on.

		/// Environment variables get cached here.
		std::map<::BT::native_path_string_t, ::BT::native_path_string_t> my_envs;

#define BT_POSIX_CHECK(x) \
			if ((x).first && !throw_on_posix_function_failures) return BT::getOSerror(); \
			else if ((x).first && throw_on_posix_function_failures) throw BT_throw \
				.add("OS-Error", x.second) \
				.add("errno", x.first)
#define BT_POSIX_CHECK_ERRNO(x) \
			if ((x) && !throw_on_posix_function_failures) return BT::getOSerrno(x); \
			else if ((x) && throw_on_posix_function_failures) throw BT_throw \
				.add("OS-Error", ::BT::getOSerrno(x).second) \
				.add("errno", x)
#define BT_POSIX_CHECK_OSERROR(x) \
			if ((x) && !throw_on_posix_function_failures) return BT::getOSerror(); \
			else if ((x) && throw_on_posix_function_failures) throw BT_throw \
				.add("OS-Error", ::BT::getOSerror().second) \
				.add("OS-Error-Number", ::BT::getOSerror().first)
#define BT_POSIX_SUCCESS ::BT::getOSerrno(0);
#define BT_UNIMPLEMENTED \
			if (throw_on_posix_function_failures) throw BT_throw.add("Reason", "Unimplemented code path"); \
			return BT::Error_Res_t(-1, "Unimplemented code path");

		/// \brief This is a class to support RAII for posix handles.
		template <typename T, typename Deleter_t = bool(*)(T)>
		class posix_handle
		{
			T _val;
			//typedef void(*Deleter_t)(T);
			Deleter_t *_deleter;
			typedef bool(*Validator_t)(T);
			Validator_t _validator;
		public:
			static bool valid_general(T vv) { return vv != nullptr; }

			posix_handle() : _val(nullptr), _deleter(nullptr) {}
			posix_handle(T v, Deleter_t *d, Validator_t val = valid_general) : _val(v), _deleter(d), _validator(val) {}
			posix_handle(posix_handle<T> &&src) : _val(src._val), _deleter(src._deleter), _validator(src._validator) { src.release(); }
			posix_handle<T, Deleter_t> & operator=(const posix_handle<T, Deleter_t>&) = delete;
			posix_handle<T, Deleter_t>(const posix_handle<T, Deleter_t>&) = delete;

			void reset(T v) {
				if (valid() && *_deleter) (*_deleter)(_val);
				_val = v;
			}
			~posix_handle() {
				if (valid() && *_deleter) (*_deleter)(_val);
			}
			bool valid() const { return _validator(_val); }
			//operator !() const { return _val == nullptr; }
			T release() { T sv = _val; _val = nullptr; return sv; }
			T get() const { return _val; }
			void swap(posix_handle<T, Deleter_t> &src) {
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

		/// Reads pid, determines the path of the executable, and writes the filename.
		::BT::Error_Res_t getPath(int pid, ::BT::native_path_string_t& modPath)
		{
			try {
#if defined(__APPLE__) || defined(__MACH__)
				// We don't use sysctl here, unlike with the other unixes, because
				// we have better control of the buffer size.
				std::vector<char> exePath(my_path_max, 0);
				uint32_t len = exePath.size();
				if (_NSGetExecutablePath(exePath.data(), &len) != 0) {
					exePath.resize(len + 1);
					len++;
					_NSGetExecutablePath(exePath.data(), &len);
				}
				else {
					// resolve symlinks, ., .. if possible
					char *canonicalPath = realpath(exePath.data(), NULL);
					if (canonicalPath != NULL) {
						strncpy(exePath.data(), canonicalPath, len);
						free(canonicalPath);
					}
				}
				modPath = std::string(exePath.data());
#elif defined(__linux__)
				struct stat buf;
				// buf.st_size: The size of a symbolic link is the length of the pathname it contains, without a terminating null byte.
				int lret = lstat("/proc/self/exe", &buf);
				BT_POSIX_CHECK_OSERROR(lret == -1);
				std::vector<char> exePath(buf.st_size + 1, 0);
				ssize_t len = ::readlink("/proc/self/exe", exePath.data(), exePath.size());
				BT_POSIX_CHECK_OSERROR(len < -1);
				while (len > buf.st_size) // symlink increased in size between system calls. Ugh.
				{
					lret = lstat("/proc/self/exe", &buf);
					BT_POSIX_CHECK_OSERROR(lret == -1);
					exePath.resize(buf.st_size + 1);
					len = ::readlink("/proc/self/exe", exePath.data(), exePath.size());
					BT_POSIX_CHECK_OSERROR(len < -1);
				}
				modPath = std::string(exePath.data());
#elif defined(__unix__)
				// See https://www.freebsd.org/cgi/man.cgi?sysctl(3)
				char exePath[my_path_max];
				int mib[4];
				mib[0] = CTL_KERN;  mib[1] = KERN_PROC;  
				mib[2] = KERN_PROC_PATHNAME;  mib[3] = -1; // -1 implies the current process
				size_t len = sizeof(exePath);
				if (sysctl(mib, 4, exePath, &len, NULL, 0) != 0)
					exePath[0] = '\0';
				modPath = std::string(exePath);
				//appd = appPath.substr(0, appPath.find_last_of("/\\"));
#else
				BT_UNIMPLEMENTED;
#endif
			}
			catch (...) {
				std::throw_with_nested(
					BT_throw
					.add("Reason", "Failure in getting the path for a process.")
					.add("PID", pid)
				);
				BT_UNIMPLEMENTED;
			}
			return BT_POSIX_SUCCESS;
		}

		/// \brief Gets the CWD of the current process.
		/// \todo Fix potential bug where the cwd is too long.
		::BT::Error_Res_t getCWD(::BT::native_path_string_t& cwd)
		{
#if defined(BT_OS_LINUX) || defined(BT_OS_UNIX) || defined(BT_OS_MACOS)
			char ccwd[my_path_max];
			char* res = getcwd(ccwd, my_path_max);
			BT_POSIX_CHECK_OSERROR(!res);
			cwd = ::BT::native_path_string_t(ccwd);

			return BT_POSIX_SUCCESS;
#else
			BT_UNIMPLEMENTED;
#endif
		}

		/// Gets the environment of the current process.
		::BT::Error_Res_t getEnvironment(std::map<::BT::native_path_string_t, ::BT::native_path_string_t> &envs)
		{
			envs.clear();
			static std::mutex rtmutex;
			if (my_envs.size())
			{
				envs = my_envs;
				return BT_POSIX_SUCCESS;
			} else {
				std::lock_guard<std::mutex> lock(rtmutex);
#if defined(BT_OS_LINUX) || defined(BT_OS_UNIX) || defined(BT_OS_MACOS)
				// Conveniently, environ has the data.
				char **envp = environ;
				while (*envp)
				{
					std::string tosplit(*envp);
					size_t pos = tosplit.find_first_of("=");
					if ((pos != ::BT::native_path_string_t::npos) && (*envp[0] != '='))
					{
						std::string name, val;
						name = tosplit.substr(0, pos);
						val = tosplit.substr(pos + 1); // string is null-terminated, so I can do this.
						envs[name] = val;
					}

					++envp;
				}
				my_envs = envs;
				return BT_POSIX_SUCCESS;
#else
				BT_UNIMPLEMENTED;
#endif
			}
		}

		/// \brief Gets the command line of the current process.
		/// \todo Get this to work on macOS.
		::BT::Error_Res_t getCmdLine(std::vector<::BT::native_path_string_t> &cmdline)
		{
			cmdline.clear();
#if defined(BT_OS_LINUX)
			using namespace std;
			ostringstream procpath;
			int pid = getPID();
			procpath << "/proc/" << pid << "/cmdline";
			std::string scmdlinef = procpath.str();
			std::ifstream icmdline(scmdlinef.c_str());

			std::string cmdnulls;
			while (icmdline.good()) {
				const int length = 8192; // Loop reads in the command line in 8192 byte increments.
				char buf[length];
				icmdline.read(buf, length);
				cmdnulls.append(buf, icmdline.gcount());
			}

			// This loop splits the command line "string" on nulls.
			const char* pcur = cmdnulls.data();
			do {
				std::string tosplit(pcur);
				cmdline.push_back(tosplit);
				pcur += tosplit.size() + 1;
			} while (pcur[0] && // A double null indicates the end of the comand line.
				(pcur < cmdnulls.data() + cmdnulls.size())); // Just in case.

			return BT_POSIX_SUCCESS;
#elif defined(BT_OS_UNIX)
			// Use sysctl to get arguments
			// See https://www.freebsd.org/cgi/man.cgi?sysctl(3)
			std::string cmdnulls(my_path_max, '\0');
			int mib[4];
			mib[0] = CTL_KERN;  mib[1] = KERN_PROC;
			mib[2] = KERN_PROC_ARGS;
			//mib[3] = -1; // -1 implies the current process
			mib[3] = getpid(); // -1 implies current process on macos, but this
			// does not work on FreeBSD.
			size_t len = cmdnulls.size();
			int retsize = sysctl(mib, 4, cmdnulls.data(), &len, NULL, 0);
			BT_POSIX_CHECK_OSERROR(retsize < 0);

			// This loop splits the command line "string" on nulls.
			const char* pcur = cmdnulls.data();
			do {
				std::string tosplit(pcur);
				cmdline.push_back(tosplit);
				pcur += tosplit.size() + 1;
			} while (pcur[0] || // A double null indicates the end of the comand line.
				(pcur >= cmdnulls.data() + cmdnulls.size())); // Just in case.

			return BT_POSIX_SUCCESS;
#elif defined(BT_OS_MACOS)
			// macOS does not have KERN_PROC_ARGS, unfortunately.
			int mib[4];
			mib[0] = CTL_KERN;  mib[1] = KERN_PROC;
			mib[2] = KERN_PROC_PID;
			mib[3] = getpid(); // -1 implies current process on macos, but this
			struct kinfo_proc proc;
			size_t size = sizeof(proc);
			int retsize = sysctl(mib, sizeof mib, &proc, &size, NULL, 0);
			BT_POSIX_CHECK_OSERROR(retsize < 0);
			BT_UNIMPLEMENTED;
#else
			BT_UNIMPLEMENTED;
#endif
		}

		/// Are we running in an elevated session?
		::BT::Error_Res_t getElevated(bool &elevated)
		{
			elevated = false;
			return BT_POSIX_SUCCESS;
		}

		/// \brief Get start time
		/// \todo Get this to work properly on macOS. The returned start time is random.
		::BT::Error_Res_t getStartTime(int pid, ::BT::native_path_string_t &startTime)
		{
			try {
#if defined(BT_OS_LINUX)
				using namespace std;
				ostringstream procpath;
				procpath << "/proc/" << pid;
				string sproc = procpath.str();

				struct stat buf;
				int lret = lstat(sproc.c_str(), &buf);
				BT_POSIX_CHECK_OSERROR(lret == -1);
				// last modification time = buf.st_mtim;
				char sbuff[200];
				strftime(sbuff, sizeof sbuff, "%D %T", gmtime(&buf.st_mtim.tv_sec));

				startTime = std::string(sbuff);
#elif defined(BT_OS_UNIX)
				int mib[4] = { CTL_KERN, KERN_PROC, KERN_PROC_PID, pid };
				struct kinfo_proc proc;
				size_t size = sizeof(proc);
				if (sysctl(mib, 4, &proc, &size, NULL, 0) == 0) {
					// proc.kp_proc.p_starttime.tv_sec
					char sbuff[200];
					strftime(sbuff, sizeof sbuff, "%D %T",
							gmtime(&proc.ki_start.tv_sec));
					startTime = std::string(sbuff);
				}
#elif defined(BT_OS_MACOS)
				int mib[4] = { CTL_KERN, KERN_PROC, KERN_PROC_PID, pid };
				struct kinfo_proc proc;
				size_t size = sizeof(proc);
				if (sysctl(mib, 4, &proc, &size, NULL, 0) == 0) {
					// proc.kp_proc.p_starttime.tv_sec
					char sbuff[200];
					strftime(sbuff, sizeof sbuff, "%D %T",
							//gmtime(&proc.ki_start.tv_sec));
						gmtime(&proc.kp_proc.p_starttime.tv_sec));
					startTime = std::string(sbuff);
				}
				BT_UNIMPLEMENTED;
#else
				BT_UNIMPLEMENTED;
#endif
			}
			catch (...) {
				std::throw_with_nested(
					BT_throw
					.add("Reason", "Failure in getting the start time for a process.")
					.add("PID", pid)
				);
			}
			return BT_POSIX_SUCCESS;
		}

		/// Get current module
		::BT::Error_Res_t GetCurrentModule(void* hModule)
		{
			hModule = (void*)GetCurrentModule;
			return BT_POSIX_SUCCESS;
		}

		/// Get module path
		::BT::Error_Res_t getModulePath(void* mod, ::BT::native_path_string_t &modPath) // = nullptr
		{
			try {
				Dl_info info;
				void* addr = mod;
				if (!addr) addr = (void*)getModulePath;
				if (dladdr(addr, &info))
				{
					modPath = std::string(info.dli_fname);
				}
				else {
					return BT::Error_Res_t(-2, "Unknown dladdr error.");
				}
			}
			catch (...) {
				std::throw_with_nested(
					BT_throw
					.add("Reason", "Failure in getting a module's path")
				);
			}
			return BT_POSIX_SUCCESS;
		}

		/// \brief List of the modules in the current program.
		/// \see moduleCallback on Linux to see why this is needed at this level of scope.
		std::map<std::string, std::string> mmods;
#if defined(__linux__) || defined(__unix__)
		/// \note Keeping function definition this way to preserve compatibility with gcc 4.7
		int moduleCallback(dl_phdr_info* info, size_t, void*)
		{
			std::string name(info->dlpi_name);
			if (!name.size()) return 0;
			mmods[name] = name;
			return 0;
		}
#endif

		/// Get loaded modules
		::BT::Error_Res_t getLoadedModules(::BT::ModuleInfo_t<> &out)
		{
			static std::mutex rtmutex;
			std::lock_guard<std::mutex> lock(rtmutex);

			mmods.clear();
#if defined(__linux__) || defined(__unix__)
			dl_iterate_phdr(moduleCallback, NULL);
#elif defined(__APPLE__) || defined(__MACH__)
			uint32_t count = _dyld_image_count();
			for (uint32_t i = 0; i < count; ++i) {
				std::string modName(_dyld_get_image_name(i));
				char cmodPath[my_path_max];
				char* ccmodPath = realpath(modName.c_str(), NULL);
				if (ccmodPath != NULL) {
					strncpy(cmodPath, ccmodPath, my_path_max);
					free(ccmodPath);
				}
				std::string modPath(ccmodPath);
				mmods[modPath] = modPath;
			}
#else
			BT_UNIMPLEMENTED;
#endif
			out.clear();
			for (const auto& m : mmods)
				out.push_back(m.first);
			return BT_POSIX_SUCCESS;
		}

		/// Get user name
		::BT::Error_Res_t getUserName(::BT::native_path_string_t &username) {
			std::map<::BT::native_path_string_t, ::BT::native_path_string_t> envs;
			::BT::Error_Res_t enverr = getEnvironment(envs);
			if (enverr.first == 0) {
				// Check first the environment variables
				// for USER, LOGNAME 
				if (envs.count(BT_NATIVE("USER"))) {
					username = envs[BT_NATIVE("USER")];
					return BT_POSIX_SUCCESS;
				} else if (envs.count(BT_NATIVE("LOGNAME"))) {
					username = envs[BT_NATIVE("LOGNAME")];
					return BT_POSIX_SUCCESS;
				}
			}
			// If the environment does not provide the username,
			// check getlogin/getlogin_r.
			// NOTE: these functions are buggy on various platforms and may fail.
			// See https://github.com/Microsoft/WSL/issues/888 for an example.
			{
				const size_t len = 65536;
				char hname[len]; // A buffer of size len (65536 bytes)
				int res = 0;
#if defined(_POSIX_C_SOURCE)
# if _POSIX_C_SOURCE >= 199506L
				res = getlogin_r(hname, len);
				if (!res) { username = std::string(hname); return BT_POSIX_SUCCESS;}
# else
				char* charres = NULL;
				charres = getlogin();
				if (charres) { username = std::string(charres); return BT_POSIX_SUCCESS; }
# endif
#endif
			}

			// If we still don't have an answer, check getpwuid_r/getpwuid.
			{
#if defined(_POSIX_C_SOURCE)
# if _POSIX_C_SOURCE >= 1 || _XOPEN_SOURCE || _BSD_SOURCE || _SVID_SOURCE || _POSIX_SOURCE
				const size_t len = 65536;
				char hname[len]; // A buffer of size len (65536 bytes)
				int res = 0;
				uid_t uid = geteuid();
				struct passwd ps;
				struct passwd* pres = nullptr; // A pointer to the result (or NULL on failure)
				res = getpwuid_r(uid, &ps, hname, len, &pres);
				// res = 0 on success, and nonzero on error. Subsumed by pres, because it is 
				// only set to non-null on success.
				// Adding res==0 check explicitly to suppress a build warning.
				if (pres && (res==0)) {
					username = std::string(ps.pw_name);
					return BT_POSIX_SUCCESS;
				}
# else
				// getpwuid
				uid_t uid = geteuid();
				struct passwd* ps = getpwuid(uid);
				if (ps) { username = std::string(ps->pw_name); return BT_POSIX_SUCCESS; }
# endif
#endif
			}

			BT_UNIMPLEMENTED;
			return BT_POSIX_SUCCESS;
		}

		/// Get computer name
		::BT::Error_Res_t getComputerName(::BT::native_path_string_t &username) {
			const size_t len = 65536;
			char hname[len]; // A buffer of size len (65536 bytes)
			int res = 0; // A return code
			res = gethostname(hname, len); // May be empty
			BT_POSIX_CHECK_ERRNO(res!=0);
			BT_POSIX_CHECK_OSERROR(hname[0] == '\0');
			username = ::BT::native_path_string_t(hname);
			return BT_POSIX_SUCCESS;
		}

		/// Get app config dir
		::BT::Error_Res_t getAppConfigDir(::BT::native_path_string_t &appConfigDir) {
			std::map<::BT::native_path_string_t, ::BT::native_path_string_t> envs;
			::BT::Error_Res_t enverr = getEnvironment(envs);
			if (enverr.first == 0) {
				// Check first the environment variables
				// for XDG_CONFIG_HOME
				if (envs.count(BT_NATIVE("XDG_CONFIG_HOME"))) {
					appConfigDir = envs[BT_NATIVE("XDG_CONFIG_HOME")];
					return BT_POSIX_SUCCESS;
				}
			}
			// If the environment variable check failed:
			{
				::BT::native_path_string_t homeDir;
				::BT::Error_Res_t home_error = getHomeDir(homeDir);
				if (home_error.first == 0) {
					appConfigDir = homeDir;
					appConfigDir.append("/.config");
					return BT_POSIX_SUCCESS;
				}
			}


			BT_UNIMPLEMENTED;
			return BT_POSIX_SUCCESS;
		}

		/// Get user home dir
		::BT::Error_Res_t getHomeDir(::BT::native_path_string_t &homeDir) {
			std::map<::BT::native_path_string_t, ::BT::native_path_string_t> envs;
			::BT::Error_Res_t enverr = getEnvironment(envs);
			if (enverr.first == 0) {
				// Check first the environment variables
				// for HOME
				if (envs.count(BT_NATIVE("HOME"))) {
					homeDir = envs[BT_NATIVE("HOME")];
					return BT_POSIX_SUCCESS;
				}
			}

			// If the environment check fails, then use getpwuid_r/getpwuid.
#if defined(_POSIX_C_SOURCE) || defined(_BSD_SOURCE) || defined(_SVID_SOURCE)
			struct passwd pw, *pwp;
			const size_t len = 65536;
			char hname[len]; // A buffer of size len (65536 bytes)

			int res = getpwuid_r(geteuid(), &pw, hname, len, &pwp);
			if (res == 0) {
				const char *chomedir = pw.pw_dir;
				homeDir = std::string(chomedir);
				return BT_POSIX_SUCCESS;
			}
#else
			struct passwd* ps = getpwuid(geteuid());
			if (ps) {
				homeDir = std::string(ps->pw_dir);
				return BT_POSIX_SUCCESS;
			}
#endif

			BT_UNIMPLEMENTED;
			return BT_POSIX_SUCCESS;
		}


#undef BT_POSIX_CHECK
#undef BT_POSIX_CHECK_ERRNO
#undef BT_POSIX_CHECK_OSERROR
#undef BT_POSIX_SUCCESS
	}
}
