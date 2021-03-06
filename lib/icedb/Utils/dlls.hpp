#pragma once
#include "../defs.h"

#if defined(_MSC_FULL_VER)
#pragma warning(push)
#pragma warning( disable : 4251 ) // DLL interface
#pragma warning( disable : 4661 ) // Exporting vector
#endif

//#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <vector>
#include <iostream>
//#include <boost/program_options.hpp>

namespace boost {
	namespace filesystem {
		class path;
	}
	namespace program_options {
		class options_description;
		class variables_map;
	}
}

namespace icedb
{
	namespace registry
	{
		/** \brief This is the basic structure passed by an icedb plugin to provide
		* identity information.
		**/
		struct DLLpreamble
		{
			/// Short DLL name
			const char* name;
			/// Short description
			const char* description;
			/// UUID that prevents different versions or copies   
			/// of the same DLL from being loaded simultaneously.
			const char* uuid;
			/// Path of the loaded module (DLL leaves it blank)
			//const char* path;
			DLLpreamble(const char* name, const char* desc, const char* uuid)
				: name(name), description(desc), uuid(uuid) {} //, path(0) {}
			DLLpreamble() : name(0), description(0), uuid(0) {} //, path(0) {}
		};

		class DLLhandleImpl;
		class DLLhandle;

		/// Validator for DLLs
		class ICEDB_DL dllValidator {
		protected:
			dllValidator();
		public:
			virtual ~dllValidator();
			virtual const char* validationSymbol() const = 0;
			virtual bool validate(void* func, bool critical = false) const = 0;
			static std::shared_ptr<const dllValidator> genDefaultValidator();
		};

		/**
		* custom validator design:
		* validator searches for a given function name only, and then passes the
		* code to the custom validator, which returns a bool.
		* If false, it fails the validation check (validator handles any throws and
		* logging). If true, then it passes and moves on to the next validator or
		* completes its load. Only after all validators pass is the dllStart
		* routine called (the one that is passed by icedb).
		**/
		class ICEDB_DL dllValidatorSet {
		private:
			std::vector<std::shared_ptr<const dllValidator> > validators;
		protected:
			dllValidatorSet();
		public:
			void append(std::shared_ptr<const dllValidator>);
			virtual ~dllValidatorSet();
			static std::shared_ptr<dllValidatorSet> generate();
			static std::shared_ptr<const dllValidatorSet> getDefault();
			bool validate(const DLLhandle*, bool critical = false) const;
		};

		/** \brief Class that loads the DLL in an os-independent manner.
		*
		* Construction is allocation.
		* This class enables the safe closiong of dlHandle if registration
		* fails to initialize properly. Previously, a dangling handle was
		* produced.
		**/
		class ICEDB_DL DLLhandle
		{
		public:
			DLLhandle(const std::string &filename,
				std::shared_ptr<const registry::dllValidatorSet>
				= registry::dllValidatorSet::getDefault(),
				bool critical = false);
			DLLhandle();
			void open(const std::string &filename, bool critical = false);
			void close();
			~DLLhandle();
			void* getSym(const char* symbol, bool critical = false) const;
			const char* filename() const;
			bool isOpen() const;
		private:
			std::shared_ptr<DLLhandleImpl> _p;
		};


		/// Load a DLL.
		void ICEDB_DL loadDLL(const std::string &filename, std::shared_ptr<const dllValidatorSet> = dllValidatorSet::getDefault(), bool critical = false);
		/// Load DLLs.
		void ICEDB_DL loadDLLs(const std::vector<std::string> &dlls, std::shared_ptr<const dllValidatorSet> = dllValidatorSet::getDefault(), bool critical = false);
		/// Load all icedb DLLs.
		void ICEDB_DL loadDLLs();

		/// Print loaded DLLs
		void ICEDB_DL printDLLs(std::ostream &out = std::cerr);

		/// List DLL search paths
		void ICEDB_DL printDLLsearchPaths(std::ostream &out = std::cerr);

		/// Find all occurances of a subpath in a search path.
		bool ICEDB_DL findPath(std::set<boost::filesystem::path> &matches,
			const boost::filesystem::path &expr,
			const std::set<boost::filesystem::path> &searchPaths, bool recurse);

		/**
		* \brief Locates all DLLs in the search path and loads them.
		*
		* The search path may be specified / manipulated from several locations:
		* - precompiled hints (from cmake)
		* - icedb.conf
		* - the command line
		*
		* This takes all of the starting points in the initial search paths and recurses through the
		* directories, selecting dll and so files for matching. The load routine is aware of the
		* library build mode (Debug, Release, MinSizeRel, RelWithDebInfo). If one of these terms appears
		* in the path (folder tree + filename) of a library, then the dll is only loaded if its
		* build mode matches the library's mode. When the dll is actually loaded (other function),
		* the build mode is reported by the DLL more directly and checked again.
		*
		* \see searchPaths
		* \see loadSearchPaths
		* \see icedb::registry::process_static_options
		**/
		void ICEDB_DL searchDLLs(std::vector<std::string> &dlls);
		void ICEDB_DL searchDLLs(std::vector<std::string> &dlls,
			const std::set<boost::filesystem::path> &searchPaths, bool recurse);

		void ICEDB_DL add_hook_table(const char* tempsig, void* store);
		void ICEDB_DL dump_hook_table(std::ostream &out = std::cerr);

		void ICEDB_DL list_loaded_modules(std::ostream &out = std::cerr);
	}

	
}


extern "C"
{
	enum dllInitResult {
		SUCCESS,
		DUPLICATE_DLL,
		OTHER_FAILURE
	};

	/// Provides interface for DLLs to register basic information about themselves
	ICEDB_DL dllInitResult icedb_registry_register_dll(
		const icedb::registry::DLLpreamble&,
		void* funcInDllForPath);

	/// Sort of a 'placeholder' symbol used to find the lib dir path.
	ICEDB_SHARED_EXPORT void _ICEDB_dllPluginBase() noexcept;
}

#if defined(_MSC_FULL_VER)
#pragma warning(pop)
#endif

