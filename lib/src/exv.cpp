#include "../icedb/exv.hpp"
#include "../icedb/versioning/versioning.hpp"
#include "../icedb/error.hpp"

namespace icedb {
	namespace exv {
		struct RIs {
			int constituent_id;
			std::string substance_name;
			std::complex<double> m;
		};
	}
}
namespace HH {
	namespace Types {
		template <class ComplexDataType>
		HH_hid_t GetHDF5TypeComplex()
		{
			typedef ComplexDataType::value_type value_type;
			// Complex number is a compound datatype of two objects.
			return GetHDF5Type<value_type, 1>({ 2 });
		}

#define HH_SPECIALIZE_COMPLEX(x) \
template<> inline HH_hid_t GetHDF5Type< x,0 >(std::initializer_list<hsize_t>, void*) { return GetHDF5TypeComplex< x >(); }
		HH_SPECIALIZE_COMPLEX(std::complex<double>);
		HH_SPECIALIZE_COMPLEX(std::complex<float>);
#undef HH_SPECIALIZE_COMPLEX

		// Handles complex number reads and writes from/to HDF5.
		template <class ComplexDataType>
		struct Object_Accessor_Complex
		{
		private:
			typedef typename ComplexDataType::value_type value_type;
			std::vector<value_type > _buf;
		public:
			Object_Accessor_Complex(ssize_t sz = -1) {}
			/// \brief Converts an object into a void* array that HDF5 can natively understand.
			/// \note The shared_ptr takes care of "deallocation" when we no longer need the "buffer".
			const void* serialize(::gsl::span<const ComplexDataType> d)
			{
				_buf.resize(d.size() * 2);
				for (size_t i = 0; i < (size_t) d.size(); ++i) {
					_buf[(2 * i) + 0] = d.at(i).real();
					_buf[(2 * i) + 1] = d.at(i).imag();
				}
				return (const void*)_buf.data();
				//return std::shared_ptr<const void>((const void*)d.data(), [](const void*) {});
			}
			/// \brief Gets the size of the buffer needed to store the object from HDF5. Used
			/// in variable-length string / complex object reads.
			/// \note For POD objects, we do not have to allocate a buffer.
			/// \returns Size needed. If negative, then we can directly write to the object, 
			/// sans allocation or deallocation.
			ssize_t getFromBufferSize() {
				return -1;
			}
			/// \brief Allocates a buffer that HDF5 can read/write into; used later as input data for object construction.
			/// \note For POD objects, we can directly write to the object.
			void marshalBuffer(ComplexDataType * objStart) {}
			/// \brief Construct an object from an HDF5-provided data stream, 
			/// and deallocate any temporary buffer.
			/// \note For trivial (POD) objects, there is no need to do anything.
			void deserialize(ComplexDataType *objStart) {}
			void freeBuffer() {}
		};

		template<> struct Object_Accessor<std::complex<double> > : public Object_Accessor_Complex<std::complex<double> > {};
		template<> struct Object_Accessor<std::complex<float> > : public Object_Accessor_Complex<std::complex<float> > {};

		/// \todo I have to implement a custom accessor for this object! The string needs 
		/// to be marshalled. This function may have to be updated to work with the
		/// custom accessor.
		template<>
		HH_hid_t GetHDF5Type<icedb::exv::RIs,0>(std::initializer_list<hsize_t>, void*)
		{
			using icedb::exv::RIs;
			static HH_hid_t obj(-1);
			static bool inited = false;
			if (inited) return obj;

			hid_t typ = H5Tcreate(H5T_COMPOUND, sizeof(RIs));
			H5Tinsert(typ, "constituent_id", HOFFSET(RIs, constituent_id),
				GetHDF5Type<decltype(RIs::constituent_id)>()());
			H5Tinsert(typ, "substance_name", HOFFSET(RIs, substance_name),
				GetHDF5Type<decltype(RIs::substance_name)>()());
			H5Tinsert(typ, "m", HOFFSET(RIs, m),
				GetHDF5Type<decltype(RIs::m)>()());
			obj = HH_hid_t(typ);
			return obj;
		}
	}
}

namespace icedb {
	namespace exv {
		const std::string EXV::_icedb_obj_type_exv_identifier = "exv";
		const uint16_t EXV::_icedb_current_exv_schema_version = 1;

		bool EXV::isEXV(HH::HH_hid_t group) {
			HH::Group g(group);
			if (!g.isGroup()) return false;
			if (!g.atts.exists("_icedb_obj_type")) return false;
			if (!g.atts.exists("_icedb_exv_schema_version")) return false;

			auto aOType = g.atts.open("_icedb_obj_type");
			if (!aOType.IsOfType<std::string>()) return false;
			std::string sOType = aOType.read<std::string>();
			if (sOType != _icedb_obj_type_exv_identifier) return false;

			return true;
		}

		bool EXV::isValid(HH::HH_hid_t gid, std::ostream *out) {
			bool good = true;
			if (!isEXV(gid)) {
				good = false;
				if (out) (*out) << "This is not a valid exv structure. Missing the appropriate icedb identifying attributes." << std::endl;
				return good;
			}

			if (out) (*out) << "TODO: finish the isValid implementation for EXV objects." << std::endl;

			return good;
		}

		EXV EXV::createEXV(
			HH::HH_hid_t newLocationAsEmptyGroup,
			gsl::not_null<const NewEXVrequiredProperties*> data)
		{
			if (!data->isValid(&(std::cerr)))
				ICEDB_throw(error::error_types::xBadInput)
				.add("Reason", "Cannot create an exv structure from the passed data. See above error "
					"messages regarding why the input object is invalid.")
				.add("Particle-id", data->particle_id);

			// newLocationAsEmptyGroup
			HH::Group res(newLocationAsEmptyGroup);

			using std::string;
			// Write debug information
			{
				using namespace icedb::versioning;
				auto libver = getLibVersionInfo();
				string sLibVer(libver->vgithash);
				res.atts.add<string>("_icedb_git_hash", sLibVer);
				res.atts.add<uint64_t>("_icedb_version",
					{ libver->vn[versionInfo::V_MAJOR], libver->vn[versionInfo::V_MINOR], libver->vn[versionInfo::V_REVISION] }, { 3 });
			}

			// Write required attributes
			res.atts.add<std::string>("_icedb_obj_type", EXV::_icedb_obj_type_exv_identifier);
			res.atts.add<uint16_t>("_icedb_shape_schema_version", EXV::_icedb_current_exv_schema_version);
			res.atts.add<std::string>("particle_id", data->particle_id);
			res.atts.add<string>("dataset_id", data->dataset_id);
			res.atts.add<string>("author", data->author);
			res.atts.add<string>("contact", data->contact);
			res.atts.add<unsigned int>("version", { data->version[0], data->version[1], data->version[2] }, { 3 });

			res.atts.add("Temperature_K", data->temperature_K);
			res.atts.add("Frequency_Hz", data->frequency_Hz);

			// Refractive indices and substances
			
			std::vector<RIs> ris;
			for (const auto &r : data->constituent_refractive_indices) {
				RIs ri;
				ri.constituent_id = std::get<0>(r);
				ri.substance_name = std::get<1>(r);
				ri.m = std::get<2>(r);
				ris.push_back(ri);
			}
			// TODO: Teach HH how to write this type of object.
			// TODO: Teach HH how to write complex numbers.
			auto dRIs = res.dsets.create<RIs>("Constituent_Refractive_Indices", { ris.size() });
			Expects(0 <= dRIs.write<RIs>(ris));

			// Angular data
			// TODO: check compressibility of the complex data types
			const size_t numScattEntries = data->angles.size();
			if (numScattEntries) {
				std::vector<float> v_i_azi(numScattEntries), v_i_pol(numScattEntries),
					v_s_azi(numScattEntries), v_s_pol(numScattEntries);
				std::vector<std::complex<double> >
					s1(numScattEntries), s2(numScattEntries), s3(numScattEntries), s4(numScattEntries);

				for (size_t i = 0; i < numScattEntries; ++i) {
					v_i_azi[i] = data->angles[i].incident_azimuth_angle;
					v_i_pol[i] = data->angles[i].incident_polar_angle;
					v_s_azi[i] = data->angles[i].scattering_azimuth_angle;
					v_s_pol[i] = data->angles[i].scattering_polar_angle;
					s1[i] = data->angles[i].amplitude_scattering_matrix[0];
					s2[i] = data->angles[i].amplitude_scattering_matrix[1];
					s3[i] = data->angles[i].amplitude_scattering_matrix[2];
					s4[i] = data->angles[i].amplitude_scattering_matrix[3];
				}
				auto d_i_azi = res.dsets.create<float>("incident_azimuth_angle", { numScattEntries });
				Expects(0 <= d_i_azi.write<float>(v_i_azi));
				auto d_s_azi = res.dsets.create<float>("scattering_azimuth_angle", { numScattEntries });
				Expects(0 <= d_s_azi.write<float>(v_s_azi));
				auto d_i_pol = res.dsets.create<float>("incident_polar_angle", { numScattEntries });
				Expects(0 <= d_i_pol.write<float>(v_i_pol));
				auto d_s_pol = res.dsets.create<float>("scattering_polar_angle", { numScattEntries });
				Expects(0 <= d_s_pol.write<float>(v_s_pol));

				auto d_s1 = res.dsets.create<std::complex<double>>("amplitude_scattering_matrix_s1", { numScattEntries });
				Expects(0 <= d_s1.write<std::complex<double>>(s1));

				//auto ctype = HH::Types::GetHDF5Type<std::complex<double> >();
			}
			return EXV(res.get());
		}
	}
}
