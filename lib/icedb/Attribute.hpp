#pragma once
#error "deprecated"
#include <memory>
#include <set>
#include <string>
#include <vector>
#include <gsl/gsl>
#include "Data_Types.hpp"


namespace icedb {
	/// Contains everything concerning attributes
	namespace Attributes {
		/** \brief This class defines an attribute.
		*
		* Attributes provide "additional information" to a table or a group.
		* They have a type and a size, and are read and written as complete objects.
		*
		* Attributes must be explicitly written to an object inheriting from CanHaveAttributes.
		* \see Attributes for usage instructions
		* \see CanHaveAttributes
		**/
		template <class DataType> class Attribute {
		protected:
			Attribute() : Attribute("", {}, {}) {}
		public:
			typedef DataType type;
			/// The data contained in the attribute. 
			/// The size of this data must match the number of elements defined by the attribute's dimensionality.
			std::vector<DataType> data;
			/// The name of the attribute
			std::string name;
			/// The attribute's dimensional span. NetCDF only accepts one-dimensional attributes.
			std::vector<size_t> dimensionality;
			/// Does this attribute span more than one dimension?
			inline bool isArray() const {
				if (dimensionality.size() > 1) return true;
				return false;
			}

			/// Create an empty attribute
			Attribute(const std::string &name) : Attribute(name, {}, {}) {}
			/// Create a zero-dimensional attribute with pre-populated data.
			Attribute(const std::string &name, DataType val)
				: Attribute(name, { 1 }, { val }) {}
			/// Create a multi-dimensional attribute with pre-populated data.
			Attribute(const std::string &name, std::initializer_list<size_t> dims, std::initializer_list<DataType> data)
				: name{ name }, dimensionality{ dims }, data{ data }
			{
				static_assert(icedb::Data_Types::Is_Valid_Data_Type<DataType>() == true,
					"Attributes must be a valid data type");

				//size_t sz = 1;
				//for (const auto &d : dims) sz *= d;
				//Expects(data.size() == sz);
			}
		};

		/** \brief This is a virtual base class for objects that can have attributes. This includes tables, groups and HDF5 files.
		**/
		class CanHaveAttributes {
			bool valid() const;
		protected:
			CanHaveAttributes();
			virtual void _setAttributeParent(std::shared_ptr<H5::H5Object> obj) = 0;
			virtual std::shared_ptr<H5::H5Object> _getAttributeParent() const = 0;
		public:
			~CanHaveAttributes();
			/// Does the object have an attribute with the given name?
			bool doesAttributeExist(const std::string &attributeName) const;
			/// Does an object (parent) have an attribute with the given name?
			static bool doesAttributeExist(gsl::not_null<const H5::H5Object*> parent, const std::string &attributeName);
			/// \brief Returns the type of an attribute.
			/// \throws if the attribute does not exist.
			std::type_index getAttributeTypeId(const std::string &attributeName) const;
			/// \brief Returns the type of an attribute.
			/// \throws if the attribute does not exist.
			static std::type_index getAttributeTypeId(gsl::not_null<const H5::H5Object*> parent, const std::string &attributeName);
			/// \brief Is the type of the attribute "Type"?
			/// \throws if the attribute does not exist.
			template<class Type> bool isAttributeOfType(const std::string &attributeName) const {
				std::type_index atype = getAttributeTypeId(attributeName);
				if (atype == typeid(Type)) return true;
				return false;
			}

			/// \brief List all attributes attached to this object.
			std::set<std::string> getAttributeNames() const;
			/// \brief Delete an attribute, by name, that is attached to this object.
			/// \throws if the attribute does not exist.
			void deleteAttribute(const std::string &attributeName);

			/// \brief Function to read the data from an attribute.
			/// \throws if the attribute does not exist
			/// \throws if the parent is invalid
			/// \param DataType is the type of the data to be read
			/// \throws if there is a type mismatch between the actual data's type and
			///		the type expected in the function call.
			/// \param parent is the container of the attribute
			/// \param attributeName is the name of the attribute
			/// \param dimensions are the returned dimensions of the attribute.
			/// \param data are the returned attribute data. The mapping of dimensions and data follows the regular C convention (in contrast to the Fortran one).
			template <class DataType> static void readAttributeData(
				gsl::not_null<const H5::H5Object*> parent,
				const std::string &attributeName,
				std::vector<size_t> &dimensions,
				std::vector<DataType> &data);
			/// \brief Function to read the data from an attribute using the provided input vectors
			/// \throws if the attribute does not exist
			/// \param DataType is the type of the data to be read
			/// \throws if there is a type mismatch between the actual data's type and
			///		the type expected in the function call.
			/// \param attributeName is the name of the attribute
			/// \param dimensions are the returned dimensions of the attribute.
			/// \param data are the returned attribute data. The mapping of dimensions and data follows the regular C convention (in contrast to the Fortran one).
			template <class DataType> void readAttributeData(
				const std::string &attributeName,
				std::vector<size_t> &dimensions,
				std::vector<DataType> &data) const;
			/// \brief Function to write an attribute to an object, with the provided raw data
			/// \throws if the base object is read-only
			/// \param DataType is the type of the data to be read
			/// \param attributeName is the name of the attribute
			/// \param dimensions are the dimensions of the attribute.
			/// \param data are the attribute data. The mapping of dimensions and data follows the regular C convention (in contrast to the Fortran one).
			template <class DataType> void writeAttributeData(
				const std::string &attributeName,
				const std::vector<size_t> &dimensionas,
				const std::vector<DataType> &data);

			/// \brief Convenience function to read an attribute's data and return an Attribute object.
			/// \throws if the base object does not exist
			/// \throws if the attribute has a mismatched DataType
			/// \throws if the attribute does not exist
			/// \param obj is the containing object
			/// \param DataType is the type of the data
			/// \param attributeName is the name of the attribute
			template <class DataType> static Attribute<DataType> readAttribute(
					gsl::not_null<const H5::H5Object*> obj, const std::string &attributeName)
			{
				Attribute<DataType> res(attributeName);
				readAttributeData(obj, attributeName, res.dimensionality, res.data);
				return res;
			}
			/// \brief Convenience function to read an attribute's data and return an Attribute object.
			/// \throws if the attribute has a mismatched DataType
			/// \throws if the attribute does not exist
			/// \param DataType is the type of the data
			/// \param attributeName is the name of the attribute
			template<class DataType> Attribute<DataType> readAttribute(const std::string &attributeName) const {
				Attribute<DataType> res(attributeName);
				readAttributeData(attributeName, res.dimensionality, res.data);
				return res;
			}

			/// \brief Convenience function to write an Attribute to an object.
			/// \throws if the attribute is somehow invalid (usually from not giving it a proper name).
			/// \param DataType is the type of the data
			/// \param is an Attribute<DataType> object
			template<class DataType> void writeAttribute(const Attribute<DataType> &attribute) {
				size_t sz = 1;
				for (const auto &d : attribute.dimensionality) sz *= d;
				Expects(attribute.data.size() == sz);

				writeAttributeData(attribute.name, attribute.dimensionality, attribute.data);
			}
			/// \brief Convenience function to write a small Attribute to an object, using initializer lists.
			/// 
			/// Example: grpTest1->writeAttribute<uint64_t>("TestInt5", {1}, {65536});
			///
			/// \param name is the attribute's name
			/// \param dims are the dimensions
			/// \param data are the data
			/// \throws if the name is invalid
			template<class DataType> void writeAttribute(
				const std::string &name,
				std::initializer_list<size_t> dims,
				std::initializer_list<DataType> data) {
				Attribute<DataType> attr(name, dims, data);
				writeAttribute(attr);
			}


		};

	}
}
