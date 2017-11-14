#include "../private/hdf5_supplemental.hpp"
#include <gsl/gsl_assert>
#include "../icedb/defs.h"
#include <string>
#include <sstream>
#include <utility>
#include <algorithm>

namespace icedb {
	namespace fs
	{
		namespace hdf5
		{
			bool useZLIB() { return true; }
			//template <class DataType>
			//MatchAttributeTypeType MatchAttributeType() {
			//	static_assert(false, "Unsupported type during attribute conversion in rtmath::plugins::hdf5::MatchAttributeType."); }
			template<> MatchAttributeTypeType MatchAttributeType<std::string>() { return std::make_unique<H5::StrType>(0, H5T_VARIABLE); }
			template<> MatchAttributeTypeType MatchAttributeType<const char*>() { return std::make_unique<H5::StrType>(0, H5T_VARIABLE); }
			template<> MatchAttributeTypeType MatchAttributeType<char>() { return std::make_unique<H5::StrType>(0, 1); }

			template<> MatchAttributeTypeType MatchAttributeType<uint8_t>() { return std::make_unique<H5::IntType>(H5::PredType::NATIVE_UINT8); }
			template<> MatchAttributeTypeType MatchAttributeType<uint16_t>() { return std::make_unique<H5::IntType>(H5::PredType::NATIVE_UINT16); }
			template<> MatchAttributeTypeType MatchAttributeType<uint32_t>() { return std::make_unique<H5::IntType>(H5::PredType::NATIVE_UINT32); }
			template<> MatchAttributeTypeType MatchAttributeType<uint64_t>() { return std::make_unique<H5::IntType>(H5::PredType::NATIVE_UINT64); }
			template<> MatchAttributeTypeType MatchAttributeType<int8_t>() { return std::make_unique<H5::IntType>(H5::PredType::NATIVE_INT8); }
			template<> MatchAttributeTypeType MatchAttributeType<int16_t>() { return std::make_unique<H5::IntType>(H5::PredType::NATIVE_INT16); }
			template<> MatchAttributeTypeType MatchAttributeType<int32_t>() { return std::make_unique<H5::IntType>(H5::PredType::NATIVE_INT32); }
			template<> MatchAttributeTypeType MatchAttributeType<int64_t>() { return std::make_unique<H5::IntType>(H5::PredType::NATIVE_INT64); }
			//template<> MatchAttributeTypeType MatchAttributeType<char>() { return std::make_unique<H5::IntType>(H5::PredType::NATIVE_CHAR)); }

			template<> MatchAttributeTypeType MatchAttributeType<float>() { return std::make_unique<H5::IntType>(H5::PredType::NATIVE_FLOAT); }
			template<> MatchAttributeTypeType MatchAttributeType<double>() { return std::make_unique<H5::IntType>(H5::PredType::NATIVE_DOUBLE); }
			// \note bools are not recommended in HDF5. This type may be switched later on.
			//template<> MatchAttributeTypeType MatchAttributeType<bool>() { return std::shared_ptr<H5::AtomType>(new H5::IntType(H5::PredType::NATIVE_HBOOL)); }

			template<> bool isStrType<std::string>() { return true; }
			template<> bool isStrType<const char*>() { return true; }

			template <> void insertAttr<std::string>(const H5::Attribute &attr, gsl::not_null<H5::AtomType*> vls_type, const std::string& value)
			{
				attr.write(*vls_type, value);
			}
			template <> void loadAttr<std::string>(const H5::Attribute &attr, gsl::not_null<H5::AtomType*> vls_type, std::string& value)
			{
				attr.read(*vls_type, value);
				//attr.write(*vls_type, value);
			}
			/*
			template <> void insertAttr<char const*>(H5::Attribute &, std::shared_ptr<H5::AtomType>, char const * const &);
			template <> void insertAttr<int>(H5::Attribute &, std::shared_ptr<H5::AtomType>, const int&);
			template <> void insertAttr<unsigned __int64>(H5::Attribute &, std::shared_ptr<H5::AtomType>, const unsigned __int64&);
			template <> void insertAttr<unsigned long long>(H5::Attribute &, std::shared_ptr<H5::AtomType>, const unsigned long long&);
			template <> void insertAttr<float>(H5::Attribute &, std::shared_ptr<H5::AtomType>, const float&);
			template <> void insertAttr<double>(H5::Attribute &, std::shared_ptr<H5::AtomType>, const double&);
			*/

			HDFgroup_t openOrCreateGroup(gsl::not_null<ICEDB_H5_GROUP_OWNER_PTR> base, gsl::not_null<const char*> name)
			{
				HDFgroup_t res;
				try {
					res=std::make_unique<H5::Group>( base->openGroup( name ));
				} catch( H5::GroupIException not_found_error ) {
					res=std::make_unique<H5::Group>(base->createGroup( name ));
				} catch( H5::FileIException not_found_error ) {
					res=std::make_unique<H5::Group>(base->createGroup( name ));
				}
				return res;
			}

			HDFgroup_t openGroup(gsl::not_null<ICEDB_H5_GROUP_OWNER_PTR> base, gsl::not_null<const char*> name)
			{
				HDFgroup_t res;
				try {
					res=(std::make_unique<H5::Group>(base->openGroup( name )));
				} catch( H5::GroupIException not_found_error ) {
					return nullptr;
				} catch( H5::FileIException not_found_error ) {
					return nullptr;
				}
				return res;
			}

			bool attrExists(gsl::not_null<H5::H5Object*> base, gsl::not_null<const char*> name)
			{
				try {
					H5::Attribute(base->openAttribute(name));
					return true;
				}
				catch (H5::AttributeIException not_found_error) {
					return false;
				}
				catch (H5::FileIException not_found_error) {
					return false;
				}
				catch (H5::GroupIException not_found_error) {
					return false;
				}
			}

			bool groupExists(gsl::not_null<ICEDB_H5_GROUP_OWNER_PTR> base, gsl::not_null<const char*> name)
			{
				try {
					H5::Group( base->openGroup( name ));
					return true;
				} catch( H5::GroupIException not_found_error ) {
					return false;
				}
				catch (H5::FileIException not_found_error) {
					return false;
				}
			}

			std::pair<bool,bool> symLinkExists(gsl::not_null<ICEDB_H5_GROUP_OWNER_PTR> base, gsl::not_null<const char*> name)
			{
				bool linkexists = false;
				bool linkgood = false;
				std::string lloc;
				try {
					H5G_stat_t stats;
					base->getObjinfo(name, false, stats);
					linkexists = true;

					//lloc = base->getLinkval(name);
					base->getObjinfo(name, true, stats);
					linkgood = true;
				} catch( H5::GroupIException not_found_error ) {
				} catch( H5::FileIException not_found_error) {
				}
				return std::pair<bool,bool>(linkexists,linkgood);
			}

			bool datasetExists(gsl::not_null<ICEDB_H5_GROUP_OWNER_PTR> base, gsl::not_null<const char*> name)
			{
				try {
					H5::DataSet(base->openDataSet(name));
					return true;
				}
				catch (H5::GroupIException not_found_error) {
					return false;
				}
				catch (H5::FileIException not_found_error) {
					return false;
				}
			}

			std::shared_ptr<H5::DSetCreatPropList> make_plist(size_t rows, size_t cols, bool compress)
			{
				using namespace H5;
				const hsize_t chunk[2] = { static_cast<hsize_t>(rows), static_cast<hsize_t>(cols) };
				auto plist = std::make_shared<H5::DSetCreatPropList>();
				plist->setChunk(2, chunk);
				if (compress)
					plist->setDeflate(6);
				return plist;
			}

			std::set<std::string> getGroupMembers(const ICEDB_H5_GROUP_OWNER &base) {
				std::set<std::string> res;
				const hsize_t numObjs = base.getNumObjs();
				for (hsize_t i = 0; i < numObjs; ++i)
				{
					std::string name = base.getObjnameByIdx(i);
					res.insert(name);
				}
				return res;
			}

			std::map<std::string, H5G_obj_t> getGroupMembersTypes(const ICEDB_H5_GROUP_OWNER &base) {
				std::map<std::string, H5G_obj_t> res;
				const hsize_t numObjs = base.getNumObjs();
				for (hsize_t i = 0; i < numObjs; ++i)
				{
					std::string name = base.getObjnameByIdx(i);
					H5G_obj_t gtype = base.getObjTypeByIdx(i);
					res[name] = gtype;
				}
				return res;
			}

			std::vector<std::string> explode(std::string const & s, char delim)
			{
				std::vector<std::string> result;
				std::istringstream iss(s);

				for (std::string token; std::getline(iss, token, delim); )
				{
					if (token.size())
						result.push_back(std::move(token));
				}

				return result;
			}

			std::vector<std::string> explodeHDF5groupPath(const std::string &s) {
				std::string mountStr = s;
				std::replace(mountStr.begin(), mountStr.end(), '\\', '/');
				return explode(mountStr, '/');
			}

			H5::Group createGroupStructure(const std::string &groupName, ICEDB_H5_GROUP_OWNER &base) {
				// Using the '/' specifier, pop off the group names and 
				// create groups if necessary in the tree. Return the final group.
				Expects(groupName.size() > 0);
				std::string mountStr = groupName;
				std::replace(mountStr.begin(), mountStr.end(), '\\', '/');
				std::vector<std::string> groups = explode(mountStr, '/');

				return std::move(createGroupStructure(groups, base));
			}

			H5::Group createGroupStructure(const std::vector<std::string> &groups, ICEDB_H5_GROUP_OWNER &base) {

				/// \note This is really awkwardly stated because of an MSVC2017 bug.
				/// The HDF5 group copy constructor fails to update its id field, even though
				/// the hid_t is a uint64 and the copy is trivial. These 'relocatable references'
				/// allow me to avoid these copies through liberal use of std::move.
				std::shared_ptr<ICEDB_H5_GROUP_OWNER> current(&base, [](ICEDB_H5_GROUP_OWNER*) {});
				std::vector<H5::Group> vgrps;
				//vgrps.push_back(current);

				//H5::Group current(base);
				for (const auto &grpname : groups) {
					if (!grpname.size()) continue; // Skip any empty entries
					std::set<std::string> members = getGroupMembers(*(current.get()));
					if (!members.count(grpname)) {
						vgrps.push_back(std::move(current->createGroup(grpname)));
						current = std::shared_ptr<H5::Group>(&(*(vgrps.rbegin())), [](H5::Group*) {});
					}
					else {
						//H5::Group newcur = current->openGroup(grpname);
						//current = newcur;
						vgrps.push_back(std::move(current->openGroup(grpname)));
						current = std::shared_ptr<H5::Group>(&(*(vgrps.rbegin())), [](H5::Group*) {});
					}
				}
				return std::move((*(vgrps.rbegin())));
			}


			/// \todo Candidate for constexpr inlining
			unsigned int getHDF5IOflags(fs::IOopenFlags flags) {
				unsigned int Hflags = 0; // HDF5 flags
				if (flags == fs::IOopenFlags::READ_ONLY) Hflags = H5F_ACC_RDONLY;
				else if (flags == fs::IOopenFlags::READ_WRITE) Hflags = H5F_ACC_RDWR;
				else if (flags == fs::IOopenFlags::TRUNCATE) Hflags = H5F_ACC_TRUNC;
				else if (flags == fs::IOopenFlags::CREATE) Hflags = H5F_ACC_CREAT;
				return Hflags;
			}


			template<> bool isType<uint64_t>(hid_t type_id) { return (H5Tequal(type_id, H5T_NATIVE_UINT64) > 0) ? true : false; }
			template<> bool isType<int64_t>(hid_t type_id) { return (H5Tequal(type_id, H5T_NATIVE_INT64) > 0) ? true : false; }
			template<> bool isType<float>(hid_t type_id) { return (H5Tequal(type_id, H5T_NATIVE_FLOAT) > 0) ? true : false; }
			template<> bool isType<double>(hid_t type_id) { return (H5Tequal(type_id, H5T_NATIVE_DOUBLE) > 0) ? true : false; }
			template<> bool isType<char>(hid_t type_id) { return (H5Tequal(type_id, H5T_NATIVE_CHAR) > 0) ? true : false; }
			template<> bool isType<std::string>(hid_t type_id) {
				std::shared_ptr<H5::AtomType> a = std::make_shared<H5::StrType>(0, H5T_VARIABLE);
				return (H5Tequal(type_id, a->getId()) > 0) ? true : false;
			}


			void readDatasetDimensions(gsl::not_null<H5::DataSet*> dataset, std::vector<size_t> &dims) {
				using namespace H5;
				const H5T_class_t type_class = dataset->getTypeClass();
				DataSpace fspace = dataset->getSpace();
				int rank = fspace.getSimpleExtentNdims();
				assert(rank >= 0);
				std::vector<hsize_t> sz(rank);
				const int dimensionality = fspace.getSimpleExtentDims(sz.data(), NULL);
				for (size_t i = 0; i < rank; ++i)
					dims.push_back((size_t)sz[i]);
			}

			size_t readDatasetNumDimensions(gsl::not_null<H5::DataSet*> dataset) {
				using namespace H5;
				const H5T_class_t type_class = dataset->getTypeClass();
				DataSpace fspace = dataset->getSpace();
				int rank = fspace.getSimpleExtentNdims();
				assert(rank >= 0);
				return (size_t)rank;
			}

		}
	}
}

