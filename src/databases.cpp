#include <set>
#include <map>
#include <memory>
#include "../icedb/fs.hpp"
#include "../icedb/gsl/gsl_assert"
#include "../private/fs_backend.hpp"
#include "../icedb/hdf5_supplemental.hpp"
#include "../icedb/Database.hpp"
#include <atomic>
#include <sstream>
#include <string>
#include "../icedb/compat/hdf5_load.h"


namespace icedb {
	namespace Databases {
		namespace impl {
			class file_image {
				hid_t propertyList;
				std::vector<char> buffer;
				const std::string filename;
				std::shared_ptr<H5::H5File> hFile;
			public:
				std::shared_ptr<H5::H5File> getHFile() const { return hFile; }
				file_image(const std::string &filename,
					size_t desiredSizeInBytes)
					: buffer(desiredSizeInBytes), filename(filename),
					propertyList(-1), hFile(nullptr)
				{
					propertyList = H5Pcreate(H5P_FILE_ACCESS);
					auto h5Result = H5Pset_fapl_core(propertyList, buffer.size(), false);
					Expects(h5Result >= 0 && "H5Pset_fapl_core failed");
					// This new memory-only dataset needs to always be writable. The flags parameter
					// has little meaning in this context.
					hFile = std::make_shared<H5::H5File>(filename.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, propertyList);
					//m_file = H5Fcreate(filename.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, propertyList);
				}
				~file_image() {
					if (propertyList != -1) H5Pclose(propertyList);
				}

			};

		}

		class Database::Database_impl {
			friend class Database;
			std::shared_ptr<H5::H5File> hFile;
			/// Used if a virtual base is needed (the typical case)
			std::shared_ptr<impl::file_image> hFileImage;

			static sfs::path resolveSymlinkPathandForceExists(const std::string &location) {
				sfs::path res;
				sfs::path pBase(location);
				Expects(sfs::exists(pBase));
				res = fs::impl::resolveSymLinks(pBase);
				Expects(sfs::exists(res));
				return res;
			}

			static fs::impl::CollectedFilesRet_Type collectActualHDF5files(const sfs::path &pBaseS) {
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
						mountFiles.insert(cand);
				}
				return mountFiles;
			}

			static std::shared_ptr<H5::H5File> makeDatabaseFileStandard(const std::string &p) {
				auto res = std::make_shared<H5::H5File>(p,
					fs::hdf5::getHDF5IOflags(fs::IOopenFlags::TRUNCATE),
					H5P_DEFAULT);
				constexpr uint64_t dbverno = 1;
				icedb::fs::hdf5::addAttr<uint64_t, H5::H5File>(res, "Version", dbverno);
				icedb::fs::hdf5::addAttr<std::string, H5::H5File>(res, "Software", "libicedb");
				return res;
			}
			static std::string getUniqueVROOTname() {
				static std::atomic<int> i{ 0 };
				int j = i++;
				std::ostringstream out;
				out << "VIRTUAL-" << j;
				return std::string(out.str());
			}
		};
		Database::Database() {
			_impl = std::make_shared<Database_impl>();
		}

		Groups::Group Database::openPath(const std::string &path) {
			return std::move(Groups::Group(path, _impl->hFile));
		}

		Database Database::createDatabase(
			const std::string &location)
		{
			const auto flag_truncate = fs::hdf5::getHDF5IOflags(fs::IOopenFlags::TRUNCATE);
			
			sfs::path pBase(location);
			sfs::create_directories(pBase);
			Expects(sfs::exists(pBase));

			sfs::create_directory(pBase / "3d_Structures");
			sfs::create_directory(pBase / "Physical_Particle_Properties");
			sfs::create_directory(pBase / "Extended_Scattering_Variables");
			sfs::create_directory(pBase / "Essential_Scattering_Variables");

			Database_impl::makeDatabaseFileStandard((pBase / "3d_Structures" / "3dS-1.hdf5").string());
			Database_impl::makeDatabaseFileStandard((pBase / "3d_Structures" / "3dS-2.hdf5").string());
			Database_impl::makeDatabaseFileStandard((pBase / "3d_Structures" / "3dS-3.hdf5").string());
			Database_impl::makeDatabaseFileStandard((pBase / "Physical_Particle_Properties" / "ppp.hdf5").string());
			Database_impl::makeDatabaseFileStandard((pBase / "Essential_Scattering_Variables" / "esv1.hdf5").string());
			Database_impl::makeDatabaseFileStandard((pBase / "Essential_Scattering_Variables" / "esv2.hdf5").string());
			Database_impl::makeDatabaseFileStandard((pBase / "metadata.hdf5").string());

			return openDatabase(location, icedb::fs::IOopenFlags::READ_WRITE);
		}

		void Database::indexDatabase(const std::string &location)
		{
			sfs::path pBaseS = Database_impl::resolveSymlinkPathandForceExists(location);
			fs::impl::CollectedFilesRet_Type mountFiles = Database_impl::collectActualHDF5files(pBaseS);
			Expects(mountFiles.size() > 0);
			if (mountFiles.size() == 1) return;

			Database res;
			res._impl->hFile = Database_impl::makeDatabaseFileStandard(location + "/index.hdf5");

			// Check for the existence of the appropriate groups in the relative path tree
			// Create any missing groups in the relative path tree, and then make the link.
			for (const auto &toMount : mountFiles) {
				std::string mountStr = toMount.second;
				if (mountStr == "index.hdf5") continue;
				auto explodedPath = fs::hdf5::explodeHDF5groupPath(mountStr);
				std::string linkName = *(explodedPath.rbegin());
				explodedPath.pop_back();

				/// \todo createGroupStructure cannot currently handle passing back an H5File
				/// (in the no groups case). It is a bug in HDF5. Once fixed, the if-else cases
				/// should be collapsed into one.
				if (explodedPath.size()) {
					auto grp = fs::hdf5::createGroupStructure(explodedPath, *(res._impl->hFile.get()));
					H5Lcreate_external(toMount.second.c_str(), "/", grp.getLocId(), linkName.c_str(), H5P_DEFAULT, H5P_DEFAULT);
				} else {
					H5Lcreate_external(toMount.second.c_str(), "/", res._impl->hFile->getLocId(), linkName.c_str(), H5P_DEFAULT, H5P_DEFAULT);
				}
			}

		}

		Database Database::openDatabase(
			const std::string &location, fs::IOopenFlags flags)
		{
			sfs::path pBaseS = Database_impl::resolveSymlinkPathandForceExists(location);
			fs::impl::CollectedFilesRet_Type mountFiles = Database_impl::collectActualHDF5files(pBaseS);
			unsigned int Hflags = fs::hdf5::getHDF5IOflags(flags);
			Database res;
			Expects(mountFiles.size() > 0);
			if (mountFiles.size() == 1) {
				res._impl->hFile = std::make_shared<H5::H5File>(
					mountFiles.begin()->first.string().c_str(), Hflags, H5P_DEFAULT);
			} else {
				constexpr size_t virt_mem_size = 10 * 1024 * 1024; // 10 MB
				res._impl->hFileImage = std::make_shared<impl::file_image>(Database_impl::getUniqueVROOTname(), virt_mem_size);
				res._impl->hFile = res._impl->hFileImage->getHFile();

				// Check for the existence of the appropriate groups in the relative path tree
				// Create any missing groups in the relative path tree
				for (const auto &toMount : mountFiles) {
					std::string mountStr = toMount.second;
					if (mountStr == "index.hdf5") continue;
					H5::Group grp = icedb::fs::hdf5::createGroupStructure(mountStr, *(res._impl->hFile.get()));
					// Open files and mount in the relative path tree
					std::shared_ptr<H5::H5File> newHfile = std::make_shared<H5::H5File>(toMount.first.string(), Hflags);
					//res._impl->mappedFiles[toMount.second] = newHfile;
					res._impl->hFile->mount(toMount.second, *(newHfile.get()), H5P_DEFAULT);
				}
			}

			return std::move(res);
		}


	}
}