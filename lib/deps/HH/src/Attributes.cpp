#include "HH/Attributes.hpp"

namespace HH {
	/// \todo Switch to explicit namespace specification.
	using namespace HH::Handles;
	using namespace HH::Types;
	using std::initializer_list;
	using std::tuple;

	Attribute::Attribute(HH_hid_t hnd_attr) : attr(hnd_attr) {  }
	Attribute::~Attribute() {}
	HH_hid_t Attribute::get() const { return attr; }

	bool Attribute::isAttribute(HH_hid_t obj) {
		H5I_type_t typ = H5Iget_type(obj());
		if (typ == H5I_BADID) throw HH_throw;
		if (typ == H5I_ATTR) return true;
		return false;
	}
	bool Attribute::isAttribute() const { return isAttribute(attr); }

	Attribute Attribute::writeFixedLengthString(
		const std::string& data)
	{
		HH_Expects(isAttribute());
		HH_hid_t dtype = HH::Types::GetHDF5TypeFixedString(data.size());
		if (H5Awrite(attr(), dtype(), data.data()) < 0) throw HH_throw.add("Reason", "H5Awrite failed.");
		return *this;
	}

	ssize_t Attribute::get_name(size_t buf_size, char* buf) const
	{
		HH_Expects(isAttribute());
		return H5Aget_name(attr(), buf_size, buf);
	}
	std::string Attribute::get_name() const
	{
		ssize_t sz = 0;
		sz = get_name(0, nullptr);
		HH_Expects(sz >= 0);
		size_t s = gsl::narrow_cast<size_t>(sz);
		std::vector<char> v(s+1,'\0');
		sz = get_name(v.size(), v.data());
		HH_Expects(sz>=0);
		return std::string(v.data());
	}


	Attribute::att_name_encoding Attribute::get_char_encoding() const {
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

	HH_hid_t Attribute::getType() const
	{
		HH_Expects(isAttribute());
		return HH_hid_t(H5Aget_type(attr()), Closers::CloseHDF5Datatype::CloseP);
	}
	HH_hid_t Attribute::getSpace() const
	{
		HH_Expects(isAttribute());
		return HH_hid_t(H5Aget_space(attr()), Closers::CloseHDF5Dataspace::CloseP);
	}
	hsize_t Attribute::getStorageSize() const
	{
		HH_Expects(isAttribute());
		return H5Aget_storage_size(attr());
	}
	Attribute::Dimensions Attribute::getDimensions() const
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
	void Attribute::describe(std::ostream& out) {
		auto d = getDimensions();
		//using namespace Tags::ObjSizes;
		using namespace std;
		out << "Attribute has:\n\tnPoints:\t" << d.numElements
			<< "\n\tDimensionality:\t" << d.dimensionality
			<< "\n\tDimensions:\t[\t";
		for (const auto& m : d.dimsCur)
			out << m << "\t";
		out << "]" << endl;
	}
	Almost_Attribute_base::~Almost_Attribute_base() {}
	Almost_Attribute_base::Almost_Attribute_base(
		const std::string& name)
		: name(name) {}
	
	Almost_Attribute_Fixed_String::~Almost_Attribute_Fixed_String() {}
	Attribute Almost_Attribute_Fixed_String::apply(HH::HH_hid_t obj) const
	{
		return addFixedLengthString(obj, name.c_str(), data);
	}
	Almost_Attribute_Fixed_String::Almost_Attribute_Fixed_String
		(const std::string& name,
		const std::string& data) : Almost_Attribute_base(name), data(data) {}

	HH_NODISCARD Attribute Almost_Attribute_Fixed_String::createFixedLengthString(
		HH_hid_t base,
		const ::std::string& attrname,
		hsize_t len,
		HH_hid_t AttributeCreationPlist,
		HH_hid_t AttributeAccessPlist)
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

		if (H5Iis_valid(attI()) <= 0) throw HH_throw.add("Reason", "Attribute invalid.");
		return Attribute(attI);
	}

	Attribute Almost_Attribute_Fixed_String::addFixedLengthString(
		HH_hid_t base,
		const ::std::string& attrname,
		const ::std::string& data,
		HH_hid_t AttributeCreationPlist,
		HH_hid_t AttributeAccessPlist
	)
	{
		auto newAttr = createFixedLengthString(base,
			attrname,
			data.size(),
			AttributeCreationPlist, AttributeAccessPlist);
		newAttr.writeFixedLengthString(data);
		return Attribute(std::move(newAttr));
	}


	Has_Attributes::~Has_Attributes() {}
	Has_Attributes::Has_Attributes(HH_hid_t obj) : base(obj) {}
	bool Has_Attributes::exists(const std::string& attname) const
	{
		auto ret = H5Aexists(base(), attname.c_str());
		if (ret < 0) throw HH_throw;
		return (ret > 0) ? true : false;
	}
	void Has_Attributes::remove(const std::string& attname)
	{
		herr_t err = H5Adelete(base(), attname.c_str());
		if (err < 0) throw HH_throw;
	}
	Attribute Has_Attributes::open(
		const std::string& name,
		HH_hid_t AttributeAccessPlist) const
	{
		auto ret = H5Aopen(base(), name.c_str(), AttributeAccessPlist());
		if (ret < 0) throw HH_throw;
		return Attribute(HH_hid_t(
			ret,
			Closers::CloseHDF5Attribute::CloseP
		));
	}
	Attribute Has_Attributes::operator[](const std::string& name) const { return open(name); }
	
	Attribute Has_Attributes::createFixedLengthString(
		const std::string& attrname,
		const std::string& data,
		HH_hid_t , // AttributeCreationPlist,
		HH_hid_t ) //AttributeAccessPlist)
	{
		return Almost_Attribute_Fixed_String::createFixedLengthString(base, attrname, data.size());
	}

	void Has_Attributes::rename(const std::string& oldName, const std::string& newName) const
	{
		auto ret = H5Arename(base(), oldName.c_str(), newName.c_str());
		if (ret < 0) throw HH_throw;
	}

	void AttributeParameterPack::apply(HH::HH_hid_t& d) const {
		for (auto& natt : newAtts)
			natt->apply(d);
	}

	/// Create and write a fixed-length string attribute.
	AttributeParameterPack& AttributeParameterPack::addFixedLengthString(
		const std::string & attrname,
		const std::string & data
	)
	{
		newAtts.push_back(std::shared_ptr<Almost_Attribute_Fixed_String>(
			new Almost_Attribute_Fixed_String(attrname, data)));
		return *this;
	}

}
