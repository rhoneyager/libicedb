#include "cdffs.h"
#include "gsl/gsl_assert"
#include <set>
#include <map>
#include <memory>
#include <H5Cpp.h>

//#include <H5>

#include "fs_backend.hpp"
#include "hdf5_supplemental.hpp"
#include "export-hdf5.hpp"

namespace icedb {
	namespace fs {
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
		};
		Database::Database() {
			_impl = std::make_shared<Database_impl>();
		}

		void Database::indexDatabase(const std::string &location)
		{
			Database res;

			sfs::path pBase(location);
			Expects(sfs::exists(pBase));
			sfs::path pBaseS = impl::resolveSymLinks(pBase);
			Expects(sfs::exists(pBaseS));

			// Find any hdf5 files underneath the current base location (recursive)
			// and create a key/value map showing where to mount these files.
			impl::CollectedFilesRet_Type mountFilesCands = impl::collectDatasetFiles(pBaseS);
			impl::CollectedFilesRet_Type mountFiles;
			// Ensure that the candidate files are actually HDF5 files
			for (const auto & cand : mountFilesCands) {
				// Returns >0 if a valid HDF5 file. = 0 if not. -1 on error (nonexistent file).
				htri_t isval = H5Fis_hdf5(cand.first.string().c_str());
				Expects(isval >= 0 && "File should exist at this point." && cand.first.string().c_str());
				if (isval > 0)
					mountFiles.insert(cand);
			}

			// Check the number of detected hdf5 / netCDF files. If only one, then this is
			// the root file and is the entire dataset. If more than one, then open
			// a virtual file hierarchy, and mount each hdf5 file into the corresponding
			// mount point.

			unsigned int Hflags = H5F_ACC_RDONLY; // HDF5 flags

			Expects(mountFiles.size() > 0);
			if (mountFiles.size() == 1) {
				res._impl->hFile = std::make_shared<H5::H5File>(
					mountFiles.begin()->first.string().c_str(), Hflags, H5P_DEFAULT);
			}
			else {
				constexpr size_t virt_mem_size = 10 * 1024 * 1024; // 10 MB
				//res._impl->hFileImage = std::make_shared<impl::file_image>("VIRTUAL-ROOT", virt_mem_size);
				res._impl->hFile = std::make_shared<H5::H5File>(
					(location + "/index.hdf5"), H5F_ACC_TRUNC, H5P_DEFAULT);
				scatdb::plugins::hdf5::addAttr<uint64_t, H5::H5File>(res._impl->hFile, "Version", 1);

				//res._impl->hFile = res._impl->hFileImage->getHFile();
				for (const auto &toMount : mountFiles) {
					// Check for the existence of the appropriate groups in the relative path tree
					// Create any missing groups in the relative path tree
					std::string mountStr = toMount.second;
					std::replace(mountStr.begin(), mountStr.end(), '\\', '/');
					if (mountStr == "index.hdf5") continue;
					// Suppress the final group - this will be the link name
					std::string linkName = mountStr.substr(mountStr.find_last_of('/') + 1);
					if (mountStr.find('/') != std::string::npos) {
						mountStr = mountStr.substr(0, mountStr.find_last_of('/'));
						H5::Group grp = impl::createGroupStructure(mountStr, *(res._impl->hFile.get()));
						H5Lcreate_external(toMount.second.c_str(), "/", grp.getLocId(), linkName.c_str(), H5P_DEFAULT, H5P_DEFAULT);
					}
					else {
						H5Lcreate_external(toMount.second.c_str(), "/", res._impl->hFile->getLocId(), linkName.c_str(), H5P_DEFAULT, H5P_DEFAULT);
					}

				}
			}

		}

		Database Database::openDatabase(
			const std::string &location, IOopenFlags flags)
		{
			Database res;
			
			sfs::path pBase(location);
			Expects(sfs::exists(pBase));
			sfs::path pBaseS = impl::resolveSymLinks(pBase);
			Expects(sfs::exists(pBaseS));

			// Find any hdf5 files underneath the current base location (recursive)
			// and create a key/value map showing where to mount these files.
			impl::CollectedFilesRet_Type mountFilesCands = impl::collectDatasetFiles(pBaseS);
			impl::CollectedFilesRet_Type mountFiles;
			// Ensure that the candidate files are actually HDF5 files
			for (const auto & cand : mountFilesCands) {
				// Returns >0 if a valid HDF5 file. = 0 if not. -1 on error (nonexistent file).
				htri_t isval = H5Fis_hdf5(cand.first.string().c_str());
				Expects(isval >= 0 && "File should exist at this point." && cand.first.string().c_str());
				if (isval > 0)
					mountFiles.insert(cand);
			}

			// Check the number of detected hdf5 / netCDF files. If only one, then this is
			// the root file and is the entire dataset. If more than one, then open
			// a virtual file hierarchy, and mount each hdf5 file into the corresponding
			// mount point.

			unsigned int Hflags = 0; // HDF5 flags
			if (flags == IOopenFlags::READ_ONLY) Hflags = H5F_ACC_RDONLY;
			else if (flags == IOopenFlags::READ_WRITE) Hflags = H5F_ACC_RDWR;
			else if (flags == IOopenFlags::TRUNCATE) Hflags = H5F_ACC_TRUNC;
			else if (flags == IOopenFlags::CREATE) Hflags = H5F_ACC_CREAT;

			Expects(mountFiles.size() > 0);
			if (mountFiles.size() == 1) {
				res._impl->hFile = std::make_shared<H5::H5File>(
					mountFiles.begin()->first.string().c_str(), Hflags, H5P_DEFAULT);
			}
			else {
				constexpr size_t virt_mem_size = 10 * 1024 * 1024; // 10 MB
				res._impl->hFileImage = std::make_shared<impl::file_image>("VIRTUAL-ROOT",virt_mem_size);
				res._impl->hFile = res._impl->hFileImage->getHFile();
				for (const auto &toMount : mountFiles) {
					// Check for the existence of the appropriate groups in the relative path tree
					// Create any missing groups in the relative path tree
					std::string mountStr = toMount.second;
					std::replace(mountStr.begin(), mountStr.end(), '\\', '/');
					if (mountStr == "index.hdf5") continue;
					H5::Group grp = impl::createGroupStructure(mountStr, *(res._impl->hFile.get()));
					// Open files and mount in the relative path tree
					std::shared_ptr<H5::H5File> newHfile = std::make_shared<H5::H5File>(toMount.first.string(), Hflags);
					//res._impl->mappedFiles[toMount.second] = newHfile;
					res._impl->hFile->mount(toMount.second, *(newHfile.get()), H5P_DEFAULT);
				}
			}


			return std::move(res);
		}

		Database Database::createDatabase(
			const std::string &location)
		{
			sfs::path pBase(location);
			sfs::create_directories(pBase);
			Expects(sfs::exists(pBase));

			sfs::create_directory(pBase / "3d_Structures");
			sfs::create_directory(pBase / "Physical_Particle_Properties");
			sfs::create_directory(pBase / "Extended_Scattering_Variables");
			sfs::create_directory(pBase / "Essential_Scattering_Variables");

			std::shared_ptr<H5::H5File> f3D1(new H5::H5File((pBase / "3d_Structures" / "3dS-1.hdf5").string(), H5F_ACC_TRUNC));
			std::shared_ptr<H5::H5File> f3D2(new H5::H5File((pBase / "3d_Structures" / "3dS-2.hdf5").string(), H5F_ACC_TRUNC));
			std::shared_ptr<H5::H5File> f3D3(new H5::H5File((pBase / "3d_Structures" / "3dS-3.hdf5").string(), H5F_ACC_TRUNC));
			H5::H5File fPPP((pBase / "Physical_Particle_Properties" / "ppp.hdf5").string(), H5F_ACC_TRUNC);
			H5::H5File fESV1((pBase / "Extended_Scattering_Variables" / "esv1.hdf5").string(), H5F_ACC_TRUNC);
			H5::H5File fESV2((pBase / "Essential_Scattering_Variables" / "esv2.hdf5").string(), H5F_ACC_TRUNC);
			//H5::H5File fMeta((pBase / "metadata.hdf5").string(), H5F_ACC_TRUNC);
			std::shared_ptr<H5::H5File> fMeta(new H5::H5File((pBase / "metadata.hdf5").string(), H5F_ACC_TRUNC));
			
			scatdb::plugins::hdf5::addAttr<uint64_t, H5::H5File>(f3D1, "Version", 1);
			scatdb::plugins::hdf5::addAttr<uint64_t, H5::H5File>(f3D2, "Version", 1);
			scatdb::plugins::hdf5::addAttr<uint64_t, H5::H5File>(f3D3, "Version", 1);
			scatdb::plugins::hdf5::addAttr<uint64_t, H5::H5File>(fMeta, "Version", 1);
			//fMeta.createAttribute("version", H)

			return openDatabase(location, icedb::fs::IOopenFlags::READ_WRITE);
		}

		/*
		Database Database::copyDatabase(
			const Database &source, const std::string &location)
		{
			//source._impl->hFile->cl
		}
		*/
	}
}