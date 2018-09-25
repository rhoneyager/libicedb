#pragma once
#include <map>
#include <vector>
#include <hdf5.h>
#include <hdf5_hl.h>
#include <gsl/pointers>

#if HH_HAS_EIGEN
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
		Dataset(HH_hid_t hnd_dset = HH::HH_hid_t::dummy()) : dset(hnd_dset), atts(hnd_dset) {}
		virtual ~Dataset() {}
		HH_hid_t get() const { return dset; }


		static bool isDataset(HH_hid_t obj) {
			H5I_type_t typ = H5Iget_type(obj());
			if (typ == H5I_DATASET) return true;
			//H5O_info_t oinfo;
			//herr_t err = H5Oget_info(obj(), &oinfo);
			//if (err < 0) return false;
			//if (oinfo.type == H5O_type_t::H5O_TYPE_DATASET) return true;
			return false;
		}
		bool isDataset() const { return isDataset(dset); }

		/// Attributes
		Has_Attributes atts;

		/// Get type
		HH_NODISCARD HH_hid_t getType() const
		{
			Expects(isDataset());
			return HH_hid_t(H5Dget_type(dset()), Closers::CloseHDF5Datatype::CloseP);
		}
		/// Get type
		inline HH_hid_t type() const { return getType(); }

		/// Convenience function to check an dataset's type. 
		/// \returns True if the type matches
		/// \returns False (0) if the type does not match
		/// \returns <0 if an error occurred.
		template <class DataType>
		htri_t isOfType() const {
			auto ttype = HH::Types::GetHDF5Type<DataType>();
			HH_hid_t otype = getType();
			return H5Tequal(ttype(), otype());
		}

		// Get dataspace
		HH_NODISCARD HH_hid_t getSpace() const
		{
			Expects(isDataset());
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
			Expects(isDataset());
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
		HH_NODISCARD herr_t write(
			const span<const DataType> data,
			HH_hid_t in_memory_dataType = HH::Types::GetHDF5Type<DataType>(),
			HH_hid_t mem_space_id = H5S_ALL,
			HH_hid_t file_space_id = H5S_ALL,
			HH_hid_t xfer_plist_id = H5P_DEFAULT)
		{
			Expects(isDataset());
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
		template <class DataType, class Marshaller = HH::Types::Object_Accessor<DataType> >
		HH_NODISCARD herr_t write(
			std::initializer_list<const DataType> data,
			HH_hid_t in_memory_dataType = HH::Types::GetHDF5Type<DataType>(),
			HH_hid_t mem_space_id = H5S_ALL,
			HH_hid_t file_space_id = H5S_ALL,
			HH_hid_t xfer_plist_id = H5P_DEFAULT)
		{
			return write<DataType, Marshaller>(gsl::span<const DataType>(data.begin(), data.size()),
				in_memory_dataType, mem_space_id, file_space_id, xfer_plist_id);
		}

		/// \brief Read the dataset
		/// \note Ensure that the correct dimension ordering is preserved
		/// \note With default parameters, the entire dataset is read
		template <class DataType>
		HH_NODISCARD herr_t read(
			span<DataType> data,
			HH_hid_t in_memory_dataType = HH::Types::GetHDF5Type<DataType>(),
			HH_hid_t mem_space_id = H5S_ALL,
			HH_hid_t file_space_id = H5S_ALL,
			HH_hid_t xfer_plist_id = H5P_DEFAULT) const
		{
			Expects(isDataset());
			return H5Dread(
				dset(), // dataset id
				in_memory_dataType(), // mem_type_id
				mem_space_id(), // mem_space_id
				file_space_id(), // file_space_id
				xfer_plist_id(), // xfer_plist_id
				data.data() // data
			);
		}

#if HH_HAS_EIGEN
		template <class EigenClass>
		void readWithEigen(EigenClass &&res, bool resize = true) const
		{
			typedef typename EigenClass::Scalar ScalarType;
			HH_hid_t dtype = HH::Types::GetHDF5Type<ScalarType>();
			// Check that the dims are 1 or 2.
			auto dims = getDimensions();
			if (resize)
				Expects(dims.dimensionality <= 2);
			int nDims[2] = { 1, 1 };
			if (dims.dimsCur.size() >= 1) nDims[0] = gsl::narrow_cast<int>(dims.dimsCur[0]);
			if (dims.dimsCur.size() >= 2) nDims[1] = gsl::narrow_cast<int>(dims.dimsCur[1]);

			if (resize)
				res.resize(nDims[0], nDims[1]);
			else
				Expects(dims.numElements == (size_t)(res.rows()*res.cols()));

			// Array copy to preserve row vs column major format.
			/// \todo Implement this more efficiently (i.e. only when needed).
			Eigen::Array<ScalarType, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> data_in;
			data_in.resize(res.rows(), res.cols());

			herr_t e = read<ScalarType>(gsl::span<ScalarType>(data_in.data(), dims.numElements));
			Expects(e >= 0);

			res = data_in;
		}
		template <class EigenClass>
		void readWithEigen(EigenClass &res, bool resize = true) const
		{
			typedef typename EigenClass::Scalar ScalarType;
			HH_hid_t dtype = HH::Types::GetHDF5Type<ScalarType>();
			// Check that the dims are 1 or 2.
			auto dims = getDimensions();
			if (resize)
				Expects(dims.dimensionality <= 2);
			int nDims[2] = { 1, 1 };
			if (dims.dimsCur.size() >= 1) nDims[0] = gsl::narrow_cast<int>(dims.dimsCur[0]);
			if (dims.dimsCur.size() >= 2) nDims[1] = gsl::narrow_cast<int>(dims.dimsCur[1]);

			if (resize)
				res.resize(nDims[0], nDims[1]);
			else
				Expects(dims.numElements == (size_t)(res.rows()*res.cols()));

			// Array copy to preserve row vs column major format.
			/// \todo Implement this more efficiently (i.e. only when needed).
			Eigen::Array<ScalarType, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> data_in;
			data_in.resize(res.rows(), res.cols());

			herr_t e = read<ScalarType>(gsl::span<ScalarType>(data_in.data(), dims.numElements));
			Expects(e >= 0);

			res = data_in;
		}
#endif


		/// Attach a dimension scale to this table.
		Dataset attachDimensionScale(unsigned int DimensionNumber, const Dataset& scale)
		{
			Expects(isDataset());
			Expects(isDataset(scale.get()));
			const herr_t res = H5DSattach_scale(dset(), scale.dset(), DimensionNumber);
			Expects(res == 0);
			return *this;
		}
		/// Detach a dimension scale
		Dataset detachDimensionScale(unsigned int DimensionNumber, const Dataset& scale)
		{
			Expects(isDataset());
			Expects(isDataset(scale.get()));
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
			attachDimensionScale(1, dim2);
			return *this;
		}
		Dataset setDims(const Dataset &dim1, const Dataset &dim2, const Dataset &dim3) {
			attachDimensionScale(0, dim1);
			attachDimensionScale(1, dim2);
			attachDimensionScale(2, dim3);
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
			Expects(isDataset());
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

		/// Is this dataset used as a dimension scale?
		bool isDimensionScale() const {
			Expects(isDataset());
			const htri_t res = H5DSis_scale(dset());
			Expects(res >= 0);
			if (res > 0) return true;
			return false;
		}

		/// Designate this table as a dimension scale
		Dataset setIsDimensionScale(const std::string &dimensionScaleName) {
			Expects(isDataset());
			const htri_t res = H5DSset_scale(dset(), dimensionScaleName.c_str());
			Expects(res == 0);
			return *this;
		}
		/// Set the axis label for the dimension designated by DimensionNumber
		Dataset setDimensionScaleAxisLabel(unsigned int DimensionNumber, const std::string &label)
		{
			Expects(isDataset());
			const htri_t res = H5DSset_label(dset(), DimensionNumber, label.c_str());
			Expects(res == 0);
			return *this;
		}
		/// \brief Get the axis label for the dimension designated by DimensionNumber
		/// \todo See if there is ANY way to dynamically determine the label size. HDF5 docs do not discuss this.
		std::string getDimensionScaleAxisLabel(unsigned int DimensionNumber) const
		{
			Expects(isDataset());
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
			Expects(isDataset());
			res = getDimensionScaleAxisLabel(DimensionNumber);
			return *this;
		}
		/// \brief Get the name of this table's defined dimension scale
		/// \todo See if there is ANY way to dynamically determine the label size. HDF5 docs do not discuss this.
		std::string getDimensionScaleName() const
		{
			Expects(isDataset());
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

		/// Is a dimension scale attached to this dataset in a certain position?
		htri_t isDimensionScaleAttached(const Dataset &scale, unsigned int DimensionNumber) const {
			return H5DSis_attached(dset(), scale.get()(), DimensionNumber);
		}
	};

	struct Has_Datasets {
	private:
		HH_hid_t base;
	public:
		Has_Datasets(HH_hid_t obj) : base(obj) {  }
		virtual ~Has_Datasets() {}

		/// \brief Does a dataset with the specified name exist?
		/// This checks for a link with the given name, and checks that the link is a dataset.
		htri_t exists(gsl::not_null<const char*> dsetname, HH_hid_t LinkAccessPlist = H5P_DEFAULT) const {
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
			HH_hid_t DatasetAccessPlist = H5P_DEFAULT) const
		{
			hid_t dsetid = H5Dopen(base(), dsetname.get(), DatasetAccessPlist());
			Expects(dsetid >= 0);
			return Dataset(HH_hid_t(dsetid, Closers::CloseHDF5Dataset::CloseP));
		}

		Dataset operator[](gsl::not_null<const char*> dsetname) const {
			return open(dsetname);
		}

		/// \brief List all datasets under this group
		std::vector<std::string> list() const {
			std::vector<std::string> res;
			H5G_info_t info;
			herr_t e = H5Gget_info(base(), &info);
			Expects(e >= 0);
			for (hsize_t i = 0; i < info.nlinks; ++i) {
				// Get the name
				ssize_t szName = H5Lget_name_by_idx(base(), ".", H5_INDEX_NAME, H5_ITER_INC,
					i, NULL, 0, H5P_DEFAULT);
				Expects(szName >= 0);
				std::vector<char> vName(szName + 1, '\0');
				H5Lget_name_by_idx(base(), ".", H5_INDEX_NAME, H5_ITER_INC,
					i, vName.data(), szName + 1, H5P_DEFAULT);

				// Get the object and check the type
				H5O_info_t oinfo;
				herr_t err = H5Oget_info_by_name(base(), vName.data(), &oinfo, H5P_DEFAULT); // H5P_DEFAULT only, per docs.
				if (err < 0) continue;
				if (oinfo.type == H5O_type_t::H5O_TYPE_DATASET) res.push_back(std::string(vName.data()));
			}
			return res;
		}

		/// \brief Open all datasets under the group. Convenience function
		std::map<std::string, Dataset> openAll() const {
			auto ls = list();
			std::map<std::string, Dataset> res;
			for (const auto &l : ls) {
				res[l] = open(l.c_str());
			}
			return res;
		}

	private:
		template <class DataType>
		Dataset _create(
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


	public:
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
			return _create<DataType>(dsetname, dimensions, dtype,
				LinkCreationPlist, DatasetCreationPlist, DatasetAccessPlist);
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

			// NOTE: This needs to always hit the non-templated type.
			Dataset d = _create<DataType>(name.data,
				dims.data,
				dtype.data,
				LinkCreationPlist.data,
				DatasetCreationPlist.data,
				DatasetAccessPlist.data);

			// Optionally, write the data: t_data_span, t_data_initializer_list, t_data_eigen
			constexpr bool has_span = has_type<t_data_span<DataType>, vals_t >::value;
			constexpr bool has_initializer_list = has_type<t_data_initializer_list<DataType>, vals_t >::value;
#if HH_HAS_EIGEN
			constexpr bool has_eigen = has_type<t_data_eigen<DataType>, vals_t >::value;
#else
			constexpr bool has_eigen = false;
#endif
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

#if HH_HAS_EIGEN
		template <class EigenClass>
		Dataset createWithEigen(
			gsl::not_null<const char*> dsetname,
			const EigenClass &d, int nDims = 2, int nRows = -1, int nCols = -1, int nZ = -1)
		{
			typedef typename EigenClass::Scalar ScalarType;
			HH_hid_t dtype = HH::Types::GetHDF5Type<ScalarType>();
			/// \todo Handle the different row and column major formats for output more efficiently.
			Eigen::Array<ScalarType, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor > dout;
			dout.resize(d.rows(), d.cols());
			dout = d;
			const auto &dconst = dout;
			if (nRows == -1) nRows = (int)d.rows();
			if (nCols == -1) nCols = (int)d.cols();
			if (nZ < 0)
				Expects(nRows*nCols == (int)(d.rows()*d.cols()));
			else
				Expects(nRows*nCols*nZ == (int)(d.rows()*d.cols()));

			if (nDims == 1) {
				auto obj = _create<ScalarType>(dsetname, { (size_t)nRows * (size_t)nCols }, dtype);
				auto sp = gsl::make_span(dconst.data(), (int)(nRows*nCols));
				//htri_t res = obj.write< typename EigenClass::Scalar >(sp);
				htri_t res = obj.write(sp);
				Expects(0 <= res);
				return obj;
			}
			else if (nDims == 2) {
				// Object like obj[x][y], where rows are x and cols are y.
				auto obj = _create<ScalarType>(dsetname, { (size_t)nRows, (size_t)nCols }, dtype);
				//auto res = obj.write<ScalarType>(gsl::make_span(dout.data(), dout.rows()*dout.cols()));
				auto sp = (gsl::make_span(dconst.data(), (int)(nRows*nCols)));
				auto res = obj.write(sp);

				Expects(0 <= res);
				return obj;
			}
			else if (nDims == 3) {
				// Object like obj[x][y][z], where rows are x and cols are y.
				auto obj = _create<ScalarType>(dsetname, { (size_t)nRows, (size_t)nCols, (size_t)nZ }, dtype);
				auto sp = gsl::make_span(dconst.data(), (int)(nRows*nCols*nZ));
				htri_t res = obj.write(sp);
				Expects(0 <= res);
				return obj;
			}
			else throw;
		}
#endif

		template <class DataType>
		Dataset createFromSpan(
			gsl::not_null<const char*> dsetname,
			const gsl::span<const DataType> d, int nDims = 1, int nRows = -1, int nCols = -1, int nZ = -1)
		{
			HH_hid_t dtype = HH::Types::GetHDF5Type<DataType>();
			HH::Dataset obj = HH::Handles::HH_hid_t::dummy();
			if (nDims == 1) {
				obj = create<DataType>(dsetname, { gsl::narrow_cast<hsize_t>(d.size()) }, dtype);
			}
			else if (nDims == 2) {
				Expects(nRows > 0);
				Expects(nCols > 0);
				Expects(nRows*nCols == (int)d.size());
				obj = create<DataType>(dsetname, { gsl::narrow_cast<hsize_t>(nRows), gsl::narrow_cast<hsize_t>(nCols) }, dtype);
			}
			else if (nDims == 3) {
				Expects(nRows > 0);
				Expects(nCols > 0);
				Expects(nZ > 0);
				Expects(nRows*nCols*nZ == (int)d.size());
				obj = create<DataType>(dsetname,
					{ gsl::narrow_cast<hsize_t>(nRows), gsl::narrow_cast<hsize_t>(nCols), gsl::narrow_cast<hsize_t>(nZ) },
					dtype);
			}
			auto res = obj.write(d);
			Expects(0 <= res);
			return obj;
		}
	};
}
