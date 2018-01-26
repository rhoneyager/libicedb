#pragma once
#ifndef __has_include
  //static_assert(0, "This library requires a recent compiler that supports __has_include");
#define __has_include(x) 0
#endif
#if __has_include(<boost/filesystem.hpp>) || defined(ICEDB_HAS_COMPILED_BOOST_FILESYSTEM)
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
  static_assert(0, "This library either requires boost::filesystem or a recent compiler that supports the C++ 2017 filesystem library.");
#endif

//__cpp_lib_experimental_filesystem - is not yet defined on all compilers, notable MSVC17.


#include <set>
#include <string>
#include <map>

namespace icedb {
	namespace fs {
		namespace impl {
			typedef std::set<sfs::path> ExtensionsMatching_Type;
			extern const ExtensionsMatching_Type common_hdf5_extensions;
			sfs::path resolveSymLinks(const sfs::path &base);

			/// File path, relative mount point
			typedef std::vector<std::pair<sfs::path, std::string> > CollectedFilesRet_Type;
			CollectedFilesRet_Type collectDatasetFiles(
				const sfs::path &base,
				const ExtensionsMatching_Type &fileExtensionsToMatch = common_hdf5_extensions);

			sfs::path resolveSymlinkPathandForceExists(const std::string &location);

			CollectedFilesRet_Type collectActualHDF5files(const sfs::path &pBaseS);

			std::string getUniqueVROOTname();
		}
	}
}
