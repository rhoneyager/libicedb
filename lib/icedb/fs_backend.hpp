#pragma once
#ifdef __has_include
#  if __has_include(<filesystem>)
#    include <filesystem>
#    define have_filesystem 1
#  endif
#  if __has_include(<experimental/filesystem>)
#    include <experimental/filesystem>
#    define have_filesystem 1
#    define experimental_filesystem
//using namespace std::experimental::filesystem::v1;
namespace sfs = std::experimental::filesystem::v1;
#  endif
#endif
#ifndef have_filesystem
#    define have_filesystem 0
#endif
static_assert(have_filesystem == 1,
	"This library requires a recent compiler that supports the C++ 2017 Filesystem library");
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