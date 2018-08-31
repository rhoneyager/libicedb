/**
* \brief Contains registry functions for extending functionality through DLLs.
*
* Contains both general and OS-specific functions.
**/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <boost/filesystem.hpp>
#include <boost/version.hpp>
#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/tokenizer.hpp>
#include <mutex>
#include "../icedb/logging.hpp"
#include "../icedb/error.hpp"
#include "../icedb/misc/os_functions.hpp"
#include "../icedb/splitSet.hpp"
#include "../icedb/dlls.hpp"
#include "../icedb/versioning/versioning.hpp"

#ifdef _WIN32
#include "windows.h"
#endif
#ifdef __unix__
#include <dlfcn.h>
#endif

#ifdef _WIN32
typedef HINSTANCE dlHandleType;
#endif
#ifdef __APPLE__
typedef void* dlHandleType;
#endif
#ifdef __unix__
typedef void* dlHandleType;
#endif

namespace icedb {
	namespace registry {
		/// Recursive and single-level DLL loading paths
		std::set<boost::filesystem::path> searchPathsRecursive, searchPathsOne;

	}
}

namespace {
	std::mutex m_hooks;
	std::map<void*, std::string> hookTable;
	/// Lists the paths of all loaded dlls
	std::set<std::string> DLLpathsLoaded;
	/// DLL information structure
	std::vector<icedb::registry::DLLpreamble> preambles;
	/// Container for the handles of all loaded dlls
	std::vector<std::shared_ptr<icedb::registry::DLLhandle> > handles;


	//bool autoLoadDLLs = true;

	/// Checks if a file is a dll file
	bool isDynamic(const boost::filesystem::path &f)
	{
		using namespace boost::filesystem;
		std::string s = f.string(); // Faster, though less accurate
		if (s.find(".so") != std::string::npos) return true;
		if (s.find(".dll") != std::string::npos) return true;
		if (s.find(".dylib") != std::string::npos) return true;
		/*
		boost::filesystem::path p = f;
		while (p.has_extension())
		{
		path ext = p.extension();
		if (ext.string() == ".so" || ext.string() == ".dll"
		|| ext.string() == ".dylib") return true;
		p.replace_extension();
		}
		*/
		return false;
	};

	/// Checks if a dll file matches the build settings of the icedb library, by file path
	bool correctVersionByName(const std::string &s)
	{
		std::string slower = boost::filesystem::path(s).filename().string();
		std::transform(slower.begin(), slower.end(), slower.begin(), ::tolower);

		using namespace std;
		// The DLL case probably sould never occur.
#ifdef _DLL
		if (slower.find("static") != string::npos) return false;
#else
		if (slower.find("dynamic") != string::npos) return false;
#endif
		// Debug vs release dlls
		std::string buildtype; // defined in cmake config (addlib.cmake)

		// TODO: move these definitions to an appropriate header?
#define BUILDTYPE_Debug 1
#define BUILDTYPE_Release 2
#define BUILDTYPE_MinSizeRel 3
#define BUILDTYPE_RelWithDebInfo 4

#if BUILDTYPE == BUILDTYPE_Debug
		buildtype = "Debug";
#elif BUILDTYPE == BUILDTYPE_Release
		buildtype = "Release";
#elif BUILDTYPE == BUILDTYPE_MinSizeRel
		buildtype = "MinSizeRel";
#elif BUILDTYPE == BUILDTYPE_Release
		buildtype = "Release";
#else
		buildtype = BUILDCONF;
#endif
		std::transform(buildtype.begin(), buildtype.end(), buildtype.begin(), ::tolower);

		if (slower.find(buildtype) == string::npos) return false;
		//{
		//	if (slower.find("release") != string::npos) return false;
		//	if (slower.find("minsizerel") != string::npos) return false;
		//	if (slower.find("debug") != string::npos) return false;
		//	if (slower.find("relwithdebinfo") != string::npos) return false;
		//}

		/// Check for x86 vs x64
#if __amd64 || _M_X64
		if (slower.find("x86") != string::npos) return false;
#else
		if (slower.find("x64") != string::npos) return false;
#endif
		/// \todo Check against windows system crt vs version-specific one
		/// \todo Figure out how to get crt lib name from loaded dll
		return true;
	}

	/**
	* \brief Determines the search paths for dlls in the icedb.conf file and in environment variables
	*
	**/
	void constructSearchPaths(bool use_cmake = false, bool use_icedb_conf = true, bool use_environment = true)
	{
		using std::vector;
		using std::set;
		using std::string;
		
		using namespace icedb;
		using namespace icedb::os_functions;
		auto info = icedb::os_functions::getInfo(os_functions::getPID());

		// Default locations
		// Install path apps

		//icedb::registry::searchPathsRecursive.emplace(boost::filesystem::path("plugins"));
		//icedb::registry::searchPathsRecursive.emplace(boost::filesystem::path("../plugins"));
		//searchPathsRecursive.emplace(boost::filesystem::path("../../plugins"));
		// Not in install path apps
		//icedb::registry::searchPathsRecursive.emplace(boost::filesystem::path("../../plugins"));
		//icedb::registry::searchPathsRecursive.emplace(boost::filesystem::path("../../../plugins"));
		//icedb::registry::searchPathsRecursive.emplace(boost::filesystem::path("../../../../plugins"));

		// Relative to application
		// Install path apps
		//boost::filesystem::path appBin(icedb::getPath(info.get()));
		//appBin.remove_filename();
		//icedb::registry::searchPathsRecursive.emplace(appBin / "plugins");
		//icedb::registry::searchPathsRecursive.emplace( appBin / "../plugins" );
		// Build path apps (linux)
		//icedb::registry::searchPathsRecursive.emplace( appBin / "../../plugins" );

		// Relative to library
		auto modinfo = getModuleInfo((void*)_ICEDB_dllPluginBase);
		boost::filesystem::path libpath(getPath(modinfo.get()));
		libpath.remove_filename();

		// Check where libexec is relative to lib. Are they in the same location?
		//if (std::string(libicedb_libexecdir) == std::string(libicedb_libdir))
		//	icedb::registry::searchPathsOne.emplace(libpath / "icedb" / "plugins");
		//else {
		icedb::registry::searchPathsOne.emplace(
			libpath / "icedb" / "plugins");
		icedb::registry::searchPathsOne.emplace(
			libpath / ".." / boost::filesystem::path(libicedb_libexecdir) / "icedb" / "plugins");
		//}
		
		// Checking environment variables
		if (use_environment)
		{

			size_t sEnv = 0;
			const char* cenv = getEnviron(info.get(), sEnv);
			std::string env(cenv, sEnv);

			//icedb::processInfo info = icedb::getInfo(icedb::getPID());
			std::map<std::string, std::string> mEnv;
			splitSet::splitNullMap(env, mEnv);
			//std::vector<std::string> mCands;
			auto searchFunc = [](const std::pair<std::string, std::string> &pred, const std::string &mKey)
			{
				std::string key = pred.first;
				std::transform(key.begin(), key.end(), key.begin(), ::tolower);
				if (key == mKey) return true;
				return false;
			};
			auto searchEnviron = [&](const std::string &evar, std::set<boost::filesystem::path> &res)
			{
				auto it = std::find_if(mEnv.cbegin(), mEnv.cend(),
					std::bind(searchFunc, std::placeholders::_1, evar));
				if (it != mEnv.cend())
				{
					typedef boost::tokenizer<boost::char_separator<char> >
						tokenizer;
					boost::char_separator<char> sep(",;");

					std::string ssubst;
					tokenizer tcom(it->second, sep);
					for (auto ot = tcom.begin(); ot != tcom.end(); ot++)
					{
						using namespace boost::filesystem;
						path testEnv(it->second);
						if (exists(testEnv))
						{
							res.emplace(testEnv);
						}
					}
				}
			};

			searchEnviron("icedb_plugins_DIR", registry::searchPathsRecursive);
			searchEnviron("icedb_dlls_recursive", registry::searchPathsRecursive);
			searchEnviron("icedb_dlls_onelevel", registry::searchPathsOne);
		}
	}

}

namespace icedb
{
	namespace registry
	{
		
		void add_hook_table(const char* tempsig, void* store) {
			std::lock_guard<std::mutex> lock(m_hooks);
			hookTable[store] = std::string(tempsig);
		}

		void dump_hook_table(std::ostream &out) {
			using namespace icedb::os_functions;
			std::lock_guard<std::mutex> lock(m_hooks);
			auto h = getModuleInfo((void*)dump_hook_table);
			out << "Hook table for icedb dll at "
				<< getPath(h.get()) << std::endl
				<< "Store\t - \tSignature\n";
			for (const auto &i : hookTable)
			{
				out << i.first << "\t - \t" << i.second << std::endl;
			}
		}

		using icedb::registry::searchPathsOne;
		using icedb::registry::searchPathsRecursive;

		class DLLhandle;

		class DLLhandleImpl
		{
			friend class DLLhandle;
			DLLhandle *parent;
			std::string fname;
			dlHandleType dlHandle;
			std::shared_ptr<const registry::dllValidatorSet> validators;
			DLLhandleImpl(std::shared_ptr<const registry::dllValidatorSet> dvs,
				DLLhandle *p)
				: dlHandle(nullptr), validators(dvs), parent(p) {
			}
			void close() {
				if (!dlHandle) return;
				//BOOST_LOG_SEV(m_reg, normal) << "Closing dll " << fname << "." << "\n";
				DLLpathsLoaded.erase(fname);
#ifdef __unix__
				dlclose(this->dlHandle);
#endif
#ifdef _WIN32
				FreeLibrary(this->dlHandle);
#endif
				//BOOST_LOG_SEV(m_reg, normal) << "Closed dll " << fname << "." << "\n";
			}
			bool isOpen() const { if (dlHandle) return true; return false; }
			void* getSym(const char* symbol, bool critical = false) const 
			{
				if (dlHandle == NULL)
				{
					ICEDB_throw(icedb::error::error_types::xHandleNotOpen)
						.add<std::string>("FileName", fname);
				}
				void* sym = nullptr;
#ifdef __unix__
				sym = dlsym(dlHandle, symbol);
#endif
#ifdef _WIN32
				sym = GetProcAddress(dlHandle, symbol);
#endif
				if (!sym)
				{
#ifdef _WIN32
					long long errcode = 0;
					errcode = (long long)GetLastError();
#endif
					if (critical)
						ICEDB_throw(icedb::error::error_types::xSymbolNotFound)
						.add<std::string>("FileName", fname)
						.add<std::string>("Symbol_name", symbol)
#ifdef _WIN32
						.add("Windows_Error_Code", errcode)
#endif
						;
				}
				return (void*)sym;
			}
			void open(const std::string &filename, bool critical = false)
			{
				if (DLLpathsLoaded.count(filename))
				{
					if (critical)
						ICEDB_throw(icedb::error::error_types::xDuplicateHook)
						.add("is_critical", critical)
						.add("file_name", filename)
						.add("file_name_b", fname);
					ICEDB_log("dlls", logging::ICEDB_LOG_DEBUG_WARNING, "DLL already loaded. " << filename);
					return;
				}
				if (dlHandle)
				{
					ICEDB_throw(icedb::error::error_types::xHandleInUse)
						.add("is_critical", critical)
						.add("file_name", filename)
						.add("file_name_b", fname);
					ICEDB_log("dlls", logging::ICEDB_LOG_DEBUG_WARNING, "DLLhandleImpl already had a loaded DLL, when trying to load " << filename);
					return;
				}
				fname = filename;

				auto attemptLoad = [&]() -> bool {

#ifdef __unix__ // Indicates that DLSYM is provided (unix, linux, mac, etc. (sometimes even windows))
					//Check that file exists here
					this->dlHandle = dlopen(filename.c_str(), RTLD_LAZY);
					const char* cerror = dlerror();
					if (cerror)
					{
						if (critical)
							ICEDB_throw(icedb::error::error_types::xDLLerror)
							.add("is_critical", critical)
							.add("file_name", filename)
							.add("otherErrorText", std::string(cerror));
						ICEDB_log("dlls", logging::ICEDB_LOG_DEBUG_WARNING, "dlopen for " << filename << " failed "
							"with error " << cerror);
						return false;
					}
#endif
#ifdef _WIN32
					this->dlHandle = LoadLibrary(filename.c_str());
					// Could not open the dll for some reason
					if (this->dlHandle == NULL)
					{
						DWORD err = GetLastError();
						if (critical)
							ICEDB_throw(icedb::error::error_types::xDLLerror)
							.add("is_critical", critical)
							.add("file_name", filename)
							.add("otherErrorText", "LoadLibrary")
							.add("otherErrorCode", err);
						ICEDB_log("dlls", logging::ICEDB_LOG_DEBUG_WARNING, "LoadLibrary for " << filename << " failed "
							"with error " << err);
						return false;
					}
#endif
					return true;
				};
				bool res = attemptLoad();
				if (!res) return;

				DLLpathsLoaded.insert(filename);

				ICEDB_log("dlls", logging::ICEDB_LOG_INFO, "Validating dll " << filename << ".");
				bool res2 = validators->validate(parent, critical);

				if (!res2) {
					ICEDB_log("dlls", logging::ICEDB_LOG_DEBUG_WARNING, "Cannot load dll " << filename
						<< ", as it failed validation checks!");
					close();
					return;
				}

				// Finally, get and invoke the initialization function

				dllInitResult(*vfStart)() = (dllInitResult(*)()) getSym("dllStart", critical);

				if (vfStart) {
					// It's a C function, so it does not raise exceptions. Might not pass exceptions either.
					dllInitResult res;
					res = vfStart();

					if (DUPLICATE_DLL == res) {
						if (critical)
							ICEDB_throw(icedb::error::error_types::xDuplicateHook)
							.add("is_critical", critical)
							.add("file_name", filename);
						ICEDB_log("dlls", logging::ICEDB_LOG_DEBUG_WARNING, "Cannot load dll " << filename
							<< ", as it already loaded!");
						close();
						return;
					}
					else if (SUCCESS != res) {
						if (critical)
							ICEDB_throw(icedb::error::error_types::xDLLerror)
							.add("is_critical", critical)
							.add("file_name", filename)
							.add("symbol_name", "dllStart");
							//.add("otherErrorCode", res);
						ICEDB_log("dlls", logging::ICEDB_LOG_DEBUG_WARNING, "Cannot load dll " << filename
							<< ", as its dllStart function failed!");
						close();
						return;
					}
					//} catch (icedb::error::xDuplicateHook &e) {
					// Already logged duplicate detected message.
					//	if (critical)
					//		throw e << icedb::error::is_Critical(critical);
					//	return false;
					//}
				}
				else {
					// Will not reach this point if critical flag is set.
					if (critical)
						ICEDB_throw(icedb::error::error_types::xSymbolNotFound)
						.add("is_critical", critical)
						.add("file_name", filename)
						.add("symbol_name", "dllStart")
						.add("otherErrorCode", res);
					ICEDB_log("dlls", logging::ICEDB_LOG_DEBUG_WARNING, "Cannot load dll " << filename
						<< ", as it is missing its main entry point!");
					close();
					return;
				}
				
			}
		public:
			~DLLhandleImpl() {
				close();
			}
		};

		DLLhandle::DLLhandle(const std::string &filename, 
			std::shared_ptr<const registry::dllValidatorSet> dvs,
			bool critical)
		{
			_p = std::shared_ptr<DLLhandleImpl>(new DLLhandleImpl(dvs, this));
			open(filename, critical);
		}
		DLLhandle::DLLhandle()
		{
			_p = std::shared_ptr<DLLhandleImpl>(new DLLhandleImpl(
				icedb::registry::dllValidatorSet::getDefault(), this));
		}
		void DLLhandle::open(const std::string &filename, bool critical)
		{ _p->open(filename, critical); }
		void DLLhandle::close() { _p->close(); }
		DLLhandle::~DLLhandle() {}
		void* DLLhandle::getSym(const char* symbol, bool critical) const
		{ return _p->getSym(symbol, critical); }
		bool DLLhandle::isOpen() const { return _p->isOpen(); }
		const char* DLLhandle::filename() const
		{
			return _p->fname.c_str();
		}

		dllValidator::dllValidator() {}
		dllValidator::~dllValidator() {}
		dllValidatorSet::dllValidatorSet() {}
		dllValidatorSet::~dllValidatorSet() {}

		
		
		class dllValidatorIcedb : public dllValidator {
		public:
			dllValidatorIcedb() {}
			~dllValidatorIcedb() {}
			virtual const char* validationSymbol() const { const char* r = "dlVer"; return r; }
			virtual bool validate(void* func, bool critical) const
			{
				void(*fVer)(icedb::versioning::versionInfo&, void**) =
					(void(*)(icedb::versioning::versionInfo&, void**)) func;
				using namespace icedb::os_functions;
				auto h = getModuleInfo((void*)func);
				std::string filename(getPath(h.get()));
				h.reset();
				//dllInitResult(*vfStart)() = nullptr;
				//void* vfStarta = nullptr;
				if (fVer) {
					using namespace icedb::versioning;
					versionInfo_p myVer;
					void* rdcheck = nullptr; // Check to make sure the same icedb functions are being used.
					myVer = getLibVersionInfo();
					std::shared_ptr<versionInfo> dllVer(new versionInfo);
					fVer(*(dllVer.get()), &rdcheck);
					auto verres = compareVersions(dllVer, myVer);
					//std::ostringstream dver, mver;
					//debug_preamble(dllVer, dver);
					//debug_preamble(myVer, mver);
					//std::string sdver = dver.str(), smver = mver.str();
					if (verres < ICEDB_VER_COMPATIBLE_2) {
						if (critical)
							ICEDB_throw(icedb::error::error_types::xDLLversionMismatch)
							.add("is_critical", critical)
							.add("file_name", filename);
						ICEDB_log("dlls", logging::ICEDB_LOG_NOTIFICATION, "dll validation failed "
							"with verres " << verres);
						return false;
					}
					//else if (verres != EXACT_MATCH) {
					//}

					// Check that the DLL's icedb function calls are really to the correct code.
					void *mdcheck = (void*) &(icedb_registry_register_dll);
					if (rdcheck != mdcheck) {
						using namespace icedb::registry;
						using namespace icedb;
						using namespace icedb::os_functions;
						auto h = getModuleInfo((void*)mdcheck);
						std::string myPath(getPath(h.get()));
						auto ho = getModuleInfo((void*)rdcheck);
						std::string rPath;
						if (rdcheck) rPath = (getPath(ho.get())); else rPath = "Unknown";
						h.reset();
						ho.reset();

						if (critical)
							ICEDB_throw(icedb::error::error_types::xDLLversionMismatch)
							.add("is_critical", critical)
							.add("file_name", filename);
						ICEDB_log("dlls", logging::ICEDB_LOG_NOTIFICATION, "DLLversionMismatch encountered. "
						"The plugin refers to the wrong icedb release!");

						return false;
					}
				}
				else {
					if (critical)
						ICEDB_throw(icedb::error::error_types::xDLLversionMismatch)
						.add("is_critical", critical)
						.add("file_name", filename);
					ICEDB_log("dlls", logging::ICEDB_LOG_NOTIFICATION, "DLL validation failure! Is this an icedb plugin? "
						"The version comparison function cannot be found.");
					return false;
				}
				return true;
			}
		};

		std::shared_ptr<const dllValidator> dllValidator::genDefaultValidator() {
			static std::shared_ptr<const dllValidator> def(new dllValidatorIcedb());
			return def;
		}

		std::shared_ptr<dllValidatorSet> dllValidatorSet::generate() {
			return std::shared_ptr<dllValidatorSet>(new dllValidatorSet);
		}
		std::shared_ptr<const dllValidatorSet> dllValidatorSet::getDefault()
		{
			static bool gen = false;
			static std::shared_ptr<const dllValidatorSet> def;
			if (!gen)
			{
				std::shared_ptr<dllValidatorSet> a = generate();
				a->append(dllValidator::genDefaultValidator());
				def = a;
			}
			return def;
		}
		void dllValidatorSet::append(std::shared_ptr<const dllValidator> v) {
			validators.push_back(v);
		}
		bool dllValidatorSet::validate(const DLLhandle* p, bool critical) const {
			for (const auto &i : validators) {
				// First, get the dll symbol function
				const char* sym = i->validationSymbol();
				// Get the address of the function
				void* func = p->getSym(sym, critical);

				if (func) {
					bool res = false;
					res = i->validate(func, critical);
					if (!res) return false;
				} else return false;
			}
			return true;
		}

		bool findPath(std::set<boost::filesystem::path> &matches, const boost::filesystem::path &expr,
			const std::set<boost::filesystem::path> &searchPaths, bool recurse)
		{
			using namespace boost::filesystem;
			using namespace std;

			using boost::filesystem::path;
			path pexpr(expr);

			for (const auto &p : searchPaths)
			{
				if (!exists(p)) continue;
				vector<path> recur;
				//recur.reserve(50000);
				if (!is_directory(p))
					recur.push_back(p);
				else {
					if (recurse)
						copy(recursive_directory_iterator(p, symlink_option::recurse),
						recursive_directory_iterator(), back_inserter(recur));
					else
						copy(directory_iterator(p),
						directory_iterator(), back_inserter(recur));
				}
				for (const auto &r : recur)
				{
					/// \todo Debug expression evaluation
					if (absolute(r, p) == pexpr) matches.emplace(r);
				}

			}

			if (matches.size()) return true;
			return false;
		}

		void searchDLLs(std::vector<std::string> &dlls, const std::set<boost::filesystem::path> &searchPaths, bool recurse)
		{
			using namespace boost::filesystem;
			using namespace std;

			for (const auto &sbase : searchPaths)
			{
				size_t sDlls = dlls.size();
				path base(sbase);
				//base = icedb::fs::expandSymlink<path, path>(base);
				if (!exists(base)) continue;
				if (is_regular_file(base)) dlls.push_back(base.string());
				else if (is_directory(base))
				{
					vector<path> recur;
					//recur.reserve(50000);
					if (recurse)
						copy(recursive_directory_iterator(base, symlink_option::recurse),
						recursive_directory_iterator(), back_inserter(recur));
					else
						copy(directory_iterator(base),
						directory_iterator(), back_inserter(recur));
					for (const auto &p : recur)
					{
						if (!is_regular_file(p)) continue;
						// Convenient function to recursively check extensions to see if one is so or dll.
						// Used because of versioning.

						if (isDynamic(p))
						{
							// Check to see if build type is present in the path
							std::string slower = p.string();
							// Convert to lower case and do matching from there (now in func)
							//std::transform(slower.begin(), slower.end(), slower.begin(), ::tolower);

							if (correctVersionByName(slower)) {
								dlls.push_back(p.string());
							}
						}
					}
				}
				size_t eDlls = dlls.size();
			}
		}

		void searchDLLs(std::vector<std::string> &dlls)
		{
			using namespace boost::filesystem;
			using namespace std;

			searchDLLs(dlls, searchPathsRecursive, true);
			searchDLLs(dlls, searchPathsOne, false);
		}

		void loadDLLs() {
			std::vector<std::string> dlls;
			searchDLLs(dlls);
			loadDLLs(dlls);
		}

		void loadDLLs(const std::vector<std::string> &dlls, std::shared_ptr<const dllValidatorSet> dvs, bool critical)
		{
			for (const auto &dll : dlls)
				loadDLL(dll, dvs);
		}

		void loadDLL(const std::string &filename, std::shared_ptr<const dllValidatorSet> dvs, bool critical)
		{
			auto doLoad = [&](const std::string &f, bool critical)
			{
				std::shared_ptr<DLLhandle> h(new DLLhandle(f, dvs, critical));
				if (h->isOpen()) {
					std::lock_guard<std::mutex> lock(m_hooks);
					handles.push_back(h);
				}
				else {
					// If critical and a throw condition occurs, then it is already logged and this is not reached.
				}
			};
			// Search for the dll
			using namespace boost::filesystem;
			path p(filename);
			ICEDB_log("dlls", logging::ICEDB_LOG_NORMAL, "Loading dll " << filename << ".");
			//if (p.is_absolute())
			{
				if (exists(p)) doLoad(p.string(), critical);
				else {
					ICEDB_throw(icedb::error::error_types::xMissingFile)
						.add("File_name", p.string())
						.add("Reason", "Cannot find dll to load");
				}
			}
		}

		void printDLLs(std::ostream &out)
		{
			out << "icedb DLL registry table:\n--------------------\n";
			for (const auto p : preambles)
			{
				out << "Name:\t\t\t" << p.name << "\n"
					<< "UUID:\t\t\t" << p.uuid << "\n"
					<< "Description:\t" << p.description << "\n";

			}
			out << std::endl;
			out << "DLL paths loaded:\n----------------\n";
			for (const auto p : DLLpathsLoaded)
				out << p << "\n";
			out << std::endl;

			/*out << "\nHook Table:\n-----------------\nClass\tTopic\tPointer";
			for (const auto hm : hookRegistry)
			{
			out << hm.first << "\n";
			for (const auto h : hm.second)
			{
			out << "\t" << h.first << "\t" << h.second << "\n";
			}
			}
			*/

			out << std::endl;
		}

		void printDLLsearchPaths(std::ostream &out)
		{
			out << "icedb DLL registry recursive search paths:\n--------------------\n";
			for (const auto p : searchPathsRecursive)
			{
				out << p.string() << "\n";
			}
			out << std::endl;
			out << "icedb DLL registry one-level search paths:\n--------------------\n";
			for (const auto p : searchPathsOne)
			{
				out << p.string() << "\n";
			}
			out << std::endl;
		}

	}

	void add_options(
		boost::program_options::options_description &cmdline,
		boost::program_options::options_description &config,
		boost::program_options::options_description &hidden)
	{
		namespace po = boost::program_options;
		using std::string;

		
		cmdline.add_options()
			("config-file", po::value<string>(), "Read a file containing program options, such as metadata. Options are specified, once per line, as OPTION=VALUE pairs.")
			;

		config.add_options()
			;

		hidden.add_options()
			("dll-load-onelevel", po::value<std::vector<std::string> >()->multitoken(),
				"Specify dlls to load. If passed a directory, it loads all dlls present (one-level). ")
				("dll-load-recursive", po::value<std::vector<std::string> >()->multitoken(),
					"Specify dlls to load. If passed a directory, it loads all dlls present (recursing). ")
			//("dll-no-default-locations", "Prevent non-command line dll locations from being read")
					("print-dll-loaded", "Prints the table of loaded DLLs.")
			("print-dll-search-paths", "Prints the search paths used when loading dlls.")
			("console-log-threshold", po::value<int>()->default_value(4), "Set threshold for logging output to console. 0 is DEBUG_2, 7 is CRITICAL.")
			("debug-log-threshold", po::value<int>()->default_value(0), "Set threshold for logging output to an attached debugger. 0 is DEBUG_2, 7 is CRITICAL.")
			("log-file", po::value<std::string>(), "Set this to log debugging output to a file.")
			("help-all", "Print all available help.")
			;
	}

	void handle_config_file_options(
		boost::program_options::options_description &opts,
		boost::program_options::variables_map &vm)
	{
		if (vm.count("config-file")) {
			namespace po = boost::program_options;
			using namespace std;
			string configfile = vm["config-file"].as<string>();
#if BOOST_VERSION < 104600
			// For RHEL's really old Boost distribution
			ifstream ifs(configfile.c_str());
			if (!ifs) {
				cout << "Could no open the response file\n";
				return 1;
			}
			// Read the whole file into a string
			stringstream ss;
			ss << ifs.rdbuf();
			// Split the file content
			using namespace boost;
			char_separator<char> sep(" \n\r");
			tokenizer<char_separator<char> > tok(ss.str(), sep);
			vector<string> args;
			copy(tok.begin(), tok.end(), back_inserter(args));
			// Parse the file and store the options
			po::store(po::command_line_parser(args).options(opts).run(), vm);
#else
			// For modern systems
			po::store(po::parse_config_file<char>(configfile.c_str(), opts, false), vm);
#endif
			po::notify(vm);
		}
		if (vm.count("help-all")) {
			using namespace std;
			cout << opts << endl;
			exit(1);
		}
	}

	void process_static_options(
		boost::program_options::variables_map &vm)
	{
		namespace po = boost::program_options;
		using std::string;
		using namespace icedb::registry;

		icedb::logging::log_properties lps;
		if (vm.count("log-file")) lps.logFile = vm["log-file"].as<std::string>();
		lps.consoleLogThreshold = vm["console-log-threshold"].as<int>();
		lps.debuggerLogThreshold = vm["debug-log-threshold"].as<int>();
		icedb::logging::setupLogging(0, nullptr, &lps);

		//if (vm.count("dll-no-default-locations"))
		//	autoLoadDLLs = false;

		if (vm.count("dll-load-onelevel"))
		{
			std::vector<std::string> sPaths = vm["dll-load-onelevel"].as<std::vector<std::string> >();
			for (const auto s : sPaths)
			{
				searchPathsOne.emplace(s);
			}
		}

		if (vm.count("dll-load-recursive"))
		{
			std::vector<std::string> sPaths = vm["dll-load-recursive"].as<std::vector<std::string> >();
			for (const auto s : sPaths)
			{
				searchPathsRecursive.emplace(s);
			}
		}

		constructSearchPaths(false, true, true);

		if (vm.count("print-dll-search-paths")) {
			printDLLsearchPaths(std::cerr);
			exit(0);
		}

		std::set<boost::filesystem::path> rPaths, oPaths;
		findPath(rPaths, boost::filesystem::path("default"), searchPathsRecursive, true);
		findPath(oPaths, boost::filesystem::path("default"), searchPathsOne, false);
		std::vector<std::string> toLoadDlls;
		// If a 'default' folder exists in the default search path, then use it for dlls.
		// If not, then use the base plugins directory.
		// Any library version / name detecting logic is in loadDLL (called by loadDLLs).

		if (rPaths.size() || oPaths.size())
		{
			searchDLLs(toLoadDlls, rPaths, true);
			searchDLLs(toLoadDlls, oPaths, false);
		}
		else { searchDLLs(toLoadDlls); }

		loadDLLs(toLoadDlls);


		if (vm.count("print-dll-loaded")) {
			printDLLs();
			exit(0);
		}
	}

}

extern "C"
{
	dllInitResult icedb_registry_register_dll(const icedb::registry::DLLpreamble &p, void* ptr)
	{
		using namespace icedb;
		icedb::registry::DLLpreamble b = p;
		using namespace icedb::os_functions;
		auto h = getModuleInfo((void*)ptr);
		std::string dllPath(getPath(h.get()));
		h.reset();

		for (const auto & i : preambles)
		{
			if (std::strcmp(i.uuid, p.uuid) == 0) {
				// Duplicate load detected!
				
				// Cannot Throw failure condition.
				//ICEDB_throw(icedb::error::xDuplicateHook())
				//	<< icedb::error::file_name(dllPath);
				return DUPLICATE_DLL;
			}
		}
		preambles.push_back(b);
		return SUCCESS;
	}

}
