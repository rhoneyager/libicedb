#include "fs_backend.hpp"
#include "gsl/gsl_assert"

#include <string>
#include <sstream>
#include <utility>
#include <H5Cpp.h>

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

			std::set<std::string> getGroupMembers(const H5::Group &base) {
				std::set<std::string> res;
				hsize_t numObjs = base.getNumObjs();
				for (hsize_t i = 0; i < numObjs; ++i)
				{
					std::string name = base.getObjnameByIdx(i);
					res.insert(name);
				}
				return res;
			}

			std::vector<std::string> explode(std::string const & s, char delim)
			{
				std::vector<std::string> result;
				std::istringstream iss(s);

				for (std::string token; std::getline(iss, token, delim); )
				{
					result.push_back(std::move(token));
				}

				return result;
			}

			H5::Group createGroupStructure(const std::string &groupName, H5::Group &base) {
				// Using the '/' specifier, pop off the group names and 
				// create groups if necessary in the tree. Return the final group.
				Expects(groupName.size() > 0);
				std::string mountStr = groupName;
				std::replace(mountStr.begin(), mountStr.end(), '\\', '/');
				std::vector<std::string> groups = explode(mountStr, '/');
				

				/// \note This is really awkwardly stated because of an MSVC2017 bug.
				/// The HDF5 group copy constructor fails to update its id field, even though
				/// the hid_t is a uint64 and the copy is trivial. These 'relocatable references'
				/// allow me to avoid these copies through liberal use of std::move.
				std::shared_ptr<H5::Group> current(&base, [](H5::Group*) {});
				std::vector<H5::Group> vgrps;
				//vgrps.push_back(current);

				//H5::Group current(base);
				for (const auto &grpname : groups) {
					if (!grpname.size()) continue; // Skip any empty entries
					std::set<std::string> members = getGroupMembers(*(current.get()));
					if (!members.count(grpname)) {
						vgrps.push_back(std::move(current->createGroup(grpname)));
						current = std::shared_ptr<H5::Group>(&(*(vgrps.rbegin())), [](H5::Group*) {});
					} else {
						//H5::Group newcur = current->openGroup(grpname);
						//current = newcur;
						vgrps.push_back(std::move(current->openGroup(grpname)));
						current = std::shared_ptr<H5::Group>(&(*(vgrps.rbegin())), [](H5::Group*) {});
					}
				}
				return std::move((*(vgrps.rbegin())));
			}
		}
	}
}
