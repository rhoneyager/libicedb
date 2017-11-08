#pragma once
#include <memory>
#include <set>
#include <string>
#include <vector>
#include <functional>

#include "compat/hdf5_load.h"
#include "fs.hpp"

namespace icedb {
	namespace fs {
		namespace hdf5 {
			/// Provides a method for calculating the offsets from std::arrays of data
#			define ARRAYOFFSET(TYPE, INDEX) [](){TYPE a; return (size_t) &a[INDEX] - (size_t) &a; }()

			std::set<std::string> getGroupMembers(const H5::Group &base);
			H5::Group createGroupStructure(const std::string &groupName, H5::Group &base);

			std::shared_ptr<H5::Group> openOrCreateGroup(std::shared_ptr<H5::H5Location> base, const char* name);
			std::shared_ptr<H5::Group> openGroup(std::shared_ptr<H5::H5Location> base, const char* name);
			bool groupExists(std::shared_ptr<H5::H5Location> base, const char* name);

			/// \param std::shared_ptr<H5::AtomType> is a pointer to a newly-constructed matching type
			/// \returns A pair of (the matching type, a flag indicating passing by pointer or reference)
			typedef std::shared_ptr<H5::AtomType> MatchAttributeTypeType;
			template <class DataType>
			MatchAttributeTypeType MatchAttributeType();

			/// Check to see if output type is for a string
			template <class DataType> bool isStrType() { return false; }
			template<> bool isStrType<std::string>();
			template<> bool isStrType<const char*>();

			/// Handles proper insertion of strings versus other data types
			template <class DataType>
			void insertAttr(H5::Attribute &attr, std::shared_ptr<H5::AtomType> vls_type, const DataType& value)
			{
				attr.write(*vls_type, &value);
			}
			template <> void insertAttr<std::string>(H5::Attribute &attr, std::shared_ptr<H5::AtomType> vls_type, const std::string& value);

			/// Convenient template to add an attribute of a variable type to a group or dataset
			template <class DataType, class Container>
			void addAttr(std::shared_ptr<Container> obj, const char* attname, const DataType &value)
			{
				std::shared_ptr<H5::AtomType> vls_type = MatchAttributeType<DataType>();
				H5::DataSpace att_space(H5S_SCALAR);
				H5::Attribute attr = obj->createAttribute(attname, *vls_type, att_space);
				insertAttr<DataType>(attr, vls_type, value);
			}

			/// Writes an array (or vector) of objects
			template <class DataType, class Container>
			void addAttrArray(std::shared_ptr<Container> obj, const char* attname, 
					const DataType *value, size_t rows, size_t cols)
			{
				hsize_t sz[] = { (hsize_t) rows, (hsize_t) cols };
				if (sz[0] == 1)
				{
					sz[0] = sz[1];
					sz[1] = 1;
				}
				int dimensionality = (sz[1] == 1) ? 1 : 2;

				std::shared_ptr<H5::AtomType> ftype = MatchAttributeType<DataType>();
				//H5::IntType ftype(H5::PredType::NATIVE_FLOAT);
				H5::ArrayType vls_type(*ftype, dimensionality, sz);

				H5::DataSpace att_space(H5S_SCALAR);
				H5::Attribute attr = obj->createAttribute(attname, vls_type, att_space);
				attr.write(vls_type, value);
			}


			/// Writes an array (or vector) of objects
			template <class DataType, class Container>
			void addAttrVector(
				std::shared_ptr<Container> obj, 
				const char* attname,
				const std::vector<size_t> &dimensionality,
				const std::vector<DataType> &value)
			{
				std::shared_ptr<H5::AtomType> ftype = MatchAttributeType<DataType>();
				H5::ArrayType vls_type(*ftype, (int) dimensionality.size(), (hsize_t*) dimensionality.data());

				H5::DataSpace att_space(H5S_SCALAR);
				H5::Attribute attr = obj->createAttribute(attname, vls_type, att_space);
				attr.write(vls_type, value.data());
			}

			
			/// Handles proper insertion of strings versus other data types
			template <class DataType>
			void loadAttr(H5::Attribute &attr, std::shared_ptr<H5::AtomType> vls_type, DataType& value)
			{
				attr.read(*vls_type, &value);
			}
			template <> void loadAttr<std::string>(H5::Attribute &attr, std::shared_ptr<H5::AtomType> vls_type, std::string& value);


			/// Convenient template to read an attribute of a variable
			template <class DataType, class Container>
			void readAttr(std::shared_ptr<Container> obj, const char* attname, DataType &value)
			{
				std::shared_ptr<H5::AtomType> vls_type = MatchAttributeType<DataType>();
				H5::DataSpace att_space(H5S_SCALAR);
				H5::Attribute attr = obj->openAttribute(attname); //, *vls_type, att_space);
				loadAttr<DataType>(attr, vls_type, value);
			}

			/// Reads an array (or vector) of objects
			template <class DataType, class Container>
			void readAttrArray(std::shared_ptr<Container> obj, const char* attname,
				DataType *value, size_t rows, size_t cols)
			{
				H5::Attribute attr = obj->openAttribute(attname);
				int dimensionality = attr.getArrayType().getArrayNDims();
				hsize_t *sz = new hsize_t[dimensionality];
				attr.getArrayType().getArrayDims(sz);

				if (dimensionality == 1)
				{
					if (sz[0] != rows && sz[0] != cols) throw("Row/column mismatch in readAttrArray");
				}
				else {
					if (sz[0] != rows) throw("Rows mismatch in readAttrArray");
					if (sz[1] != cols) throw("Cols mismatch in readAttrArray");
				}
				

				//if (dimensionality == 2)
				//	value.resize(sz[0], sz[1]);
				//else if (dimensionality == 1)
				//	value.resize(sz[0]);

				std::shared_ptr<H5::AtomType> ftype = MatchAttributeType<DataType>();
				//H5::IntType ftype(H5::PredType::NATIVE_FLOAT);
				H5::ArrayType vls_type(*ftype, dimensionality, sz);

				//H5::DataSpace att_space(H5S_SCALAR);
				//H5::Attribute attr = obj->createAttribute(attname, vls_type, att_space);
				attr.read(vls_type, value);
				delete[] sz;
			}


			/// Reads an array (or vector) of objects
			template <class DataType, class Container>
			void readAttrVector(std::shared_ptr<Container> obj, const char* attname,
				std::vector<DataType> &value)
			{
				H5::Attribute attr = obj->openAttribute(attname);
				int dimensionality = attr.getArrayType().getArrayNDims();
				hsize_t *sz = new hsize_t[dimensionality];
				attr.getArrayType().getArrayDims(sz);
				hsize_t numElems = 1;
				for (size_t i = 0; i < dimensionality; ++i) numElems *= sz[i];
				value.resize(numElems);

				std::shared_ptr<H5::AtomType> ftype = MatchAttributeType<DataType>();
				//H5::IntType ftype(H5::PredType::NATIVE_FLOAT);
				H5::ArrayType vls_type(*ftype, dimensionality, sz);

				//H5::DataSpace att_space(H5S_SCALAR);
				//H5::Attribute attr = obj->createAttribute(attname, vls_type, att_space);
				attr.read(vls_type, value.data());
				delete[] sz;
			}

			template <class Container>
			std::vector<hsize_t> getAttrDimensionality(
				std::shared_ptr<Container> obj, const char* attname)
			{
				std::vector<hsize_t> dims;
				H5::Attribute attr = obj->openAttribute(attname);
				int dimensionality = attr.getArrayType().getArrayNDims();
				dims.resize(dimensionality);
				attr.getArrayType().getArrayDims(dims.data());
				return dims;
			}

			bool attrExists(std::shared_ptr<H5::H5Object> obj, const char* attname);

			/// Convenience function to either open or create a group
			std::shared_ptr<H5::Group> openOrCreateGroup(
				std::shared_ptr<H5::H5Location> base, const char* name);

			/// Convenience function to check if a given group exists
			bool groupExists(std::shared_ptr<H5::H5Location> base, const char* name);

			/// Convenience function to check if a symbolic link exists, and if the object being 
			/// pointed to also exists.
			/// \returns std::pair<bool,bool> refers to, respectively, if a symbolic link is found and 
			/// if the symbolic link is good.
			std::pair<bool, bool> symLinkExists(std::shared_ptr<H5::H5Location> base, const char* name);

			/// \brief Convenience function to open a group, if it exists
			/// \returns nullptr is the group does not exist.
			std::shared_ptr<H5::Group> openGroup(std::shared_ptr<H5::H5Location> base, const char* name);

			/// Convenience function to check if a given dataset exists
			bool datasetExists(std::shared_ptr<H5::H5Location> base, const char* name);

			template <class DataType, class Container>
			std::shared_ptr<H5::DataSet> addDatasetArray(std::shared_ptr<Container> obj, const char* name, size_t rows, size_t cols, 
				const DataType *values, std::shared_ptr<H5::DSetCreatPropList> iplist = nullptr)
			{
				using namespace H5;
				hsize_t sz[] = { (hsize_t) rows, (hsize_t) cols };
				int dimensionality = 2;
				if (cols == 1) dimensionality = 1;
				DataSpace fspace(dimensionality, sz);
				std::shared_ptr<H5::AtomType> ftype = MatchAttributeType<DataType>();
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

				std::shared_ptr<DataSet> dataset(new DataSet(obj->createDataSet(name, *(ftype.get()), 
					fspace, *(plist.get()))));
				dataset->write(values, *(ftype.get()));
				return dataset;
			}

			template <class DataType, class Container>
			std::shared_ptr<H5::DataSet> addDatasetArray(std::shared_ptr<Container> obj, const char* name, size_t rows, 
				const DataType *values, std::shared_ptr<H5::DSetCreatPropList> iplist = nullptr)
			{
				return addDatasetArray(obj, name, rows, 1, values, iplist);
			}
			
			template <class Container>
			std::shared_ptr<H5::DataSet> readDatasetDimensions(std::shared_ptr<Container> obj, const char* name, std::vector<size_t> &out)
			{
				using namespace H5;

				std::shared_ptr<H5::DataSet> dataset(new H5::DataSet(obj->openDataSet(name)));
				H5T_class_t type_class = dataset->getTypeClass();
				DataSpace fspace = dataset->getSpace();
				int rank = fspace.getSimpleExtentNdims();

				hsize_t *sz = new hsize_t[rank];
				int dimensionality = fspace.getSimpleExtentDims( sz, NULL);
				for (size_t i = 0; i < rank; ++i)
					out.push_back(sz[i]);

				delete[] sz;
				return dataset;
			}

			template <class DataType, class Container>
			std::shared_ptr<H5::DataSet> readDatasetArray(std::shared_ptr<Container> obj, const char* name,
				DataType *values)
			{
				using namespace H5;

				std::shared_ptr<H5::DataSet> dataset(new H5::DataSet(obj->openDataSet(name)));
				H5T_class_t type_class = dataset->getTypeClass();
				DataSpace fspace = dataset->getSpace();
				/*
				int rank = fspace.getSimpleExtentNdims();

				hsize_t *sz = new hsize_t[rank];
				int dimensionality = fspace.getSimpleExtentDims( sz, NULL);

				if (dimensionality == 2)
				value.resize(sz[0], sz[1]);
				else if (dimensionality == 1)
				{
				// Odd, but it keeps row and column-vectors separate.
				if (value.cols() == 1)
				value.resize(sz[0], 1);
				else value.resize(1, sz[0]);
				}
				*/

				//DataSpace fspace(dimensionality, sz);
				std::shared_ptr<H5::AtomType> ftype = MatchAttributeType<DataType>();

				dataset->read(values, *(ftype.get()));
				//delete[] sz;
				return dataset;
			}

			/// \brief Add column names to table.
			/// \param num is the number of columns
			/// \param stride allows name duplication (for vectors)
			template <class Container>
			void addNames(std::shared_ptr<Container> obj, const std::string &prefix, size_t num, const std::function<std::string(int)> &s, size_t stride = 0, size_t mCols = 0)
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
			void addColNames(std::shared_ptr<Container> obj, size_t num, const std::function<std::string(int)> &s, size_t stride = 0, size_t mCols = 0)
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


			std::set<std::string> getGroupMembers(const H5::Group &base);

			std::vector<std::string> explode(std::string const & s, char delim);
			std::vector<std::string> explodeHDF5groupPath(const std::string &s);

			H5::Group createGroupStructure(const std::string &groupName, H5::Group &base);
			H5::Group createGroupStructure(const std::vector<std::string> &groupNames, H5::Group &base);

			unsigned int getHDF5IOflags(enum class fs::IOopenFlags flag);

			/// Functions to detect the _type_ of data
			template <class DataType>
			bool isType(hid_t) { return false; }

			template <class DataType, class Container>
			bool isType(std::shared_ptr<Container> obj, const std::string &attributeName) {
				H5::Attribute attr = obj->openAttribute(attributeName);
				return isType<DataType>(attr.getDataType().getId());
			}

		}
	}
}
