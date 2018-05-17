#pragma once
#include <tuple>
#include <typeinfo>
#include <typeindex>
#include <vector>
#include <gsl/gsl>
#include <hdf5.h>

#include "Handles.hpp"
#include "Tags.hpp"
#include "Types.hpp"

namespace HH {
	struct Attributes_Container
	{
	private:
		// This is a weak object. It does not close.
		not_invalid<HH_hid_t> base;
	public:
		/// @name General Functions
		/// @{
		/// Does an attribute with the specified name exist?
		[[nodiscard]] static htri_t exists(not_invalid<HH_hid_t> base, not_null<const char*> attname)
		{
			return H5Aexists(base().h, attname.get());
		}
		/// Delete an attribute with the specified name.
		/// \note The base HDF5 function is H5Adelete, but delete is a reserved name in C++.
		/// \returns false on error, true on success.
		[[nodiscard]] static bool remove(not_invalid<HH_hid_t> base, not_null<const char*> attname)
		{
			herr_t err = H5Adelete(base().h, attname.get());
			if (err >= 0) return true;
			return false;
		}
		/// \\brief Open an attribute
		[[nodiscard]] static Attributes open(
			const not_invalid<HH_hid_t> &base,
			not_null<const char*> name,
			not_invalid<HH_hid_t> AttributeAccessPlist = H5P_DEFAULT)
		{
			return std::move(H5A_ScopedHandle(H5Aopen(base().h, name.get(), AttributeAccessPlist.get().h)));
		}
		/// \brief Create an attribute, without setting its data.
		template <class DataType>
		[[nodiscard]] H5A_ScopedHandle&& create(
			not_invalid<HH_hid_t> base,
			not_null<const char*> attrname,
			initializer_list<size_t> dimensions = { 1 },
			not_invalid<HH_hid_t> AttributeCreationPlist = H5P_DEFAULT,
			not_invalid<HH_hid_t> AttributeAccessPlist = H5P_DEFAULT)
		{
			auto dtype = GetHDF5Type<DataType>();

			std::vector<hsize_t> hdims;
			for (const auto &d : dimensions)
				hdims.push_back(gsl::narrow_cast<hsize_t>(d));
			H5S_ScopedHandle dspace{
				H5Screate_simple(
					gsl::narrow_cast<int>(dimensions.size()),
					hdims.data(),
					nullptr) };

			return std::move(H5A_ScopedHandle(H5Acreate(
				base().h,
				attrname.get(),
				dtype.get(),
				dspace.get(),
				AttributeCreationPlist.get(),
				AttributeAccessPlist.get()
			));
		}

	};

	struct Attribute {
	private:
		not_invalid<HH_hid_t> attr;
	public:
		using namespace HH::Handles;
		using namespace gsl;
		using std::initializer_list;
		using std::tuple;

		Attributes(not_invalid<HH_hid_t> hnd_attr) : attr(hnd_attr) {}
		virtual ~Attributes() {}

		
		
		/// \brief Write data to an attribute
		/// \note Writing attributes is an all-or-nothing process.
		template <class DataType>
		[[nodiscard]] herr_t write(
			span<DataType> data,
			not_invalid<HH_hid_t> in_memory_dataType = GetHDF5Type<DataType>())
		{
			const size_t in_memory_size_bytes = H5Tget_size(in_memory_dataType().h);
			Expects((data.size_bytes() == in_memory_size_bytes) && "Memory alignment error");
			return H5Awrite(attr().h, in_memory_dataType().h, data.data());
		}

		/// \brief Read data from an attribute
		/// \note Reading attributes is an all-or-nothing process.
		template <class DataType>
		[[nodiscard]] herr_t read(
			span<DataType> data,
			not_invalid<HH_hid_t> in_memory_dataType = GetHDF5Type<DataType>())
		{
			const size_t in_memory_size_bytes = H5Tget_size(in_memory_dataType().h);
			Expects((data.size_bytes() == in_memory_size_bytes) && "Memory alignment error");
			return H5Aread(attr().h, in_memory_dataType().h, data.data());
		}

		// Get an attribute's name
		[[nodiscard]] ssize_t get_name(size_t buf_size, char* buf);

		// Rename an attribute
		[[nodiscard]] htri_t rename(not_null<const char*> newName);

		/// @}

		/// @name Type-querying Functions
		/// @{
		/// Get attribute type, as an HDF5 type object.
		/// \see Types.hpp for the functions to compare the HDF5 type with a system type.
		[[nodiscard]] H5T_ScopedHandle getAttributeType();

		/// Convenience function to check an attribute's type. 
		/// \returns True if the type matches
		/// \returns False (0) if the type does not match
		/// \returns <0 if an error occurred.
		template <class DataType>
		[[nodiscard]] htri_t IsAttributeOfType();

		/// Get an attribute's size

		/// @}


		/// @name Convenience functions
		/// @{

		/// Create and write an attribute, for arbitrary dimensions.
		template <class DataType>
		H5A_ScopedHandle&& add(
			not_invalid<HH_hid_t> base,
			not_null<const char*> attrname,
			span<DataType> data,
			initializer_list<size_t> dimensions = { data.size() },
			not_invalid<HH_hid_t> AttributeCreationPlist = H5P_DEFAULT,
			not_invalid<HH_hid_t> AttributeAccessPlist = H5P_DEFAULT,
			not_invalid<HH_hid_t> in_memory_dataType = GetHDF5Type<DataType>())
		{
			H5A_ScopedHandle newAttr = create<DataType>(base().h, attrname.get(), dimensions,
				AttributeCreationPlist, AttributeAccessPlist);
			Expects(newAttr.valid());
			herr_t res = write<DataType>(newAttr().h, data, in_memory_dataType);
			Expects(res == true);
			return std::move(newAttr);
		}

		/// Open and read an attribute, for expected dimensions.
		template <class DataType>
		H5A_ScopedHandle&& read(
			not_invalid<HH_hid_t> base,
			not_null<const char*> attrname,
			span<DataType> data,
			not_invalid<HH_hid_t> AttributeAccessPlist = H5P_DEFAULT,
			not_invalid<HH_hid_t> in_memory_dataType = GetHDF5Type<DataType>())
		{
			auto attr = open(base, attrname, AttributeAccessPlist);
			Expects(attr.valid());
			auto res = read(attr(), data, in_memory_dataType);
			Expects(res >= 0);
			return std::move(res);
		}

		/// Open and read an attribute, with unknown dimensions
		template <class DataType>
		H5A_ScopedHandle&& read(
			not_invalid<HH_hid_t> base,
			not_null<const char*> attrname,
			std::vector<DataType> &data,
			not_invalid<HH_hid_t> AttributeAccessPlist = H5P_DEFAULT)
		{
			data.clear();
			static_assert(false, "FINISH");
			// Query the data dimensions.
			// Resize
			return read(base, attrname, make_span(data.data(), data.size()));
		}

		/// @}

	}

}
