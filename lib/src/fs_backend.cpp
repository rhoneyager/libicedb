#include "../icedb/fs_backend.hpp"
#include <gsl/gsl_assert>
#include "../icedb/compat/hdf5_load.h"
#include <algorithm>
#include <atomic>
#include <sstream>
#include <string>
#include <iostream>
namespace icedb {
	namespace fs {
		namespace impl {
			const ExtensionsMatching_Type common_hdf5_extensions
				= { ".hdf5", ".nc", ".h5", ".cdf", ".hdf" };


			sfs::path resolveSymLinks(const sfs::path &base)
			{
				sfs::path res = base;
				while (sfs::is_symlink(res) && sfs::exists(res))
				{
					res = sfs::read_symlink(res);
				}
				return res;
			}

			CollectedFilesRet_Type collectDatasetFiles(
				const sfs::path &base,
				const ExtensionsMatching_Type &valid_extensions)
			{
				std::string sBase = base.string();
				std::replace(sBase.begin(), sBase.end(), '\\', '/');

				std::vector<std::pair<sfs::path, std::string> > res;
				auto sbase = resolveSymlinkPathandForceExists(base.string());
				if (sfs::is_regular_file(sbase)) {
					if (valid_extensions.count(base.extension()) > 0) {
						res.push_back(std::pair<sfs::path, std::string>(sbase, "/"));
					}
				}
				else {
					for (const auto& p : sfs::recursive_directory_iterator(base)) {
						auto sp = resolveSymLinks(p.path());
						if (!sfs::exists(sp)) continue;
						if (sfs::is_regular_file(sp)) {
							if (valid_extensions.count(sp.extension()) > 0) {
								std::string sP = p.path().string();
								std::replace(sP.begin(), sP.end(), '\\', '/');
								Expects(sP.find(sBase) == 0);
								std::string relPath = sP.substr(sBase.length());
								// Remove any '/'s from the beginning of the string
								relPath = relPath.substr(relPath.find_first_not_of('/'));

								/// \todo Once std::filesystem adds the lexically_proximate method to all 
								/// compilers, change the path decomposition to the standard method.
								/*
								std::string sP = p.path().string();
								std::replace(sP.begin(), sP.end(), '\\', '/');
								std::string sBase = base.string();
								//Expects(sP.substr(0, sBase.size()) == sBase);
								Expects(sP.size() >= sBase.size() + 1);
								std::string relpath = sP.substr(sBase.size() + 1);
								*/

								res.push_back(std::pair<sfs::path, std::string>(p, relPath));
							}
						}
					}
				}
				return res;
			}


			sfs::path resolveSymlinkPathandForceExists(const std::string &location) {
				sfs::path res;
				sfs::path pBase(location);
				Expects(sfs::exists(pBase));
				res = fs::impl::resolveSymLinks(pBase);
				Expects(sfs::exists(res));
				return res;
			}

			CollectedFilesRet_Type collectActualHDF5files(const sfs::path &pBaseS) {
				// Find any hdf5 files underneath the current base location (recursive)
				// and create a key/value map showing where to mount these files.
				fs::impl::CollectedFilesRet_Type mountFilesCands = fs::impl::collectDatasetFiles(pBaseS);
				fs::impl::CollectedFilesRet_Type mountFiles;
				// Ensure that the candidate files are actually HDF5 files
				for (const auto & cand : mountFilesCands) {
					// Returns >0 if a valid HDF5 file. = 0 if not. -1 on error (nonexistent file).
					htri_t isval = H5Fis_hdf5(cand.first.string().c_str());
					Expects(isval >= 0 && "File should exist at this point." && cand.first.string().c_str());
					if (isval > 0)
						mountFiles.push_back(cand);
					if (isval == 0)
						std::cerr << "File " << cand.first.string() << " is somehow invalid." << std::endl;
				}
				return mountFiles;
			}

			std::string getUniqueVROOTname() {
				static std::atomic<int> i{ 0 };
				int j = i++;
				std::ostringstream out;
				out << "VIRTUAL-" << j;
				return std::string(out.str());
			}
			
		}
	}
}
