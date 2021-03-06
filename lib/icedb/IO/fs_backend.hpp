#pragma once
#include "../defs.h"
#include <boost/version.hpp>
# define have_boost_filesystem 1
# include <boost/filesystem.hpp>

/*
#ifndef __has_include
  //static_assert(0, "This library requires a recent compiler that supports __has_include");
#define __has_include(x) 0
#endif
#if (__has_include(<boost/filesystem.hpp>) || defined(ICEDB_HAS_COMPILED_BOOST_FILESYSTEM)) && (!ICEDB_PROHIBIT_BOOST_FILESYSTEM) && (BOOST_VERSION > 104400)
# define have_boost_filesystem 1
# include <boost/filesystem.hpp>
  namespace sfs = boost::filesystem;
#elif __has_include(<filesystem>)
  // filesystem is still problematic, so I am ignoring it for now. Build system almost always overrides with boost::filesystem.
# include <filesystem>
# define have_std_filesystem 1
# define experimental_filesystem
  namespace sfs = std::experimental::filesystem::v1;
 // namespace sfs = std::filesystem;
#elif __has_include(<experimental/filesystem>)
# include <experimental/filesystem>
# define have_std_filesystem 1
# define experimental_filesystem
  namespace sfs = std::experimental::filesystem::v1;
#else
# error("This library either requires boost::filesystem v3 or a recent compiler that supports the C++ 2017 filesystem library.");
#endif
*/
//__cpp_lib_experimental_filesystem - is not yet defined on all compilers, notable MSVC17.


#include <set>
#include <string>
#include <map>

namespace icedb {
	namespace fs {

		namespace sfs = boost::filesystem;

		/// Internal filesystem functions. These will be made user-inaccessible in a future release.
		/// \deprecated These will be moved to a private header in a future release.
		namespace impl {
			typedef std::set<sfs::path> ExtensionsMatching_Type;
			extern const ExtensionsMatching_Type common_hdf5_extensions;
			/// Finds out where a symbolic link points to
			ICEDB_DL sfs::path resolveSymLinks(const sfs::path &base);

			/// File path, relative mount point
			typedef std::vector<std::pair<sfs::path, std::string> > CollectedFilesRet_Type;
			/// \brief Find all files under the base location that have matching extensions.
			/// Used to collect files for reading.
			/// \param base is the base path
			/// \param fileExtensionsToMatch is a list of file extensions that are matched
			/// \param MatchOnAnySingleFile is a flag that forces a match if a single file is specified.
			ICEDB_DL CollectedFilesRet_Type collectDatasetFiles(
				const sfs::path &base,
				const ExtensionsMatching_Type &fileExtensionsToMatch = common_hdf5_extensions,
				bool MatchOnAnySingleFile = true);

			/// Like resolveSymLinks, but throw if the resulting path does not exist.
			ICEDB_DL sfs::path resolveSymlinkPathandForceExists(const std::string &location);

			/// Like collectDatasetFiles for HDF5 files, but then check that these files are, indeed, HDF5 files.
			ICEDB_DL CollectedFilesRet_Type collectActualHDF5files(const sfs::path &pBaseS);

			/// Generate a unique string, used in memort-only HDF5 file trees.
			ICEDB_DL std::string getUniqueVROOTname();
		}
	}
}
