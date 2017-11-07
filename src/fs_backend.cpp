#include "../private/fs_backend.hpp"
#include "../icedb/gsl/gsl_assert"


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
				std::map<sfs::path, std::string> res;

				for (const auto& p : sfs::recursive_directory_iterator(base)) {
					auto sp = resolveSymLinks(p.path());
					if (!sfs::exists(sp)) continue;
					if (sfs::is_regular_file(sp)) {
						if (valid_extensions.count(sp.extension()) > 0) {
							/// \todo Once Filesystem adds lexically_proximate to all 
							/// compilers, change the path decomposition to the standard method.
							std::string sP = p.path().string();
							std::replace(sP.begin(), sP.end(), '\\', '/');
							std::string sBase = base.string();
							sBase = sBase + "";
							Expects(sP.substr(0, sBase.size()) == sBase);
							Expects(sP.size() >= sBase.size() + 1);
							std::string relpath = sP.substr(sBase.size()+1);
							

							res[p] = relpath;
						}
					}
				}
				return res;
			}

			
		}
	}
}
