#pragma once
#include "defs.hpp"
#include <tuple>
#include <typeinfo>
#include <typeindex>
#include <vector>
#include <ostream>
#include <iostream>
#include <hdf5.h>

#include "gsl/gsl"
#include "Handles.hpp"
#include "Tags.hpp"
#include "Types.hpp"
#include "Errors.hpp"

namespace HH {
	/// \todo Switch to explicit namespace specification.
	using namespace HH::Handles;
	using namespace HH::Types;
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
			if (typ == H5I_BADID) throw HH_throw;
			if (typ == H5I_ATTR) return true;
			return false;
		}
		bool isAttribute() const { return isAttribute(attr); }

		/// \brief Write data to an attribute
		/// \note Writing attributes is an all-or-nothing process.
		template <class DataType, class Marshaller = HH::Types::Object_Accessor<DataType> >
		Attribute write(
			gsl::span<const DataType> data,
			HH_hid_t in_memory_dataType = HH::Types::GetHDF5Type<DataType>())
		{
			HH_Expects(isAttribute());
			Marshaller m;
			auto d = m.serialize(data);
			if (H5Awrite(attr(), in_memory_dataType(), d) < 0) throw HH_throw.add("Reason", "H5Awrite failed.");
			return *this;
		}
		template <class DataType> //, class Marshaller = HH::Types::Object_Accessor<DataType> >
		Attribute write(
			DataType data,
			HH_hid_t in_memory_dataType = HH::Types::GetHDF5Type<DataType>())
		{
			/// \note Compiler bug: if write( is used instead of write<DataType>, the compiler enters into an infinite allocation loop.
			return write<DataType>(gsl::make_span(&data, 1), in_memory_dataType);
		}

		Attribute writeFixedLengthString(
			const std::string & data)
		{
			HH_Expects(isAttribute());
			HH_hid_t dtype = HH::Types::GetHDF5TypeFixedString(data.size());
			if (H5Awrite(attr(), dtype(), data.data()) < 0) throw HH_throw.add("Reason", "H5Awrite failed.");
			return *this;
		}
	private:
		template //<class MarshaledType, 
			<class DataType>
			Attribute _read(HH_hid_t in_memory_dataType, size_t flsize, gsl::span<DataType> data) const {
			//MarshaledType m(flsize);
			herr_t ret = H5Aread(attr(), in_memory_dataType(), data.data());
			if (ret < 0) throw HH_throw.add("Reason", "H5Aread failed.");
			return *this;
		}
	public:
		/// \brief Read data from an attribute
		/// \note Reading attributes is an all-or-nothing process.
		template <class DataType>
		Attribute read(
			gsl::span<DataType> data,
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
		Attribute read(
			std::vector<DataType> & data,
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
			HH_Expects(H5Sis_simple(space()) > 0);
			hssize_t numPoints = H5Sget_simple_extent_npoints(space());
			if (isVLenArrayType) numPoints *= flsize;

			data.resize(gsl::narrow_cast<size_t>(numPoints));
			return read(gsl::make_span(data.data(), data.size()), in_memory_dataType);
		}

		/// Read into a single value (convenience function)
		template <class DataType>
		Attribute read(
			DataType & data,
			HH_hid_t in_memory_dataType = HH::Types::GetHDF5Type<DataType>()) const
		{
			return read<DataType>(gsl::make_span(&data, 1), in_memory_dataType);
		}

		/// Read into a single value (convenience function)
		template <class DataType>
		HH_NODISCARD DataType read(HH_hid_t in_memory_dataType = HH::Types::GetHDF5Type<DataType>()) const
		{
			DataType res;
			read<DataType>(res, in_memory_dataType);
			return res;
		}

		/// \brief Get an attribute's name
		HH_NODISCARD ssize_t get_name(size_t buf_size, char* buf) const
		{
			HH_Expects(isAttribute());
			return H5Aget_name(attr(), buf_size, buf);
		}
		/// \brief Get an attribute's name.
		/// \returns the name of the attribute, as either an ASCII or UTF-8 string.
		/// \note To determine the character encoding, see get_char_encoding().
		std::string get_name() const {
			ssize_t sz = get_name(0, NULL) + 1; // Number of _characters_ needed to store the name.
			std::vector<char> res(gsl::narrow_cast<size_t>(sz), '\0');
			sz = get_name(sz, res.data());
			// Null-terminated always
			return std::string(res.data());
		}
		enum class att_name_encoding { ASCII, UTF8 };
		att_name_encoding get_char_encoding() const {
			HH_Expects(isAttribute());
			// See https://support.hdfgroup.org/HDF5/doc/Advanced/UsingUnicode/index.html
			// HDF5 encodes in only either ASCII or UTF-8.
			HH_hid_t pl(H5Aget_create_plist(attr()), Closers::CloseHDF5PropertyList::CloseP);
			H5T_cset_t encoding;
			herr_t encerr = H5Pget_char_encoding(pl(), &encoding);
			HH_Expects(encerr >= 0);
			// encoding is either H5T_CSET_ASCII or H5T_CSET_UTF8.
			if (encoding == H5T_CSET_ASCII) return att_name_encoding::ASCII;
			else if (encoding == H5T_CSET_UTF8) return att_name_encoding::UTF8;
			else throw;
			return att_name_encoding::ASCII; // Suppress Clang warning
		}

		/// @}

		/// @name Type-querying Functions
		/// @{
		/// Get attribute type, as an HDF5 type object.
		/// \see Types.hpp for the functions to compare the HDF5 type with a system type.
		HH_hid_t getType() const
		{
			HH_Expects(isAttribute());
			return HH_hid_t(H5Aget_type(attr()), Closers::CloseHDF5Datatype::CloseP);
		}
		inline HH_hid_t type() const { return getType(); }

		/// Convenience function to check an attribute's type. 
		/// \returns True if the type matches
		/// \returns False (0) if the type does not match
		/// \returns <0 if an error occurred.
		template <class DataType>
		bool isOfType() const {
			auto ttype = HH::Types::GetHDF5Type<DataType>();
			HH_hid_t otype = getType();
			auto ret = H5Tequal(ttype(), otype());
			if (ret < 0) throw HH_throw;
			return (ret > 0) ? true : false;
		}

		/// Get an attribute's dataspace
		HH_hid_t getSpace() const
		{
			HH_Expects(isAttribute());
			return HH_hid_t(H5Aget_space(attr()), Closers::CloseHDF5Dataspace::CloseP);
		}
		inline HH_hid_t space() const { return getSpace(); }

		/// Get the amount of storage space used INSIDE HDF5 for an attribute
		hsize_t getStorageSize() const
		{
			HH_Expects(isAttribute());
			return H5Aget_storage_size(attr());
		}

		/// Get attribute's dimensions
		struct Dimensions {
			std::vector<hsize_t> dimsCur, dimsMax;
			hsize_t dimensionality;
			hsize_t numElements;
			Dimensions(const std::vector<hsize_t>& dimscur, const std::vector<hsize_t>& dimsmax, hsize_t dality, hsize_t np)
				: dimsCur(dimscur), dimsMax(dimsmax), dimensionality(dality), numElements(np) {}
		};
		Dimensions getDimensions() const
		{
			HH_Expects(isAttribute());
			std::vector<hsize_t> dims;
			auto space = getSpace();
			HH_Expects(H5Sis_simple(space()) > 0);
			hssize_t numPoints = H5Sget_simple_extent_npoints(space());
			int dimensionality = H5Sget_simple_extent_ndims(space());
			HH_Expects(dimensionality >= 0);
			dims.resize(dimensionality);
			int err = H5Sget_simple_extent_dims(space(), dims.data(), nullptr);
			HH_Expects(err >= 0);

			return Dimensions(dims, dims, (hsize_t)dimensionality, (hsize_t)numPoints);
		}

		/// @}

		void describe(std::ostream & out = std::cout) {
			auto d = getDimensions();
			using namespace Tags::ObjSizes;
			using namespace std;
			cout << "Attribute has:\n\tnPoints:\t" << d.numElements
				<< "\n\tDimensionality:\t" << d.dimensionality
				<< "\n\tDimensions:\t[\t";
			for (const auto& m : d.dimsCur)
				cout << m << "\t";
			cout << "]" << endl;
		}
	};

	struct Almost_Attribute_base
	{
	protected:
		std::string name;
	public:
		virtual ~Almost_Attribute_base() {}
		virtual Attribute apply(HH::HH_hid_t obj) const = 0;
		Almost_Attribute_base(
			const std::string& name)
			: name(name) {}
	};

	template <class DataType>
	struct Almost_Attribute : public Almost_Attribute_base
	{
	private:
		::std::vector<hsize_t> dimensions;
		HH::HH_hid_t in_memory_dataType = HH::Types::GetHDF5Type<DataType>();
		::std::vector<DataType> data;
		///::gsl::span<const DataType> data;
	public:
		virtual ~Almost_Attribute() {}
		HH_MAYBE_UNUSED virtual Attribute apply(HH::HH_hid_t obj) const override
		{

			return add(obj, name.c_str(), data, gsl::make_span(dimensions), in_memory_dataType);
		}

		Almost_Attribute(const std::string& name,
			::gsl::span<const DataType> data,
			::std::initializer_list<hsize_t> dimensions,
			HH_hid_t in_memory_dataType = HH::Types::GetHDF5Type<DataType>())
			: Almost_Attribute_base(name),
			data(data.begin(), data.end()), in_memory_dataType(in_memory_dataType), dimensions(dimensions) {}

		/// \brief Create an attribute, without setting its data.
		HH_NODISCARD static Attribute create(
			HH_hid_t base,
			const ::std::string& attrname,
			::gsl::span<const hsize_t> dimensions,
			HH_hid_t dtype = HH::Types::GetHDF5Type<DataType>(),
			HH_hid_t AttributeCreationPlist = H5P_DEFAULT,
			HH_hid_t AttributeAccessPlist = H5P_DEFAULT)
		{
			//HH_hid_t dtypeb = HH::Types::GetHDF5Type<DataType>(); // For debugging...
			std::vector<hsize_t> hdims;
			for (const auto& d : dimensions)
				hdims.push_back(gsl::narrow_cast<hsize_t>(d));
			HH_hid_t dspace{
				H5Screate_simple(
					gsl::narrow_cast<int>(hdims.size()),
					hdims.data(),
					nullptr),
				Closers::CloseHDF5Dataspace::CloseP };

			auto attI = HH_hid_t(
				H5Acreate(
					base(),
					attrname.c_str(),
					dtype(),
					dspace(),
					AttributeCreationPlist(),
					AttributeAccessPlist()),
				Closers::CloseHDF5Attribute::CloseP
			);
			if (H5Iis_valid(attI()) <= 0) throw HH_throw.add("Reason", "Attribute is not valid.");
			return Attribute(attI);
		}

		HH_NODISCARD static Attribute create(
			HH_hid_t base,
			const ::std::string & attrname,
			::std::initializer_list<hsize_t> dimensions = { 1 },
			HH_hid_t dtype = HH::Types::GetHDF5Type<DataType>(),
			HH_hid_t AttributeCreationPlist = H5P_DEFAULT,
			HH_hid_t AttributeAccessPlist = H5P_DEFAULT)
		{
			std::vector<hsize_t> vdims(dimensions);

			return create(base, attrname, gsl::make_span(vdims), dtype,
				AttributeCreationPlist, AttributeAccessPlist);
		}

		/// Create and write an attribute, for arbitrary dimensions.
		static Attribute add(
			HH_hid_t base,
			const ::std::string & attrname,
			::gsl::span<const DataType> data,
			::gsl::span<const hsize_t> dimensions,
			HH_hid_t in_memory_dataType = HH::Types::GetHDF5Type<DataType>(),
			HH_hid_t AttributeCreationPlist = H5P_DEFAULT,
			HH_hid_t AttributeAccessPlist = H5P_DEFAULT
		)
		{
			auto newAttr = create(base,
				attrname,
				dimensions,
				in_memory_dataType,
				AttributeCreationPlist, AttributeAccessPlist);
			/// \todo Already checked validity. Check again.
			//HH_Expects(newAttr.get());
			newAttr.template write<DataType>(data, in_memory_dataType);
			return Attribute(std::move(newAttr));
		}

		static Attribute add(
			HH_hid_t base,
			const ::std::string & attrname,
			::std::initializer_list<DataType> data,
			::gsl::span<const hsize_t> dimensions,
			HH_hid_t in_memory_dataType = HH::Types::GetHDF5Type<DataType>(),
			HH_hid_t AttributeCreationPlist = H5P_DEFAULT,
			HH_hid_t AttributeAccessPlist = H5P_DEFAULT)
		{
			//HH_hid_t in_memory_dataType_debug = HH::Types::GetHDF5Type<DataType>(); // debugging
			auto newAttr = create(base,
				attrname,
				dimensions,
				in_memory_dataType,
				AttributeCreationPlist, AttributeAccessPlist);
			/// \todo Attribute creation check handled already. Check again.
			newAttr.template write<DataType>(
				::gsl::make_span(data.begin(), data.size()),
				in_memory_dataType);
			return Attribute(std::move(newAttr));
		}

		static Attribute add(
			HH_hid_t base,
			const ::std::string & attrname,
			::gsl::span<const DataType> data,
			HH_hid_t in_memory_dataType = HH::Types::GetHDF5Type<DataType>(),
			HH_hid_t AttributeCreationPlist = H5P_DEFAULT,
			HH_hid_t AttributeAccessPlist = H5P_DEFAULT)
		{
			auto newAttr = create(base,
				attrname,
				{ gsl::narrow_cast<hsize_t>(data.size()) },
				in_memory_dataType,
				AttributeCreationPlist, AttributeAccessPlist);
			/// \todo Already checked validity. Check again.
			//HH_Expects(newAttr.get());
			newAttr.template write<DataType>(data, in_memory_dataType);
			return Attribute(std::move(newAttr));

			//return add(base, attrname, data, { gsl::narrow_cast<size_t>(data.size()) },
			//	in_memory_dataType, AttributeCreationPlist, AttributeAccessPlist);
		}

		static Attribute add(
			HH_hid_t base,
			const ::std::string & attrname,
			::std::vector<DataType> data,
			HH_hid_t in_memory_dataType = HH::Types::GetHDF5Type<DataType>(),
			HH_hid_t AttributeCreationPlist = H5P_DEFAULT,
			HH_hid_t AttributeAccessPlist = H5P_DEFAULT)
		{
			auto newAttr = create(base,
				attrname,
				{ gsl::narrow_cast<hsize_t>(data.size()) },
				in_memory_dataType,
				AttributeCreationPlist, AttributeAccessPlist);
			/// \todo Already checked validity. Check again.
			//HH_Expects(newAttr.get());
			newAttr.template write<DataType>(data, in_memory_dataType);
			return Attribute(std::move(newAttr));

			//return add(base, attrname, data,
			//	{ gsl::narrow_cast<size_t>(data.size()) },
			//	in_memory_dataType, AttributeCreationPlist, AttributeAccessPlist);
		}

		static Attribute add(
			HH_hid_t base,
			const ::std::string & attrname,
			DataType data,
			HH_hid_t in_memory_dataType = HH::Types::GetHDF5Type<DataType>(),
			HH_hid_t AttributeCreationPlist = H5P_DEFAULT,
			HH_hid_t AttributeAccessPlist = H5P_DEFAULT)
		{
			auto newAttr = create(base, attrname, { 1 },
				in_memory_dataType,
				AttributeCreationPlist, AttributeAccessPlist);
			newAttr.template write<DataType>(data, in_memory_dataType);
			return Attribute(newAttr);
		}

	};

	struct Almost_Attribute_Fixed_String : public Almost_Attribute_base
	{
	private:
		::std::string data;
	public:
		virtual ~Almost_Attribute_Fixed_String() {}
		HH_MAYBE_UNUSED virtual Attribute apply(HH::HH_hid_t obj) const override
		{
			return addFixedLengthString(obj, name.c_str(), data);
		}

		Almost_Attribute_Fixed_String(const std::string& name,
			const std::string& data) : Almost_Attribute_base(name), data(data) {}

		/// \brief Create a special fixed-size string attribute. Needed for a few 
		HH_NODISCARD static Attribute createFixedLengthString(
			HH_hid_t base,
			const ::std::string& attrname,
			hsize_t len,
			HH_hid_t AttributeCreationPlist = H5P_DEFAULT,
			HH_hid_t AttributeAccessPlist = H5P_DEFAULT)
		{
			HH_hid_t dtype = HH::Types::GetHDF5TypeFixedString(len);
			std::vector<hsize_t> hdims = { 1 };
			HH_hid_t dspace{
				H5Screate_simple(
					gsl::narrow_cast<int>(hdims.size()),
					hdims.data(),
					nullptr),
				Closers::CloseHDF5Dataspace::CloseP };

			auto attI = HH_hid_t(
				H5Acreate(
					base(),
					attrname.c_str(),
					dtype(),
					dspace(),
					AttributeCreationPlist(),
					AttributeAccessPlist()),
				Closers::CloseHDF5Attribute::CloseP
			);

			if(H5Iis_valid(attI()) <= 0) throw HH_throw.add("Reason", "Attribute invalid.");
			return Attribute(attI);
		}

		/// Create and write a fixed-length string attribute.
		static Attribute addFixedLengthString(
			HH_hid_t base,
			const ::std::string & attrname,
			const ::std::string &data,
			HH_hid_t AttributeCreationPlist = H5P_DEFAULT,
			HH_hid_t AttributeAccessPlist = H5P_DEFAULT
		)
		{
			auto newAttr = createFixedLengthString(base,
				attrname,
				data.size(),
				AttributeCreationPlist, AttributeAccessPlist);
			newAttr.writeFixedLengthString(data);
			return Attribute(std::move(newAttr));
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
		bool exists(const std::string& attname) const
		{
			auto ret = H5Aexists(base(), attname.c_str());
			if (ret < 0) throw HH_throw;
			return (ret > 0) ? true : false;
		}
		/// Delete an attribute with the specified name.
		/// \note The base HDF5 function is H5Adelete, but delete is a reserved name in C++.
		/// \returns false on error, true on success.
		void remove(const std::string& attname)
		{
			herr_t err = H5Adelete(base(), attname.c_str());
			if (err < 0) throw HH_throw;
		}

		/// \todo open, create and add should also give a scoped handle return object as an option.
		/// \\brief Open an attribute
		Attribute open(
			const std::string & name,
			HH_hid_t AttributeAccessPlist = H5P_DEFAULT) const
		{
			auto ret = H5Aopen(base(), name.c_str(), AttributeAccessPlist());
			if (ret < 0) throw HH_throw;
			return Attribute(HH_hid_t(
				ret,
				Closers::CloseHDF5Attribute::CloseP
			));
		}

		Attribute operator[](const std::string & name) const { return open(name); }

		/// \brief Create an attribute, without setting its data.
		template <class DataType>
		Attribute create(
			const std::string & attrname,
			const std::vector<hsize_t> & dimensions = { 1 },
			HH_hid_t dtype = HH::Types::GetHDF5Type<DataType>(),
			HH_hid_t AttributeCreationPlist = H5P_DEFAULT,
			HH_hid_t AttributeAccessPlist = H5P_DEFAULT)
		{
			return Almost_Attribute<DataType>::create(base, attrname,
				dimensions, dtype, AttributeCreationPlist, AttributeAccessPlist);
		}

		/// Create a fixed-length string attribute
		Attribute createFixedLengthString(
			const std::string & attrname,
			const std::string & data,
			HH_hid_t AttributeCreationPlist = H5P_DEFAULT,
			HH_hid_t AttributeAccessPlist = H5P_DEFAULT)
		{
			return Almost_Attribute_Fixed_String::createFixedLengthString(base, attrname, data.size());
		}


		/// \brief Rename an attribute
		/// \note This can be in UTF-8... must match the attribute's creation property list.
		void rename(const std::string & oldName, const std::string & newName) const
		{
			auto ret = H5Arename(base(), oldName.c_str(), newName.c_str());
			if (ret < 0) throw HH_throw;
		}


		/// @name Convenience functions
		/// @{

		/// Create and write a fixed-length string attribute.
		template <class DataType>
		HH_MAYBE_UNUSED Has_Attributes addFixedLengthString(
			const std::string & attrname,
			const std::string & data,
			HH_hid_t AttributeCreationPlist = H5P_DEFAULT,
			HH_hid_t AttributeAccessPlist = H5P_DEFAULT
		)
		{
			Almost_Attribute_Fixed_String::addFixedLengthString(base(), attrname,
				data, AttributeCreationPlist, AttributeAccessPlist);
			return *this;
		}

		/// Create and write an attribute, for arbitrary dimensions.
		template <class DataType>
		HH_MAYBE_UNUSED Has_Attributes add(
			const std::string & attrname,
			::gsl::span<const DataType> data,
			::std::initializer_list<hsize_t> dimensions,
			HH_hid_t in_memory_dataType = HH::Types::GetHDF5Type<DataType>(),
			HH_hid_t AttributeCreationPlist = H5P_DEFAULT,
			HH_hid_t AttributeAccessPlist = H5P_DEFAULT
		)
		{
			Almost_Attribute<DataType>::add(base(), attrname,
				data, dimensions, in_memory_dataType, AttributeCreationPlist, AttributeAccessPlist);
			return *this;
		}

		template <class DataType>
		HH_MAYBE_UNUSED Has_Attributes add(
			const std::string & attrname,
			::std::initializer_list<DataType> data,
			::std::initializer_list<hsize_t> dimensions,
			HH_hid_t in_memory_dataType = HH::Types::GetHDF5Type<DataType>(),
			HH_hid_t AttributeCreationPlist = H5P_DEFAULT,
			HH_hid_t AttributeAccessPlist = H5P_DEFAULT)
		{
			gsl::span<const hsize_t> sdims(dimensions.begin(), dimensions.end());
			Almost_Attribute<DataType>::add(base(), attrname,
				data, sdims, in_memory_dataType, AttributeCreationPlist, AttributeAccessPlist);
			return *this;
		}

		template <class DataType>
		HH_MAYBE_UNUSED Has_Attributes add(
			const std::string & attrname,
			::gsl::span<const DataType> data,
			HH_hid_t in_memory_dataType = HH::Types::GetHDF5Type<DataType>(),
			HH_hid_t AttributeCreationPlist = H5P_DEFAULT,
			HH_hid_t AttributeAccessPlist = H5P_DEFAULT)
		{
			Almost_Attribute<DataType>::add(base(), attrname,
				data, in_memory_dataType, AttributeCreationPlist, AttributeAccessPlist);
			return *this;
		}

		template <class DataType>
		HH_MAYBE_UNUSED Has_Attributes add(
			const std::string & attrname,
			::std::initializer_list<DataType> data,
			HH_hid_t in_memory_dataType = HH::Types::GetHDF5Type<DataType>(),
			HH_hid_t AttributeCreationPlist = H5P_DEFAULT,
			HH_hid_t AttributeAccessPlist = H5P_DEFAULT)
		{
			Almost_Attribute<DataType>::add(base(), attrname,
				data, in_memory_dataType, AttributeCreationPlist, AttributeAccessPlist);
			return *this;
		}

		template <class DataType>
		HH_MAYBE_UNUSED Has_Attributes add(
			const std::string & attrname,
			DataType data,
			HH_hid_t in_memory_dataType = HH::Types::GetHDF5Type<DataType>(),
			HH_hid_t AttributeCreationPlist = H5P_DEFAULT,
			HH_hid_t AttributeAccessPlist = H5P_DEFAULT)
		{
			Almost_Attribute<DataType>::add(base(), attrname,
				data, in_memory_dataType, AttributeCreationPlist, AttributeAccessPlist);
			return *this;
		}


		/// \todo Switch to attribute objects. Keep raw object as an option.
		/// Open and read an attribute, for expected dimensions.
		template <class DataType>
		HH_MAYBE_UNUSED Has_Attributes read(
			const std::string & attrname,
			gsl::span<DataType> data,
			HH_hid_t in_memory_dataType = GetHDF5Type<DataType>(),
			HH_hid_t AttributeAccessPlist = H5P_DEFAULT) const
		{
			Attribute attr = open(attrname, AttributeAccessPlist);
			attr.read(data, in_memory_dataType);
			return *this;
		}

		/// Open and read an attribute, with unknown dimensions
		template <class DataType>
		HH_MAYBE_UNUSED Has_Attributes read(
			const std::string & attrname,
			std::vector<DataType> & data,
			HH_hid_t in_memory_dataType = GetHDF5Type<DataType>(),
			HH_hid_t AttributeAccessPlist = H5P_DEFAULT) const
		{
			data.clear();
			Attribute attr = open(attrname, AttributeAccessPlist);
			auto d = attr.getDimensions();
			using namespace Tags::ObjSizes;
			using namespace std;
			hsize_t nPoints = d.numElements;
			data.resize(gsl::narrow_cast<size_t>(nPoints));
			attr.read(gsl::make_span(data.data(), data.size()), in_memory_dataType);
			return *this;
		}

		/// Read an attribute
		template <class DataType>
		HH_NODISCARD DataType read(
			const std::string & attrname,
			HH_hid_t in_memory_dataType = HH::Types::GetHDF5Type<DataType>(),
			HH_hid_t AttributeAccessPlist = H5P_DEFAULT) const
		{
			DataType res;
			Attribute attr = open(attrname, AttributeAccessPlist);
			attr.read<DataType>(res, in_memory_dataType);
			return res;
		}


		/// @}
	};

	struct AttributeParameterPack
	{
	private:
		std::vector<std::shared_ptr<Almost_Attribute_base>> newAtts;
	public:
		template <typename NameType, typename AttType>
		AttributeParameterPack& AddSimpleAttributes(
			NameType attname, AttType val)
		{
			newAtts.push_back(std::shared_ptr<Almost_Attribute<AttType>>(
				new Almost_Attribute<AttType>(attname, { val }, { 1 })));
			return *this;
		}
		template <typename NameType, typename AttType, typename... Rest>
		AttributeParameterPack& AddSimpleAttributes(NameType attname, AttType val, Rest... rest)
		{
			AddSimpleAttributes<NameType, AttType>(attname, val);
			AddSimpleAttributes<Rest...>(rest...);
			return *this;
		}

		void apply(HH::HH_hid_t& d) const {
			for (auto& natt : newAtts)
				natt->apply(d);
		}

		/// Create and write a fixed-length string attribute.
		AttributeParameterPack& addFixedLengthString(
			const std::string & attrname,
			const std::string & data
		)
		{
			newAtts.push_back(std::shared_ptr<Almost_Attribute_Fixed_String>(
				new Almost_Attribute_Fixed_String(attrname, data)));
			return *this;
		}


		/// Create and write an attribute, for arbitrary dimensions.
		template <class DataType>
		AttributeParameterPack& add(
			const std::string & attrname,
			::gsl::span<const DataType> data,
			::std::initializer_list<hsize_t> dimensions
		)
		{
			newAtts.push_back(std::shared_ptr<Almost_Attribute<DataType>>(
				new Almost_Attribute<DataType>(attrname, data, dimensions)));
			return *this;
		}

		template <class DataType>
		AttributeParameterPack& add(
			const std::string & attrname,
			::std::initializer_list<const DataType> data,
			::std::initializer_list<hsize_t> dimensions)
		{
			std::vector<DataType> vd(data.begin(), data.end());
			newAtts.push_back(std::shared_ptr<Almost_Attribute<DataType>>(
				new Almost_Attribute<DataType>(attrname, gsl::make_span(vd), dimensions)));
			return *this;
		}

		template <class DataType>
		AttributeParameterPack& add(
			const std::string & attrname,
			::gsl::span<const DataType> data)
		{
			newAtts.push_back(std::shared_ptr<Almost_Attribute<DataType>>(
				new Almost_Attribute<DataType>(attrname, data, { data.size() })));
			return *this;
		}

		template <class DataType>
		AttributeParameterPack& add(
			const std::string & attrname,
			::std::initializer_list<const DataType> data)
		{
			std::vector<DataType> vd(data.begin(), data.end());
			newAtts.push_back(std::shared_ptr<Almost_Attribute<DataType>>(
				new Almost_Attribute<DataType>(attrname, gsl::make_span(vd), { data.size() })));
			return *this;
		}

		template <class DataType>
		AttributeParameterPack& add(
			const std::string & attrname,
			DataType data)
		{
			newAtts.push_back(std::shared_ptr<Almost_Attribute<DataType>>(
				new Almost_Attribute<DataType>(attrname, data, { 1 })));
			return *this;
		}

	};
}
