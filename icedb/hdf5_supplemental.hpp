#pragma once
#include <memory>
#include <set>
#include <string>
#include <vector>
#include <functional>

#include <gsl/gsl>
#include "compat/hdf5_load.h"
#include "fs.hpp"
#include "util.hpp"

namespace icedb {
	namespace fs {
		namespace hdf5 {
			/// Provides a method for calculating the offsets from std::arrays of data
#			define ARRAYOFFSET(TYPE, INDEX) [](){TYPE a; return (size_t) &a[INDEX] - (size_t) &a; }()

			std::set<std::string> getGroupMembers(const H5::Group &base);
			std::map<std::string, H5G_obj_t> getGroupMembersTypes(const H5::Group &base);

			H5::Group createGroupStructure(const std::string &groupName, H5::Group &base);

			typedef std::unique_ptr<H5::Group > HDFgroup_t;
			//typedef std::unique_ptr<H5::Group, mem::icedb_delete<H5::Group> > HDFgroup_t;

			HDFgroup_t openOrCreateGroup(gsl::not_null<H5::H5Location*> base, gsl::not_null<const char*> name);
			HDFgroup_t openGroup(gsl::not_null<H5::H5Location*> base, gsl::not_null<const char*> name);
			bool groupExists(gsl::not_null<H5::H5Location*> base, gsl::not_null<const char*> name);

			/// \param std::shared_ptr<H5::AtomType> is a pointer to a newly-constructed matching type
			/// \returns A pair of (the matching type, a flag indicating passing by pointer or reference)
			typedef std::unique_ptr<H5::AtomType> MatchAttributeTypeType;
			template <class DataType>
			MatchAttributeTypeType MatchAttributeType() {
				static_assert(false, 
					"Unsupported type during attribute conversion in rtmath::plugins::hdf5::MatchAttributeType.");
			}
			extern template MatchAttributeTypeType MatchAttributeType<std::string>();
			extern template MatchAttributeTypeType MatchAttributeType<const char*>();
			extern template MatchAttributeTypeType MatchAttributeType<char>();

			extern template MatchAttributeTypeType MatchAttributeType<uint8_t>();
			extern template MatchAttributeTypeType MatchAttributeType<uint16_t>();
			extern template MatchAttributeTypeType MatchAttributeType<uint32_t>();
			extern template MatchAttributeTypeType MatchAttributeType<uint64_t>();
			extern template MatchAttributeTypeType MatchAttributeType<int8_t>();
			extern template MatchAttributeTypeType MatchAttributeType<int16_t>();
			extern template MatchAttributeTypeType MatchAttributeType<int32_t>();
			extern template MatchAttributeTypeType MatchAttributeType<int64_t>();
			extern template MatchAttributeTypeType MatchAttributeType<float>();
			extern template MatchAttributeTypeType MatchAttributeType<double>();

			/// Check to see if output type is for a string
			template <class DataType> bool isStrType() { return false; }
			extern template bool isStrType<std::string>();
			extern template bool isStrType<const char*>();

			/// Handles proper insertion of strings versus other data types
			template <class DataType>
			void insertAttr(const H5::Attribute &attr, gsl::not_null<H5::AtomType*> vls_type, const DataType& value)
			{
				attr.write(*vls_type, &value);
			}
			template <> void insertAttr<std::string>(const H5::Attribute &attr, gsl::not_null<H5::AtomType*> vls_type, const std::string& value);

			/// Convenient template to add an attribute of a variable type to a group or dataset
			template <class DataType, class Container>
			void addAttr(gsl::not_null<Container*> obj, gsl::not_null<const char*> attname, const DataType &value)
			{
				auto vls_type = MatchAttributeType<DataType>();
				H5::DataSpace att_space(H5S_SCALAR);
				H5::Attribute attr = obj->createAttribute(attname, *vls_type, att_space);
				insertAttr<DataType>(attr, vls_type.get(), value);
			}

			/// Writes an array of objects
			template <class DataType, class Container>
			void addAttrArray(
				gsl::not_null<Container*> obj,
				gsl::not_null<const char*> attname,
				const std::vector<size_t> &dimensionality,
				const std::vector<DataType> &value)
			{
				auto ftype = MatchAttributeType<DataType>();
				H5::ArrayType vls_type(*ftype, static_cast<int>(dimensionality.size()), static_cast<const hsize_t*>(dimensionality.data()));

				H5::DataSpace att_space(H5S_SCALAR);
				H5::Attribute attr = obj->createAttribute(attname, vls_type, att_space);
				attr.write(vls_type, value.data());
			}

			/// Writes a vector of objects
			template <class DataType, class Container>
			void addAttrVector(
				gsl::not_null<Container*> obj,
				gsl::not_null<const char*> attname,
				const std::vector<DataType> &data)
			{
				auto ftype = MatchAttributeType<DataType>();
				hsize_t dsize = static_cast<hsize_t>(data.size());
				H5::DataSpace att_space(1, &dsize);
				H5::Attribute attr = obj->createAttribute(attname, *(ftype.get()), att_space);
				attr.write(*ftype, data.data());
			}

			/// Handles proper insertion of strings versus other data types
			template <class DataType>
			void loadAttr(const H5::Attribute &attr, gsl::not_null<H5::AtomType*> vls_type, DataType& value)
			{
				attr.read(*vls_type, &value);
			}
			template <> void loadAttr<std::string>(const H5::Attribute &attr, gsl::not_null<H5::AtomType*> vls_type, std::string& value);

			/// Convenient template to read an attribute of a variable
			template <class DataType, class Container>
			void readAttr(gsl::not_null<Container*> obj, gsl::not_null<const char*> attname, DataType &value)
			{
				auto vls_type = MatchAttributeType<DataType>();
				H5::DataSpace att_space(H5S_SCALAR);
				H5::Attribute attr = obj->openAttribute(attname); //, *vls_type, att_space);
				loadAttr<DataType>(attr, vls_type.get(), value);
			}

			/// Reads an array (or vector) of objects
			template <class DataType, class Container>
			void readAttrArray(gsl::not_null<Container*> obj, gsl::not_null<const char*> attname,
				std::vector<size_t> &dims,
				std::vector<DataType> &value)
			{
				H5::Attribute attr = obj->openAttribute(attname);
				int dimensionality = attr.getArrayType().getArrayNDims();
				Expects(dimensionality > 0);
				std::vector<hsize_t> sz(static_cast<size_t>(dimensionality));
				attr.getArrayType().getArrayDims(sz.data());

				dims.resize(static_cast<size_t>(dimensionality));
				size_t numElems = 1;
				for (size_t i = 0; i < dimensionality; ++i) {
					dims[i] = static_cast<size_t>(sz[i]);
					numElems *= dims[i];
				}
				value.resize(numElems);

				auto ftype = MatchAttributeType<DataType>();
				//H5::IntType ftype(H5::PredType::NATIVE_FLOAT);
				H5::ArrayType vls_type(*ftype, dimensionality, sz.data());

				//H5::DataSpace att_space(H5S_SCALAR);
				//H5::Attribute attr = obj->createAttribute(attname, vls_type, att_space);
				attr.read(vls_type, value.data());
			}

			/// Reads an array (or vector) of objects
			template <class DataType, class Container>
			void readAttrVector(gsl::not_null<Container*> obj, gsl::not_null<const char*> attname,
				std::vector<DataType> &value)
			{
				/*
				std::shared_ptr<H5::AtomType> ftype = MatchAttributeType<DataType>();
				hsize_t dsize = (hsize_t) data.size();
				H5::DataSpace att_space(1, &dsize);
				H5::Attribute attr = obj->createAttribute(attname, *(ftype.get()), att_space);
				attr.write(*ftype, data.data());
				*/
				H5::Attribute attr = obj->openAttribute(attname);
				auto ftype = MatchAttributeType<DataType>();
				H5::DataSpace dspace = attr.getSpace();
				value.resize(dspace.getSimpleExtentNdims());
				attr.read(*(ftype.get()), value.data());
			}

			enum class DataContainerType {
				BASIC,
				ARRAY,
				COMPOUND,
				ENUM,
				OPAQUE,
				VLEN,
				STRING,
				UNKNOWN
			};

			template <class Container>
			DataContainerType getAttributeGroupingType(gsl::not_null<Container*> obj, gsl::not_null<const char*> attname)
			{
				H5::Attribute attr = obj->openAttribute(attname);
				const hid_t attrid = attr.getId();
				H5::DataType dtype = attr.getDataType().getId();
				const hid_t dtypeid = dtype.getId();
				const H5T_class_t class_type = H5Tget_class(dtypeid);
				if (class_type == H5T_class_t::H5T_ARRAY) return DataContainerType::ARRAY;
				else if (class_type == H5T_class_t::H5T_COMPOUND) return DataContainerType::COMPOUND;
				else if (class_type == H5T_class_t::H5T_ENUM) return DataContainerType::ENUM;
				else if (class_type == H5T_class_t::H5T_OPAQUE) return DataContainerType::OPAQUE;
				else if (class_type == H5T_class_t::H5T_INTEGER) return DataContainerType::BASIC;
				else if (class_type == H5T_class_t::H5T_FLOAT) return DataContainerType::BASIC;
				else if (class_type == H5T_class_t::H5T_VLEN) return DataContainerType::VLEN;
				else if (class_type == H5T_class_t::H5T_STRING) return DataContainerType::STRING;

				return DataContainerType::UNKNOWN;
			}

			template <class Container>
			std::vector<hsize_t> getAttrArrayDimensionality(
				gsl::not_null<Container*> obj, gsl::not_null<const char*> attname)
			{
				std::vector<hsize_t> dims;
				H5::Attribute attr = obj->openAttribute(attname);
				int dimensionality = attr.getArrayType().getArrayNDims();
				dims.resize(dimensionality);
				attr.getArrayType().getArrayDims(dims.data());
				return dims;
			}

			bool attrExists(gsl::not_null<H5::H5Object*> obj, gsl::not_null<const char*> attname);

			/// Convenience function to either open or create a group
			HDFgroup_t openOrCreateGroup(
				gsl::not_null<H5::H5Location*> base, gsl::not_null<const char*> name);

			/// Convenience function to check if a given group exists
			bool groupExists(gsl::not_null<H5::H5Location*> base, gsl::not_null<const char*> name);

			/// Convenience function to check if a symbolic link exists, and if the object being 
			/// pointed to also exists.
			/// \returns std::pair<bool,bool> refers to, respectively, if a symbolic link is found and 
			/// if the symbolic link is good.
			std::pair<bool, bool> symLinkExists(gsl::not_null<H5::H5Location*> base, gsl::not_null<const char*> name);

			/// \brief Convenience function to open a group, if it exists
			/// \returns nullptr is the group does not exist.
			HDFgroup_t openGroup(gsl::not_null<H5::H5Location*> base, gsl::not_null<const char*> name);

			/// Convenience function to check if a given dataset exists
			bool datasetExists(gsl::not_null<H5::H5Location*> base, gsl::not_null<const char*> name);

			//typedef std::unique_ptr<H5::DataSet, mem::icedb_delete<H5::DataSet> > HDFdataset_t;
			typedef std::unique_ptr<H5::DataSet > HDFdataset_t;


			template <class DataType, class Container>
			HDFdataset_t addDatasetArray(
				gsl::not_null<Container*> obj, gsl::not_null<const char*> name, size_t rows, size_t cols,
				gsl::not_null<const DataType*> values, H5::DSetCreatPropList* iplist = nullptr)
			{
				using namespace H5;
				std::vector<hsize_t> sz{ (hsize_t)rows, (hsize_t)cols };
				int dimensionality = 2;
				if (cols == 1) dimensionality = 1;
				DataSpace fspace(dimensionality, sz);
				auto ftype = MatchAttributeType<DataType>();
				std::shared_ptr<DSetCreatPropList> plist;
				if (iplist) plist = iplist;
				else
				{
					plist = std::shared_ptr<DSetCreatPropList>(new DSetCreatPropList);
					if (!isStrType<DataType>())
					{
						int fillvalue = -1;
						plist->setFillValue(PredType::NATIVE_INT, &fillvalue);
						if (useZLIB()) {
							plist->setChunk(2, sz);
							plist->setDeflate(6);
						}
					}
				}

				HDFdataset_t dataset(new DataSet(obj->createDataSet(name, *(ftype.get()),
					fspace, *(plist.get()))));
				dataset->write(values, *(ftype.get()));
				return dataset;
			}

			template <class DataType, class Container>
			HDFdataset_t addDatasetArray(
				gsl::not_null<Container*> obj, gsl::not_null<const char*> name, size_t rows,
				gsl::not_null<const DataType*> values, H5::DSetCreatPropList* iplist = nullptr)
			{
				return std::move(addDatasetArray(obj, name, rows, 1, values, iplist));
			}
			
			template <class DataType, class Container>
			gsl::not_null<Container*> addDatasetArray(
				const std::vector<size_t> &dimensions,
				gsl::not_null<Container*> obj,
				gsl::not_null<const DataType*> values)
			{
				using namespace H5;
				std::vector<hsize_t> sz(dimensions.size());
				for (size_t i = 0; i < sz.size(); ++i)
					sz[i] = static_cast<hsize_t>(dimensions[i]);
				//DataSpace fspace(static_cast<int>(sz.size()), sz);
				auto ftype = MatchAttributeType<DataType>();

				obj->write(values, *(ftype.get()));
				return obj;
			}


			template <class Container>
			HDFdataset_t readDatasetDimensions(
				gsl::not_null<Container*> obj,
				gsl::not_null<const char*> name,
				std::vector<size_t> &out)
			{
				using namespace H5;

				HDFdataset_t dataset(new H5::DataSet(obj->openDataSet(name)));
				H5T_class_t type_class = dataset->getTypeClass();
				DataSpace fspace = dataset->getSpace();
				int rank = fspace.getSimpleExtentNdims();

				std::vector<hsize_t> sz(rank);
				int dimensionality = fspace.getSimpleExtentDims( sz.data(), NULL);
				for (size_t i = 0; i < rank; ++i)
					out.push_back(sz[i]);

				return dataset;
			}

			void readDatasetDimensions(gsl::not_null<H5::DataSet*> dataset, std::vector<size_t> &dims);

			size_t readDatasetNumDimensions(gsl::not_null<H5::DataSet*> dataset);

			template <class DataType, class Container>
			HDFdataset_t readDatasetArray(
				gsl::not_null<Container*> obj, gsl::not_null<const char*> name,
				gsl::not_null<DataType*> values)
			{
				using namespace H5;

				HDFdataset_t dataset(new H5::DataSet(obj->openDataSet(name)));
				H5T_class_t type_class = dataset->getTypeClass();
				DataSpace fspace = dataset->getSpace();

				//DataSpace fspace(dimensionality, sz);
				auto ftype = MatchAttributeType<DataType>();

				dataset->read(values, *(ftype.get()));
				return std::move(dataset);
			}

			template <class DataType, class Container>
			gsl::not_null<Container*> readDatasetArray(
				gsl::not_null<Container*> dataset,
				gsl::not_null<DataType*> values)
			{
				using namespace H5;

				H5T_class_t type_class = dataset->getTypeClass();
				DataSpace fspace = dataset->getSpace();

				//DataSpace fspace(dimensionality, sz);
				auto ftype = MatchAttributeType<DataType>();

				dataset->read(values, *(ftype.get()));
				return dataset;
			}

			/// \brief Add column names to table.
			/// \param num is the number of columns
			/// \param stride allows name duplication (for vectors)
			template <class Container>
			void addNames(
				gsl::not_null<Container*> obj,
				const std::string &prefix,
				size_t num,
				const std::function<std::string(int)> &s, size_t stride = 0, size_t mCols = 0)
			{
				size_t nstride = stride;
				if (!nstride) nstride = 1;
				for (size_t i = 0; i < num; ++i)
				{
					size_t j = i / nstride;
					std::string lbl = s((int)j);
					std::ostringstream fldname;
					fldname << prefix;
					fldname.width(2);
					fldname.fill('0');
					fldname << std::right << i << "_NAME";
					std::string sfldname = fldname.str();
					addAttr<std::string, Container>(obj, sfldname.c_str(), lbl);
				}
			}


			/// \brief Add column names to table.
			/// \param num is the number of columns
			/// \param stride allows name duplication (for vectors)
			template <class Container>
			void addColNames(gsl::not_null<Container*> obj, size_t num, const std::function<std::string(int)> &s, size_t stride = 0, size_t mCols = 0)
			{
				size_t nstride = stride;
				if (!nstride) nstride = 1;
				for (size_t i = 0; i < num; ++i)
				{
					size_t j = i / nstride;
					std::string lbl = s((int)j);
					std::ostringstream fldname;
					fldname << "COLUMN_";
					fldname.width(2);
					fldname.fill('0');
					fldname << std::right << i << "_NAME";
					std::string sfldname = fldname.str();
					if (stride)
					{
						size_t k = i % nstride;
						if (!mCols) {
							if (k == 0) lbl.append("_X");
							if (k == 1) lbl.append("_Y");
							if (k == 2) lbl.append("_Z");
							if (k == 3) lbl.append("_R");
						}
						else {
							size_t row = k / mCols;
							size_t col = k % mCols;
							std::ostringstream iappend;
							iappend << "_" << row << "_" << col;
							lbl.append(iappend.str());
						}
					}
					addAttr<std::string, Container>(obj, sfldname.c_str(), lbl);
				}
			}


			/// Creates a property list with the compression + chunking as specified
			std::shared_ptr<H5::DSetCreatPropList> make_plist(size_t rows, size_t cols, bool compress = true);

			std::vector<std::string> explode(std::string const & s, char delim);
			std::vector<std::string> explodeHDF5groupPath(const std::string &s);

			H5::Group createGroupStructure(const std::string &groupName, H5::Group &base);
			H5::Group createGroupStructure(const std::vector<std::string> &groupNames, H5::Group &base);

			unsigned int getHDF5IOflags(enum class fs::IOopenFlags flag);

			/// Functions to detect the _type_ of data
			template <class DataType>
			bool isType(hid_t) { static_assert(false, "Unsupported type."); return false; }

			template <class DataType, class Container>
			bool isType(gsl::not_null<Container*> obj, const std::string &attributeName) {
				H5::Attribute attr = obj->openAttribute(attributeName);
				return isType<DataType>(attr.getDataType().getId());
			}
			template <class DataType, class Container>
			bool isType(gsl::not_null<Container*> obj) {
				return isType<DataType>(obj->getDataType().getId());
			}
			extern template bool isType<uint64_t>(hid_t type_id);
			extern template bool isType<int64_t>(hid_t type_id);
			extern template bool isType<float>(hid_t type_id);
			extern template bool isType<double>(hid_t type_id);
			extern template bool isType<char>(hid_t type_id);
			extern template bool isType<std::string>(hid_t type_id);

		}
	}
}
