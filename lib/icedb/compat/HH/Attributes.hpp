#pragma once
#include <tuple>
#include <typeinfo>
#include <typeindex>
#include <vector>
#include <ostream>
#include <iostream>
#include <gsl/gsl>
//#include <tl/Expected.hpp>
#include <hdf5.h>

#include "Handles.hpp"
#include "Tags.hpp"
#include "Types.hpp"

/* TODOS:
- add, write and read attributes from Eigen objects
*/

namespace HH {
	/// \todo Switch to explicit namespace specification.
	using namespace HH::Handles;
	using namespace HH::Types;
	using namespace gsl;
	using std::initializer_list;
	using std::tuple;

	struct Attribute
	{
	private:
		/// The attribute container manages its own view of the attribute.
		/// It takes ownership.
		/// Copying should be prohibited!!!!!
		/// Must release to transfer the handle!!!!!
		HH_hid_t attr;
	public:
		Attribute(HH_hid_t hnd_attr) : attr(hnd_attr) {  }
		virtual ~Attribute() {}

		HH_hid_t get() const { return attr; }

		static bool isAttribute(HH_hid_t obj) {
			H5I_type_t typ = H5Iget_type(obj());
			if (typ == H5I_ATTR) return true;
			return false;
		}
		bool isAttribute() const { return isAttribute(attr); }

		/// \brief Write data to an attribute
		/// \note Writing attributes is an all-or-nothing process.
		template <class DataType, class Marshaller = HH::Types::Object_Accessor<DataType> >
		HH_NODISCARD herr_t write(
			::gsl::span<const DataType> data,
			HH_hid_t in_memory_dataType = HH::Types::GetHDF5Type<DataType>())
		{
			Expects(isAttribute());
			Marshaller m;
			auto d = m.serialize(data);
			return H5Awrite(attr(), in_memory_dataType(), d);
			//return 0;
			//return H5Awrite(attr(), in_memory_dataType(), data.data());
		}
		template <class DataType> //, class Marshaller = HH::Types::Object_Accessor<DataType> >
		HH_NODISCARD herr_t write(
			DataType data,
			HH_hid_t in_memory_dataType = HH::Types::GetHDF5Type<DataType>())
		{
			/// \note Compiler bug: if write( is used instead of write<DataType>, the compiler enters into an infinite allocation loop.
			return write<DataType>(gsl::make_span(&data, 1), in_memory_dataType);
		}

		/// \todo Change reads to verify the data type being read.
		/// Add a force attribute to skip this check.

	private:
		template //<class MarshaledType, 
			<class DataType>
			herr_t _read(HH_hid_t in_memory_dataType, size_t flsize, gsl::span<DataType> data) const {
			//MarshaledType m(flsize);
			herr_t ret = H5Aread(attr(), in_memory_dataType(), data.data());
			return ret;
		}
	public:
		/// \brief Read data from an attribute
		/// \note Reading attributes is an all-or-nothing process.
		template <class DataType>
		HH_NODISCARD herr_t read(
			span<DataType> data,
			HH_hid_t in_memory_dataType = HH::Types::GetHDF5Type<DataType>()) const
		{
			// For basic data-types, size in bytes is enough.
			// For VLA / struct / string / compund data-types, I need to get the 
			// size of the contained dataspace and datatype.
			//auto space = getSpace();
			auto ftype = getType();
			H5T_class_t type_class = H5Tget_class(ftype());
			// Detect if this is a variable-length array string type:
			bool isVLenArrayType = false;
			if (type_class == H5T_STRING) {
				htri_t i = H5Tis_variable_str(ftype());
				if (i > 0) isVLenArrayType = true;
			}
			auto flsize = H5Tget_size(ftype()); // Separate meaning if vlan type
												// Separate marshalling treatment based on type
			if (isVLenArrayType) return _read //<HH::Types::Object_Accessor<DataType*>, 
				<DataType>
				(in_memory_dataType, flsize, data);
			else return _read<DataType> //<HH::Types::Object_Accessor<DataType>>
				(in_memory_dataType, flsize, data);
		}

		/// \brief Vector read convenience function
		/// \note Assuming that there will never be an array typr of variable-length strings, or other oddities.
		template <class DataType>
		HH_NODISCARD herr_t read(
			std::vector<DataType> &data,
			HH_hid_t in_memory_dataType = HH::Types::GetHDF5Type<DataType>()) const
		{
			auto space = getSpace();
			auto ftype = getType();
			H5T_class_t type_class = H5Tget_class(ftype());
			//HH_hid_t type_class(H5Tget_class(ftype()), Closers::CloseHDF5Datatype::CloseP);
			// Detect if this is a variable-length array string type:
			bool isVLenArrayType = false;
			if (type_class == H5T_STRING) {
				htri_t i = H5Tis_variable_str(ftype());
				if (i > 0) isVLenArrayType = true;
			}
			auto flsize = H5Tget_size(ftype()); // Separate meaning if vlan type

												// Currently, all dataspaces are simple. May change in the future.
			Expects(H5Sis_simple(space()) > 0);
			hssize_t numPoints = H5Sget_simple_extent_npoints(space());
			if (isVLenArrayType) numPoints *= flsize;

			data.resize(gsl::narrow_cast<size_t>(numPoints));
			return read(gsl::make_span(data.data(), data.size()), in_memory_dataType);
		}

		/// Read into a single value (convenience function)
		template <class DataType>
		HH_NODISCARD herr_t read(
			DataType &data,
			HH_hid_t in_memory_dataType = HH::Types::GetHDF5Type<DataType>()) const
		{
			return read<DataType>(gsl::make_span(&data, 1), in_memory_dataType);
		}

		/// Read into a single value (convenience function)
		template <class DataType>
		HH_NODISCARD DataType read(HH_hid_t in_memory_dataType = HH::Types::GetHDF5Type<DataType>()) const
		{
			DataType res;
			herr_t err = read<DataType>(res, in_memory_dataType);
			Expects((err >= 0) && "Attribute read error");
			return res;
		}

		/// \brief Get an attribute's name
		HH_NODISCARD ssize_t get_name(size_t buf_size, char* buf) const
		{
			Expects(isAttribute());
			return H5Aget_name(attr(), buf_size, buf);
		}
		/// \brief Get an attribute's name.
		/// \returns the name of the attribute, as either an ASCII or UTF-8 string.
		/// \note To determine the character encoding, see get_char_encoding().
		std::string get_name() const {
			ssize_t sz = get_name(0, NULL); // Number of _characters_ needed to store the name.
			std::vector<char> res(gsl::narrow_cast<size_t>(sz), '\0');
			sz = get_name(sz, res.data());
			// Null-terminated always
			return std::string(res.data());
		}
		enum class att_name_encoding { ASCII, UTF8 };
		att_name_encoding get_char_encoding() const {
			Expects(isAttribute());
			// See https://support.hdfgroup.org/HDF5/doc/Advanced/UsingUnicode/index.html
			// HDF5 encodes in only either ASCII or UTF-8.
			HH_hid_t pl(H5Aget_create_plist(attr()), Closers::CloseHDF5PropertyList::CloseP);
			H5T_cset_t encoding;
			herr_t encerr = H5Pget_char_encoding(pl(), &encoding);
			Expects(encerr >= 0);
			// encoding is either H5T_CSET_ASCII or H5T_CSET_UTF8.
			if (encoding == H5T_CSET_ASCII) return att_name_encoding::ASCII;
			else if (encoding == H5T_CSET_UTF8) att_name_encoding::UTF8;
			else throw;
		}

		/// @}

		/// @name Type-querying Functions
		/// @{
		/// Get attribute type, as an HDF5 type object.
		/// \see Types.hpp for the functions to compare the HDF5 type with a system type.
		HH_hid_t getType() const
		{
			Expects(isAttribute());
			return HH_hid_t(H5Aget_type(attr()), Closers::CloseHDF5Datatype::CloseP);
		}
		inline HH_hid_t type() const { return getType(); }

		/// Convenience function to check an attribute's type. 
		/// \returns True if the type matches
		/// \returns False (0) if the type does not match
		/// \returns <0 if an error occurred.
		template <class DataType>
		htri_t isOfType() {
			auto ttype = HH::Types::GetHDF5Type<DataType>();
			HH_hid_t otype = getType();
			return H5Tequal(ttype(), otype());
		}

		/// Get an attribute's dataspace
		HH_hid_t getSpace() const
		{
			Expects(isAttribute());
			return HH_hid_t(H5Aget_space(attr()), Closers::CloseHDF5Dataspace::CloseP);
		}
		inline HH_hid_t space() const { return getSpace(); }

		/// Get the amount of storage space used INSIDE HDF5 for an attribute
		hsize_t getStorageSize() const
		{
			Expects(isAttribute());
			return H5Aget_storage_size(attr());
		}

		/// Get attribute's dimensions
		/// \todo Replace with a structure
		std::tuple<
			Tags::ObjSizes::t_dimensions_current,
			Tags::ObjSizes::t_dimensions_max,
			Tags::ObjSizes::t_dimensionality,
			Tags::ObjSizes::t_numpoints>
			//std::tuple<std::vector<hsize_t>, int, hssize_t>
			getDimensions()
		{
			Expects(isAttribute());
			std::vector<hsize_t> dims;
			auto space = getSpace();
			Expects(H5Sis_simple(space()) > 0);
			hssize_t numPoints = H5Sget_simple_extent_npoints(space());
			int dimensionality = H5Sget_simple_extent_ndims(space());
			Expects(dimensionality >= 0);
			dims.resize(dimensionality);
			int err = H5Sget_simple_extent_dims(space(), dims.data(), nullptr);
			Expects(err >= 0);

			return std::make_tuple(
				Tags::ObjSizes::t_dimensions_current(dims),
				Tags::ObjSizes::t_dimensions_max(dims),
				Tags::ObjSizes::t_dimensionality(dimensionality),
				Tags::ObjSizes::t_numpoints(numPoints));
			//dims, dimensionality, numPoints);
		}

		/// @}

		void describe(std::ostream &out = std::cout) {
			auto d = getDimensions();
			using namespace Tags::ObjSizes;
			using namespace std;
			cout << "Attribute has:\n\tnPoints:\t" << ::std::get<t_numpoints>(d).data
				<< "\n\tDimensionality:\t" << ::std::get<t_dimensionality>(d).data
				<< "\n\tDimensions:\t[\t";
			for (const auto &m : ::std::get<t_dimensions_current>(d).data)
				cout << m << "\t";
			cout << "]" << endl;
		}
	};

	struct Has_Attributes
	{
	private:
		HH_hid_t base;
	public:
		Has_Attributes(HH_hid_t obj) : base(obj) {}
		virtual ~Has_Attributes() {}

		/// @name General Functions
		/// @{
		/// Does an attribute with the specified name exist?
		htri_t exists(not_null<const char*> attname) const
		{
			return H5Aexists(base(), attname.get());
		}
		/// Delete an attribute with the specified name.
		/// \note The base HDF5 function is H5Adelete, but delete is a reserved name in C++.
		/// \returns false on error, true on success.
		HH_NODISCARD bool remove(not_null<const char*> attname)
		{
			herr_t err = H5Adelete(base(), attname.get());
			if (err >= 0) return true;
			return false;
		}


		/// \todo open, create and add should also give a scoped handle return object as an option.
		/// \\brief Open an attribute
		Attribute open(
			not_null<const char*> name,
			HH_hid_t AttributeAccessPlist = H5P_DEFAULT) const
		{
			/// \todo Check for failure
			return Attribute(HH_hid_t(
				H5Aopen(base(), name.get(), AttributeAccessPlist()),
				Closers::CloseHDF5Attribute::CloseP
			));
		}

		Attribute operator[](not_null<const char*> name) const { return open(name); }

		/// \brief Create an attribute, without setting its data.
		template <class DataType>
		HH_NODISCARD Attribute create(
			not_null<const char*> attrname,
			initializer_list<size_t> dimensions = { 1 },
			HH_hid_t dtype = HH::Types::GetHDF5Type<DataType>(),
			HH_hid_t AttributeCreationPlist = H5P_DEFAULT,
			HH_hid_t AttributeAccessPlist = H5P_DEFAULT)
		{
			//HH_hid_t dtypeb = HH::Types::GetHDF5Type<DataType>(); // For debugging...
			std::vector<hsize_t> hdims;
			for (const auto &d : dimensions)
				hdims.push_back(gsl::narrow_cast<hsize_t>(d));
			HH_hid_t dspace{
				H5Screate_simple(
					gsl::narrow_cast<int>(dimensions.size()),
					hdims.data(),
					nullptr),
				Closers::CloseHDF5Dataspace::CloseP };

			auto attI = HH_hid_t(
				H5Acreate(
					base(),
					attrname.get(),
					dtype(),
					dspace(),
					AttributeCreationPlist(),
					AttributeAccessPlist()),
				Closers::CloseHDF5Attribute::CloseP
			);
			Expects(H5Iis_valid(attI()));
			return Attribute(attI);
		}

		/// \brief Rename an attribute
		/// \note This can be in UTF-8... must match the attribute's creation property list.
		HH_NODISCARD herr_t rename(not_null<const char*> oldName, not_null<const char*> newName) const
		{
			return H5Arename(base(), oldName.get(), newName.get());
		}


		/// @name Convenience functions
		/// @{

		/// Create and write an attribute, for arbitrary dimensions.
		template <class DataType>
		Attribute add(
			::gsl::not_null<const char*> attrname,
			::gsl::span<const DataType> data,
			::std::initializer_list<size_t> dimensions,
			HH_hid_t in_memory_dataType = HH::Types::GetHDF5Type<DataType>(),
			HH_hid_t AttributeCreationPlist = H5P_DEFAULT,
			HH_hid_t AttributeAccessPlist = H5P_DEFAULT
		)
		{
			auto newAttr = create<DataType>(
				attrname.get(),
				dimensions,
				in_memory_dataType,
				AttributeCreationPlist, AttributeAccessPlist);
			/// \todo Already checked validity. Check again.
			//Expects(newAttr.get());
			herr_t res = newAttr.template write<DataType>(data, in_memory_dataType);
			Expects(res >= 0);
			return Attribute(std::move(newAttr));
		}

		template <class DataType>
		Attribute add(
			::gsl::not_null<const char*> attrname,
			::std::initializer_list<const DataType> data,
			::std::initializer_list<size_t> dimensions,
			HH_hid_t in_memory_dataType = HH::Types::GetHDF5Type<DataType>(),
			HH_hid_t AttributeCreationPlist = H5P_DEFAULT,
			HH_hid_t AttributeAccessPlist = H5P_DEFAULT)
		{
			//HH_hid_t in_memory_dataType_debug = HH::Types::GetHDF5Type<DataType>(); // debugging
			auto newAttr = create<DataType>(
				attrname.get(),
				dimensions,
				in_memory_dataType,
				AttributeCreationPlist, AttributeAccessPlist);
			/// \todo Attribute creation check handled already. Check again.
			herr_t res = newAttr.template write<DataType>(
				::gsl::make_span(data.begin(), data.size()),
				in_memory_dataType);
			Expects(res >= 0);
			return Attribute(std::move(newAttr));
		}

		template <class DataType>
		Attribute add(
			::gsl::not_null<const char*> attrname,
			::gsl::span<const DataType> data,
			HH_hid_t in_memory_dataType = HH::Types::GetHDF5Type<DataType>(),
			HH_hid_t AttributeCreationPlist = H5P_DEFAULT,
			HH_hid_t AttributeAccessPlist = H5P_DEFAULT)
		{
			//HH_hid_t in_memory_dataType_check = HH::Types::GetHDF5Type<DataType>(); // debugging
			return add<DataType>(attrname, data, { gsl::narrow_cast<size_t>(data.size()) },
				in_memory_dataType, AttributeCreationPlist, AttributeAccessPlist);
		}

		template <class DataType>
		Attribute add(
			::gsl::not_null<const char*> attrname,
			::std::initializer_list<const DataType> data,
			HH_hid_t in_memory_dataType = HH::Types::GetHDF5Type<DataType>(),
			HH_hid_t AttributeCreationPlist = H5P_DEFAULT,
			HH_hid_t AttributeAccessPlist = H5P_DEFAULT)
		{
			return add<DataType>(attrname, data, { gsl::narrow_cast<size_t>(data.size()) },
				in_memory_dataType, AttributeCreationPlist, AttributeAccessPlist);
		}

		template <class DataType>
		Attribute add(
			::gsl::not_null<const char*> attrname,
			DataType data,
			HH_hid_t in_memory_dataType = HH::Types::GetHDF5Type<DataType>(),
			HH_hid_t AttributeCreationPlist = H5P_DEFAULT,
			HH_hid_t AttributeAccessPlist = H5P_DEFAULT)
		{
			auto newAttr = create<DataType>(attrname.get(), { 1 },
				in_memory_dataType,
				AttributeCreationPlist, AttributeAccessPlist);
			/// \todo Check for failure
			//Expects(newAttr.);
			herr_t res = newAttr.template write<DataType>(data, in_memory_dataType);
			Expects(res >= 0);
			return Attribute(newAttr);
		}


		/// \todo Switch to attribute objects. Keep raw object as an option.
		/// Open and read an attribute, for expected dimensions.
		template <class DataType>
		herr_t read(
			not_null<const char*> attrname,
			span<DataType> data,
			HH_hid_t in_memory_dataType = GetHDF5Type<DataType>(),
			HH_hid_t AttributeAccessPlist = H5P_DEFAULT)
		{
			Attribute attr = open(attrname, AttributeAccessPlist);
			return attr.read(data, in_memory_dataType);
		}

		/// Open and read an attribute, with unknown dimensions
		template <class DataType>
		herr_t read(
			not_null<const char*> attrname,
			std::vector<DataType> &data,
			HH_hid_t in_memory_dataType = GetHDF5Type<DataType>(),
			HH_hid_t AttributeAccessPlist = H5P_DEFAULT)
		{
			data.clear();
			Attribute attr = open(attrname, AttributeAccessPlist);
			auto d = attr.getDimensions();
			using namespace Tags::ObjSizes;
			using namespace std;
			t_numpoints::value_type nPoints = get<t_numpoints>(d).data;
			data.resize(gsl::narrow_cast<size_t>(nPoints));
			return attr.read(gsl::make_span(data.data(), data.size()), in_memory_dataType);
			//static_assert(false, "FINISH THIS");
			//return read(base, attrname, make_span(data.data(), data.size()));
		}

		/// Read an attribute
		template <class DataType>
		HH_NODISCARD DataType read(
			not_null<const char*> attrname,
			HH_hid_t in_memory_dataType = HH::Types::GetHDF5Type<DataType>(),
			HH_hid_t AttributeAccessPlist = H5P_DEFAULT)
		{
			DataType res;
			Attribute attr = open(attrname, AttributeAccessPlist);
			herr_t err = attr.read<DataType>(res, in_memory_dataType);
			Expects((err >= 0) && "Attribute read error");
			return res;
		}


		/// @}
	};

}
