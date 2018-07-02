#pragma once
#include <hdf5.h>
#include <hdf5_hl.h>
#include <gsl/pointers>
#if __has_include(<Eigen/Dense>)
# include<Eigen/Dense>
#endif
#include "Handles.hpp"
#include "Types.hpp"
#include "Attributes.hpp"
#include "Tags.hpp"
#include "PropertyLists.hpp"


/* TODOs:
- Write and read from Eigen objects
- Add column names
- Write complex objects / types
- Compression handling?
- Convenience function to create a property list that sets the fill value, chunking, compression type and level.
*/

namespace HH {
	using namespace HH::Handles;
	using namespace HH::Types;
	using namespace gsl;
	using std::initializer_list;
	using std::tuple;

	struct Dataset {
	private:
		HH_hid_t dset;
	public:
		Dataset(HH_hid_t hnd_dset) : dset(hnd_dset), atts(hnd_dset) {}
		virtual ~Dataset() {}
		HH_hid_t get() const { return dset; }

		/// Attributes
		Has_Attributes atts;

		// Get type
		[[nodiscard]] HH_hid_t getType() const
		{
			return HH_hid_t(H5Dget_type(dset()), Closers::CloseHDF5Datatype::CloseP);
		}

		// Get dataspace
		[[nodiscard]] HH_hid_t getSpace() const
		{
			return HH_hid_t(H5Dget_space(dset()), Closers::CloseHDF5Dataspace::CloseP);
		}

		/// Get current and maximum dimensions, and number of total points.
		struct Dimensions {
			std::vector<hsize_t> dimsCur, dimsMax;
			size_t dimensionality;
			size_t numElements;
			Dimensions(const std::vector<hsize_t>& dimscur, const std::vector<hsize_t> &dimsmax, size_t dality, size_t np)
				: dimsCur(dimscur), dimsMax(dimsmax), dimensionality(dality), numElements(np) {}
		};
		Dimensions getDimensions() const
		{
			std::vector<hsize_t> dimsCur, dimsMax;
			auto space = getSpace();
			Expects(H5Sis_simple(space()) > 0);
			hssize_t numPoints = H5Sget_simple_extent_npoints(space());
			int dimensionality = H5Sget_simple_extent_ndims(space());
			Expects(dimensionality >= 0);
			dimsCur.resize(dimensionality);
			dimsMax.resize(dimensionality);
			int err = H5Sget_simple_extent_dims(space(), dimsCur.data(), dimsMax.data());
			Expects(err >= 0);

			return Dimensions(dimsCur, dimsMax, dimsCur.size(), numPoints);
		}

		// Resize the dataset?


		/// \brief Write the dataset
		/// \note Ensure that the correct dimension ordering is preserved.
		/// \note With default parameters, the entire dataset is written.
		template <class DataType, class Marshaller = HH::Types::Object_Accessor<DataType> >
		[[nodiscard]] herr_t write(
			span<const DataType> data,
			HH_hid_t in_memory_dataType = HH::Types::GetHDF5Type<DataType>(),
			HH_hid_t mem_space_id = H5S_ALL,
			HH_hid_t file_space_id = H5S_ALL,
			HH_hid_t xfer_plist_id = H5P_DEFAULT)
		{
			Marshaller m;
			auto d = m.serialize(data);
			return H5Dwrite(
				dset(), // dataset id
				in_memory_dataType(), // mem_type_id
				mem_space_id(), // mem_space_id
				file_space_id(), // file_space_id
				xfer_plist_id(), // xfer_plist_id
				d // data
				  //data.data() // data
			);
		}

		/// \brief Read the dataset
		/// \note Ensure that the correct dimension ordering is preserved
		/// \note With default parameters, the entire dataset is read
		template <class DataType>
		[[nodiscard]] herr_t read(
			span<DataType> data,
			HH_hid_t in_memory_dataType = HH::Types::GetHDF5Type<DataType>(),
			HH_hid_t mem_space_id = H5S_ALL,
			HH_hid_t file_space_id = H5S_ALL,
			HH_hid_t xfer_plist_id = H5P_DEFAULT)
		{
			return H5Dread(
				dset(), // dataset id
				in_memory_dataType(), // mem_type_id
				mem_space_id(), // mem_space_id
				file_space_id(), // file_space_id
				xfer_plist_id(), // xfer_plist_id
				data.data() // data
			);
		}

		/// \todo Read and write using Eigen (convenience functions)

		/// Attach a dimension scale to this table.
		Dataset attachDimensionScale(unsigned int DimensionNumber, const Dataset& scale)
		{
			const herr_t res = H5DSattach_scale(dset(), scale.dset(), DimensionNumber);
			Expects(res == 0);
			return *this;
		}
		/// Detach a dimension scale
		Dataset detachDimensionScale(unsigned int DimensionNumber, const Dataset& scale)
		{
			const herr_t res = H5DSdetach_scale(dset(), scale.dset(), DimensionNumber);
			Expects(res == 0);
			return *this;
		}
		Dataset setDims(std::initializer_list<Dataset> dims) {
			unsigned int i = 0;
			for (auto it = dims.begin(); it != dims.end(); ++it, ++i) {
				attachDimensionScale(i, *it);
			}
			return *this;
		}
		Dataset setDims(const Dataset &dims) {
			attachDimensionScale(0, dims);
			return *this;
		}
		Dataset setDims(const Dataset &dim1, const Dataset &dim2) {
			attachDimensionScale(0, dim1);
			attachDimensionScale(0, dim2);
			return *this;
		}
		Dataset setDims(const Dataset &dim1, const Dataset &dim2, const Dataset &dim3) {
			attachDimensionScale(0, dim1);
			attachDimensionScale(0, dim2);
			attachDimensionScale(0, dim3);
			return *this;
		}
		/*
		template <typename AttType>
		Dataset AddSimpleAttributes(const std::pair<std::string, AttType> &att)
		{
		atts.add<AttType>(att.first, att.second);
		return *this;
		}
		template <typename AttType, typename... Rest>
		Dataset AddSimpleAttributes(const std::pair<std::string, AttType> &att, Rest&&... rest)
		{
		AddSimpleAttributes(att);
		AddSimpleAttributes<Rest...>(std::forward<Rest>(rest)...);
		return *this;
		}
		template <typename AttType>
		Dataset AddSimpleAttributes(const std::string &attname, AttType val)
		{
		atts.add<AttType>(attname.c_str(), val);
		return *this;
		}
		*/

		/// \note NameType is unused; should always be const char*. Exists to make my template formatting easier.
		template <typename NameType, typename AttType>
		Dataset AddSimpleAttributes(
			NameType attname, AttType val)
		{
			atts.add<AttType>(attname, { val }, { 1 });
			/// \todo Check return value
			return *this;
		}
		template <typename NameType, typename AttType, typename... Rest>
		Dataset AddSimpleAttributes(NameType attname, AttType val, Rest... rest)
		{
			AddSimpleAttributes<NameType, AttType>(attname, val);
			AddSimpleAttributes<Rest...>(rest...);
			return *this;
		}

		/// Is this Table used as a dimension scale?
		bool isDimensionScale() const {
			const htri_t res = H5DSis_scale(dset());
			Expects(res >= 0);
			if (res > 0) return true;
			return false;
		}

		/// Designate this table as a dimension scale
		Dataset setIsDimensionScale(const std::string &dimensionScaleName) {
			const htri_t res = H5DSset_scale(dset(), dimensionScaleName.c_str());
			Expects(res == 0);
			return *this;
		}
		/// Set the axis label for the dimension designated by DimensionNumber
		Dataset setDimensionScaleAxisLabel(unsigned int DimensionNumber, const std::string &label)
		{
			const htri_t res = H5DSset_label(dset(), DimensionNumber, label.c_str());
			Expects(res == 0);
			return *this;
		}
		/// \brief Get the axis label for the dimension designated by DimensionNumber
		/// \todo See if there is ANY way to dynamically determine the label size. HDF5 docs do not discuss this.
		std::string getDimensionScaleAxisLabel(unsigned int DimensionNumber) const
		{
			constexpr size_t max_label_size = 1000;
			std::array<char, max_label_size> label;
			label.fill('\0');
			const ssize_t res = H5DSget_label(dset(), DimensionNumber, label.data(), max_label_size);
			// res is the size of the label. The HDF5 documentation does not include whether the label is null-terminated,
			// so I am terminating it manually.
			label[max_label_size - 1] = '\0';
			return std::string(label.data());
		}
		Dataset getDimensionScaleAxisLabel(unsigned int DimensionNumber, std::string &res) const {
			res = getDimensionScaleAxisLabel(DimensionNumber);
			return *this;
		}
		/// \brief Get the name of this table's defined dimension scale
		/// \todo See if there is ANY way to dynamically determine the label size. HDF5 docs do not discuss this.
		std::string getDimensionScaleName() const
		{
			constexpr size_t max_label_size = 1000;
			std::array<char, max_label_size> label;
			label.fill('\0');
			const ssize_t res = H5DSget_scale_name(dset(), label.data(), max_label_size);
			// res is the size of the label. The HDF5 documentation does not include whether the label is null-terminated,
			// so I am terminating it manually.
			label[max_label_size - 1] = '\0';
			return std::string(label.data());
		}
		Dataset getDimensionScaleName(std::string &res) const {
			res = getDimensionScaleName();
			return *this;
		}
	};

	struct Has_Datasets {
	private:
		HH_hid_t base;
	public:
		Has_Datasets(HH_hid_t obj) : base(obj) {}
		virtual ~Has_Datasets() {}

		/// \brief Does a dataset with the specified name exist?
		/// This checks for a link with the given name, and checks that the link is a dataset.
		htri_t exists(gsl::not_null<const char*> dsetname, HH_hid_t LinkAccessPlist = H5P_DEFAULT) {
			htri_t linkExists = H5Lexists(base(), dsetname.get(), LinkAccessPlist());
			if (linkExists <= 0) return linkExists;
			H5O_info_t oinfo;
			herr_t err = H5Oget_info_by_name(base(), dsetname.get(), &oinfo, H5P_DEFAULT); // H5P_DEFAULT only, per docs.
			if (err < 0) return -1;
			if (oinfo.type == H5O_type_t::H5O_TYPE_DATASET) return 1;
			return 0;
		}

		// Remove by name - handled by removing the link

		// Rename - handled by a new hard link

		/// \brief Open a dataset
		Dataset open(gsl::not_null<const char*> dsetname,
			HH_hid_t DatasetAccessPlist = H5P_DEFAULT)
		{
			hid_t dsetid = H5Dopen(base(), dsetname.get(), DatasetAccessPlist());
			Expects(dsetid >= 0);
			return Dataset(HH_hid_t(dsetid, Closers::CloseHDF5Dataset::CloseP));
		}

		Dataset operator[](gsl::not_null<const char*> dsetname) {
			return open(dsetname);
		}

		/// \brief Create a dataset
		template <class DataType>
		Dataset create(
			gsl::not_null<const char*> dsetname,
			initializer_list<size_t> dimensions,
			HH_hid_t dtype = HH::Types::GetHDF5Type<DataType>(),
			HH_hid_t LinkCreationPlist = H5P_DEFAULT,
			HH_hid_t DatasetCreationPlist = H5P_DEFAULT,
			HH_hid_t DatasetAccessPlist = H5P_DEFAULT)
		{
			std::vector<hsize_t> hdims;
			for (const auto &d : dimensions)
				hdims.push_back(gsl::narrow_cast<hsize_t>(d));
			HH_hid_t dspace{
				H5Screate_simple(
					gsl::narrow_cast<int>(dimensions.size()),
					hdims.data(),
					nullptr),
				Closers::CloseHDF5Dataspace::CloseP };

			hid_t dsetid = H5Dcreate(base(), dsetname.get(),
				dtype(),
				dspace(),
				LinkCreationPlist(),
				DatasetCreationPlist(),
				DatasetAccessPlist());
			Expects(dsetid >= 0);
			return Dataset(HH_hid_t(dsetid, Closers::CloseHDF5Dataset::CloseP));
		}

		template <class DataType, class ... Args>
		Dataset create(std::tuple<Args...> vals) {
			using namespace Tags;
			using namespace Tags::PropertyLists;
			typedef std::tuple<Args...> vals_t;
			static_assert(has_type<t_name, vals_t >::value, "Must specify the dataset name");
			static_assert(has_type<t_dimensions, vals_t >::value, "Must specify the dataset dimensions");

			auto name = std::get<t_name>(vals);
			auto dims = std::get<t_dimensions>(vals);

			auto dtype = t_datatype(HH::Types::GetHDF5Type<DataType>()); getOptionalValue(dtype, vals);

			auto LinkCreationPlist = t_LinkCreationPlist(H5P_DEFAULT); getOptionalValue(LinkCreationPlist, vals);
			auto DatasetCreationPlist = t_DatasetCreationPlist(H5P_DEFAULT); getOptionalValue(DatasetCreationPlist, vals);
			auto DatasetAccessPlist = t_DatasetAccessPlist(H5P_DEFAULT); getOptionalValue(DatasetAccessPlist, vals);

			Dataset d = create<DataType>(name.data, dims.data, dtype.data, LinkCreationPlist.data, DatasetCreationPlist.data, DatasetAccessPlist.data);

			// Optionally, write the data: t_data_span, t_data_initializer_list, t_data_eigen
			constexpr bool has_span = has_type<t_data_span<DataType>, vals_t >::value;
			constexpr bool has_initializer_list = has_type<t_data_initializer_list<DataType>, vals_t >::value;
			constexpr bool has_eigen = has_type<t_data_eigen<DataType>, vals_t >::value;
			constexpr unsigned int num_initializers =
				((has_span) ? 1 : 0) + ((has_initializer_list) ? 1 : 0) + ((has_eigen) ? 1 : 0);
			static_assert(num_initializers <= 1, "You are trying to set the data in a dataset using "
				"more than one source in a single function call.");

			// I need in_memory_dataType, mem_space_id,
			//	file_space_id, xfer_plist_id
			//t_datatype; --- need separate in_memory datatype?
			auto file_space = t_file_space(H5S_ALL); getOptionalValue(file_space, vals);
			auto mem_space = t_mem_space(H5S_ALL); getOptionalValue(mem_space, vals);
			auto xfer_plist = t_xferPlist(H5P_DEFAULT); getOptionalValue(xfer_plist, vals);

			/*
			if (has_span) {
			auto data = t_data_span<DataType>; getOptionalValue(data, vals);
			Expects(0 <= d.write<DataType>(data.data, dtype.data, mem_space.data,
			file_space.data, xfer_plist.data));
			}
			else if (has_initializer_list) {
			auto data = t_data_initializer_list<DataType>; getOptionalValue(data, vals);
			Expects(0 <= d.write<DataType>(
			gsl::make_span(data.data.begin(), data.data.size()),
			dtype.data, mem_space.data,
			file_space.data, xfer_plist.data));
			//return createWithData(dsetname, gsl::make_span(data.begin(), data.size()),
			//	dimensions, dtype, LinkCreationPlist, DatasetCreationPlist,
			//	DatasetAccessPlist, in_memory_dataType,
			//	mem_space_id, file_space_id, xfer_plist_id);
			}
			else if (has_eigen) {
			auto data = t_data_eigen<DataType>; getOptionalValue(data, vals);
			typedef DataType::RealScalar RealDataType;

			bool isRowMajor = data.data.IsRowMajor;
			auto rows = data.data.rows();
			auto cols = data.data.cols();
			if ((rows == 0) || (cols == 0)) throw;
			//std::initializer_list<size_t> mDims{ gsl::narrow_cast<hsize_t>(rows), gsl::narrow_cast<hsize_t>(cols) };
			//if (collapseSingularDimensions) {
			//	if ((rows > 2) && (cols > 2)) mDims = std::initializer_list<size_t>{ gsl::narrow_cast<hsize_t>(rows), gsl::narrow_cast<hsize_t>(cols) };
			//	else mDims = std::initializer_list<size_t>{ std::max(gsl::narrow_cast<hsize_t>(rows),gsl::narrow_cast<hsize_t>(cols)) };
			//}
			if (isRowMajor
			|| ((rows == 1) || (cols == 1)) ) {
			Expects(0 <= d.write<DataType>(
			// data.data.data - damn.
			// (data.data) is the Eigen object.
			gsl::make_span(data.data.data(), data.data.size()),
			dtype.data, mem_space.data,
			file_space.data, xfer_plist.data));
			//return createWithData<RealDataType>(dsetname, gsl::make_span(data.data(), data.size()), mDims,
			//	dtype, LinkCreationPlist, DatasetCreationPlist, DatasetAccessPlist, in_memory_dataType,
			//	mem_space_id, file_space_id, xfer_plist_id);
			}
			else {
			throw;
			// A memory copy shall occur...

			//return createWithData<RealDataType>(dsetname, gsl::make_span(data.data(), data.size()), mDims,
			//	dtype, LinkCreationPlist, DatasetCreationPlist, DatasetAccessPlist, in_memory_dataType,
			//	mem_space_id, file_space_id, xfer_plist_id);
			}
			}
			*/

			return d;
		}
		template <class DataType, class ... Args>
		Dataset create(Args... args) {
			auto t = std::make_tuple(args...);
			return create<DataType, Args...>(t);
		}

	};
}