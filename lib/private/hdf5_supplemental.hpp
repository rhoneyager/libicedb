#pragma once
#include <memory>
#include <set>
#include <string>
#include <vector>
#include <functional>
#include <stdexcept>
#include <sstream>
#include <iostream> // For debugging

#include <gsl/gsl>
#include "icedb_h5.h" // Auto-generated. Gets installed in the correct location.
#include "hdf5_load.h"
#include "../icedb/fs.hpp"
#include "../icedb/util.hpp"

#define INST_ATTR(y) \
			y(double); \
			y(float); \
			y(uint64_t); \
			y(int64_t); \
			y(uint32_t); \
			y(int32_t); \
			y(uint16_t); \
			y(int16_t); \
			y(uint8_t); \
			y(int8_t); \
			y(char); \
			y(std::string);

namespace icedb {
	namespace fs {
		namespace hdf5 {
			int useZLIB();
			void useZLIB(int);

			/// Provides a method for calculating the offsets from std::arrays of data
#			define ARRAYOFFSET(TYPE, INDEX) [](){TYPE a; return (size_t) &a[INDEX] - (size_t) &a; }()

			std::set<std::string> getGroupMembers(const ICEDB_H5_GETNUMOBJS_OWNER &base);
			std::map<std::string, H5G_obj_t> getGroupMembersTypes(const ICEDB_H5_GETNUMOBJS_OWNER &base);


			typedef std::unique_ptr<H5::Group > HDFgroup_t;
			//typedef std::unique_ptr<H5::Group, mem::icedb_delete<H5::Group> > HDFgroup_t;

			HDFgroup_t openOrCreateGroup(gsl::not_null<ICEDB_H5_GROUP_OWNER_PTR> base, gsl::not_null<const char*> name);
			HDFgroup_t openGroup(gsl::not_null<ICEDB_H5_GROUP_OWNER_PTR> base, gsl::not_null<const char*> name);
			bool groupExists(gsl::not_null<ICEDB_H5_GROUP_OWNER_PTR> base, gsl::not_null<const char*> name);

			/// \param std::shared_ptr<H5::AtomType> is a pointer to a newly-constructed matching type
			/// \returns A pair of (the matching type, a flag indicating passing by pointer or reference)
			typedef std::unique_ptr<H5::AtomType> MatchAttributeTypeType;
			template <class DataType>
			MatchAttributeTypeType MatchAttributeType()
			{
				throw(std::invalid_argument("Unsupported type during attribute conversion in rtmath::plugins::hdf5::MatchAttributeType."));
			}
			template<> MatchAttributeTypeType MatchAttributeType<std::string>();
			template<> MatchAttributeTypeType MatchAttributeType<const char*>();
			template<> MatchAttributeTypeType MatchAttributeType<char>();

			template<> MatchAttributeTypeType MatchAttributeType<uint8_t>();
			template<> MatchAttributeTypeType MatchAttributeType<uint16_t>();
			template<> MatchAttributeTypeType MatchAttributeType<uint32_t>();
			template<> MatchAttributeTypeType MatchAttributeType<uint64_t>();
			template<> MatchAttributeTypeType MatchAttributeType<int8_t>();
			template<> MatchAttributeTypeType MatchAttributeType<int16_t>();
			template<> MatchAttributeTypeType MatchAttributeType<int32_t>();
			template<> MatchAttributeTypeType MatchAttributeType<int64_t>();
			template<> MatchAttributeTypeType MatchAttributeType<float>();
			template<> MatchAttributeTypeType MatchAttributeType<double>();

			/// Check to see if output type is for a string
			template <class DataType> bool isStrType() { return false; }
			template<> bool isStrType<std::string>();
			template<> bool isStrType<const char*>();

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
				//H5::DataSpace att_space(H5S_SCALAR); // Will not work with older NetCDF versions
				//H5::DataSpace att_space(H5S_SIMPLE);
				hsize_t dsize = 1; //static_cast<hsize_t>(data.size());
				H5::DataSpace att_space(1, &dsize);
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
				std::vector<hsize_t> hdims;
				for (const auto &d : dimensionality)
					hdims.push_back(static_cast<hsize_t>(d));

				// This newer form can be read by old NetCDF versions (like 4.1.1, 01/2014)
				// The older form could only be read by newer NetCDF versions
				// Of course, NC can only ever read singular-dimension attributes.
				H5::DataSpace att_space(static_cast<int>(dimensionality.size()), hdims.data());
				H5::Attribute attr = obj->createAttribute(attname, *(ftype.get()), att_space);
				attr.write(*ftype, value.data());
				
				// Older form cannot be read by older versions of NetCDF.
				//H5::ArrayType vls_type(*ftype, static_cast<int>(dimensionality.size()), (hdims.data()));
				//H5::DataSpace att_space(H5S_SCALAR); // Will not work with older NetCDF versions.
				//H5::Attribute attr = obj->createAttribute(attname, vls_type, att_space);
				//attr.write(vls_type, value.data());
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
				//const hid_t attrid = attr.getId();
				H5::DataType dtype = attr.getDataType();
				//const hid_t dtypeid = dtype.getId();
				//const H5T_class_t class_type = H5Tget_class(dtypeid);
				H5T_class_t class_type = dtype.getClass();
				// I could also use the H5::DataType::detectClass method here...
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

			/// Convenience function to check if a symbolic link exists, and if the object being 
			/// pointed to also exists.
			/// \returns std::pair<bool,bool> refers to, respectively, if a symbolic link is found and 
			/// if the symbolic link is good.
			std::pair<bool, bool> symLinkExists(gsl::not_null<ICEDB_H5_GROUP_OWNER_PTR> base, gsl::not_null<const char*> name);

			// \brief Convenience function to open a group, if it exists
			// \returns nullptr is the group does not exist.
			//HDFgroup_t openGroup(gsl::not_null<H5::H5Location*> base, gsl::not_null<const char*> name);

			/// Convenience function to check if a given dataset exists
			bool datasetExists(gsl::not_null<ICEDB_H5_GROUP_OWNER_PTR> base, gsl::not_null<const char*> name);

			//typedef std::unique_ptr<H5::DataSet, mem::icedb_delete<H5::DataSet> > HDFdataset_t;
			typedef std::unique_ptr<H5::DataSet > HDFdataset_t;


			template <class DataType, class Container>
			HDFdataset_t addDatasetArray(
				gsl::not_null<Container*> obj, gsl::not_null<const char*> name, size_t rows, size_t cols,
				gsl::not_null<const DataType*> values, std::shared_ptr<H5::DSetCreatPropList> iplist = nullptr)
			{
				using namespace H5;
				std::vector<hsize_t> sz{ (hsize_t)rows, (hsize_t)cols };
				int dimensionality = 2;
				if (cols == 1) dimensionality = 1;
				H5::DataSpace fspace(dimensionality, sz.data());
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
							plist->setChunk(static_cast<int>(sz.size()), sz.data());
							plist->setDeflate(useZLIB());
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
				gsl::not_null<const DataType*> values, std::shared_ptr<H5::DSetCreatPropList> iplist = nullptr)
			{
				return std::move(addDatasetArray(obj, name, rows, 1, values, iplist));
			}
			
			template <class DataType, class Container>
			gsl::not_null<H5::DataSet*> writeDatasetArray(
				const std::vector<size_t> &dimensions,
				gsl::not_null<H5::DataSet*> dset,
				gsl::not_null<const DataType*> values)
			{
				using namespace H5;
				std::vector<hsize_t> sz(dimensions.size());
				for (size_t i = 0; i < sz.size(); ++i)
					sz[i] = static_cast<hsize_t>(dimensions[i]);
				//DataSpace fspace(static_cast<int>(sz.size()), sz);
				auto ftype = MatchAttributeType<DataType>();

				dset->write(values, *(ftype.get()));
				return dset;
			}

			template <class DataType, class Container>
			HDFdataset_t createDatasetRaw(
				gsl::not_null<Container*> parent, 
				gsl::not_null<const char*> name, 
				const std::vector<size_t> &dims, 
				std::shared_ptr<H5::DSetCreatPropList> iplist = nullptr)
			{
				using namespace H5;
				std::vector<hsize_t> sz(dims.size());
				for (size_t i = 0; i < sz.size(); ++i)
					sz[i] = static_cast<hsize_t>(dims[i]);

				DataSpace fspace(static_cast<int>(sz.size()), sz.data());
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
							plist->setChunk(static_cast<int>(sz.size()), sz.data());
							plist->setDeflate(useZLIB());
						}
					}
				}

				HDFdataset_t dataset(new DataSet(parent->createDataSet(name, *(ftype.get()),
					fspace, *(plist.get()))));
				return dataset;
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
					fldname << std::string(prefix);
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

			H5::Group createGroupStructure(const std::string &groupName, ICEDB_H5_GROUP_OWNER &base);
			H5::Group createGroupStructure(const std::vector<std::string> &groupNames, ICEDB_H5_GROUP_OWNER &base);


			unsigned int getHDF5IOflags(fs::IOopenFlags flag);

			/// Functions to detect the _type_ of data
			template <class DataType>
			bool isType(hid_t) { throw(std::invalid_argument("Unsupported type. (icedb::fs::hdf5::isType(hid_t))")); return false; }

			template <class DataType, class Container>
			bool isType(gsl::not_null<const Container*> obj, const std::string &attributeName) {
				H5::Attribute attr = obj->openAttribute(attributeName);
				return isType<DataType>(attr.getDataType().getId());
			}
			template <class DataType, class Container>
			bool isType(gsl::not_null<const Container*> obj) {
				return isType<DataType>(obj->getDataType().getId());
			}
			template<> bool isType<uint64_t>(hid_t type_id);
			template<> bool isType<int64_t>(hid_t type_id);
			template<> bool isType<uint32_t>(hid_t type_id);
			template<> bool isType<int32_t>(hid_t type_id);
			template<> bool isType<uint16_t>(hid_t type_id);
			template<> bool isType<int16_t>(hid_t type_id);
			template<> bool isType<uint8_t>(hid_t type_id);
			template<> bool isType<int8_t>(hid_t type_id);
			template<> bool isType<float>(hid_t type_id);
			template<> bool isType<double>(hid_t type_id);
			template<> bool isType<char>(hid_t type_id);
			template<> bool isType<std::string>(hid_t type_id);

		}
	}
}
