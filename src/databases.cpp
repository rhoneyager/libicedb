#include <set>
#include <map>
#include <memory>
#include <stdexcept>
#include "../icedb/fs.hpp"
#include <gsl/gsl_assert>
#include "../icedb/fs_backend.hpp"
#include "../private/hdf5_supplemental.hpp"
#include "../icedb/Database.hpp"
#include "../private/Database_impl.hpp"
#include "../icedb/compat/hdf5_load.h"


namespace icedb {
	namespace Databases {
		std::shared_ptr<H5::H5File> file_image::getHFile() const { return hFile; }
		file_image::file_image(const std::string &filename,
			size_t desiredSizeInBytes)
					: buffer(desiredSizeInBytes), filename(filename),
					propertyList(-1), hFile(nullptr)
				{
					propertyList = H5Pcreate(H5P_FILE_ACCESS);
					const auto h5Result = H5Pset_fapl_core(propertyList, buffer.size(), false);
					Expects(h5Result >= 0 && "H5Pset_fapl_core failed");
					// This new memory-only dataset needs to always be writable. The flags parameter
					// has little meaning in this context.
					hFile = std::make_shared<H5::H5File>(filename.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, propertyList);
					//m_file = H5Fcreate(filename.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, propertyList);
				}
		file_image::~file_image() {
					if (propertyList != -1) H5Pclose(propertyList);
				}

		std::shared_ptr<H5::H5File> Database_impl::makeDatabaseFileStandard(const std::string &p) {
				auto res = std::make_shared<H5::H5File>(p,
					fs::hdf5::getHDF5IOflags(fs::IOopenFlags::TRUNCATE),
					H5P_DEFAULT);
				constexpr uint64_t dbverno = 1;
				icedb::fs::hdf5::addAttr<uint64_t, H5::H5File>(res.get(), "Version", dbverno);
				icedb::fs::hdf5::addAttr<std::string, H5::H5File>(res.get(), "Software", "libicedb");
				return res;
			}
		
		Groups::Group::Group_ptr Database_impl::createGroup(const std::string &path) {
			return Groups::Group::createGroup(path, hFile.get());
		}

		Groups::Group::Group_ptr Database_impl::openGroup(const std::string &path) {
			return Groups::Group::openGroup(path, hFile.get());
		}

		Database_impl::Database_impl() : hFileImage(fs::impl::getUniqueVROOTname(), 10 * 1024 * 1024)
		{ }

		Database_impl::Database_impl(size_t virtualMemSizeInBytes) : hFileImage(fs::impl::getUniqueVROOTname(), virtualMemSizeInBytes)
		{ }

		Database_impl::~Database_impl() {}

		Database::~Database() {}

		Database::Database() {}

		Database::Database_ptr Database::createSampleDatabase(
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
			sfs::create_directory(pBase / "Testing");

			Database_impl::makeDatabaseFileStandard((pBase / "3d_Structures" / "3dS-1.hdf5").string());
			Database_impl::makeDatabaseFileStandard((pBase / "3d_Structures" / "3dS-2.hdf5").string());
			Database_impl::makeDatabaseFileStandard((pBase / "3d_Structures" / "3dS-3.hdf5").string());
			Database_impl::makeDatabaseFileStandard((pBase / "Physical_Particle_Properties" / "ppp.hdf5").string());
			Database_impl::makeDatabaseFileStandard((pBase / "Essential_Scattering_Variables" / "esv1.hdf5").string());
			Database_impl::makeDatabaseFileStandard((pBase / "Essential_Scattering_Variables" / "esv2.hdf5").string());
			Database_impl::makeDatabaseFileStandard((pBase / "Testing" / "scratch.hdf5").string());
			Database_impl::makeDatabaseFileStandard((pBase / "metadata.hdf5").string());

			return std::move(openDatabase(location, icedb::fs::IOopenFlags::READ_WRITE));
		}

		Database::Database_ptr Database::openVirtualDatabase(size_t memSizeInBytes)
		{
			auto res = std::make_unique<Database_impl>();
			//std::unique_ptr<Database_impl, mem::icedb_delete<Database_impl> > res(new Database_impl);
			//= std::make_unique(<Database_impl, mem::icedb_delete<Database_impl> >();
			res->hFile = res->hFileImage.getHFile();

			return res;
		}


		Database::Database_ptr Database::openDatabase(
			const std::string &location, fs::IOopenFlags flags)
		{
			sfs::path pBaseS = fs::impl::resolveSymLinks(location);
			if (!sfs::exists(pBaseS)) {
				throw(std::invalid_argument("Attempting to open a database root that does not exist. Did you mean to first create this database?"));
			}
			fs::impl::CollectedFilesRet_Type mountFiles = fs::impl::collectActualHDF5files(pBaseS);
			unsigned int Hflags = fs::hdf5::getHDF5IOflags(flags);
			auto res = std::make_unique<Database_impl>();
			//std::unique_ptr<Database_impl, mem::icedb_delete<Database_impl> > res(new Database_impl);
				//= std::make_unique(<Database_impl, mem::icedb_delete<Database_impl> >();

			Expects(mountFiles.size() > 0);
			if (mountFiles.size() == 1) {
				res->hFile = std::make_shared<H5::H5File>(
					mountFiles.begin()->first.string().c_str(), Hflags, H5P_DEFAULT);
			}
			else {
				res->hFile = res->hFileImage.getHFile();

				// Check for the existence of the appropriate groups in the relative path tree
				// Create any missing groups in the relative path tree
				for (const auto &toMount : mountFiles) {
					std::string mountStr = toMount.second;
					if (mountStr == "index.hdf5") continue;
					H5::Group grp = icedb::fs::hdf5::createGroupStructure(mountStr, *(res->hFile.get()));
					// Open files and mount in the relative path tree
					std::shared_ptr<H5::H5File> newHfile = std::make_shared<H5::H5File>(toMount.first.string(), Hflags);
					//res._impl->mappedFiles[toMount.second] = newHfile;
					res->hFile->mount(toMount.second, *(newHfile.get()), H5P_DEFAULT);
				}
			}

			return res;
		}

		void Database::indexDatabase(const std::string &location)
		{
			sfs::path pBaseS = fs::impl::resolveSymlinkPathandForceExists(location);
			fs::impl::CollectedFilesRet_Type mountFiles = fs::impl::collectActualHDF5files(pBaseS);
			Expects(mountFiles.size() > 0);
			if (mountFiles.size() == 1) return;

			std::unique_ptr<Database_impl> res = std::make_unique<Database_impl>();
			res->hFile = Database_impl::makeDatabaseFileStandard(location + "/index.hdf5");

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
					auto grp = fs::hdf5::createGroupStructure(explodedPath, *(res->hFile.get()));
					H5Lcreate_external(toMount.second.c_str(), "/", grp.getLocId(), linkName.c_str(), H5P_DEFAULT, H5P_DEFAULT);
				} else {
					H5Lcreate_external(toMount.second.c_str(), "/", res->hFile->getLocId(), linkName.c_str(), H5P_DEFAULT, H5P_DEFAULT);
				}
			}

		}

		

	}
}
