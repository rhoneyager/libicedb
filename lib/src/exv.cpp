#include "../icedb/exv.hpp"
#include "../icedb/versioning/versioning.hpp"
#include "../icedb/error.hpp"
#include "../icedb/compat/HH/Attributes.hpp"
#include "../icedb/compat/HH/Types/Complex.hpp"

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
			if (!aOType.isOfType<std::string>()) return false;
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
				std::vector<float> v_geom_angles(numScattEntries * 3);
				std::vector<float> v_i_azi(numScattEntries), v_i_pol(numScattEntries),
					v_s_azi(numScattEntries), v_s_pol(numScattEntries);
				std::vector<std::complex<double> >
					s(numScattEntries*4);

				for (size_t i = 0; i < numScattEntries; ++i) {
					v_i_azi[i] = data->angles[i].incident_azimuth_angle;
					v_i_pol[i] = data->angles[i].incident_polar_angle;
					v_s_azi[i] = data->angles[i].scattering_azimuth_angle;
					v_s_pol[i] = data->angles[i].scattering_polar_angle;
					s[(4 * i) + 0] = data->angles[i].amplitude_scattering_matrix[0];
					s[(4 * i) + 1] = data->angles[i].amplitude_scattering_matrix[1];
					s[(4 * i) + 2] = data->angles[i].amplitude_scattering_matrix[2];
					s[(4 * i) + 3] = data->angles[i].amplitude_scattering_matrix[3];
					v_geom_angles[(3 * i) + 0] = data->angles[i].alpha;
					v_geom_angles[(3 * i) + 1] = data->angles[i].beta;
					v_geom_angles[(3 * i) + 2] = data->angles[i].gamma;
				}
				auto d_geom_angles = res.dsets.create<float>("angles", { numScattEntries, 3 });
				Expects(0 <= d_geom_angles.write<float>(v_geom_angles));
				auto d_i_azi = res.dsets.create<float>("incident_azimuth_angle", { numScattEntries });
				Expects(0 <= d_i_azi.write<float>(v_i_azi));
				auto d_s_azi = res.dsets.create<float>("scattering_azimuth_angle", { numScattEntries });
				Expects(0 <= d_s_azi.write<float>(v_s_azi));
				auto d_i_pol = res.dsets.create<float>("incident_polar_angle", { numScattEntries });
				Expects(0 <= d_i_pol.write<float>(v_i_pol));
				auto d_s_pol = res.dsets.create<float>("scattering_polar_angle", { numScattEntries });
				Expects(0 <= d_s_pol.write<float>(v_s_pol));

				auto d_s = res.dsets.create<std::complex<double>>("amplitude_scattering_matrix", { numScattEntries, 4 });
				Expects(0 <= d_s.write<std::complex<double>>(s));

				// TODO: Need to serialize complex number writes.

				//auto ctype = HH::Types::GetHDF5Type<std::complex<double> >();
			}
			return EXV(res.get());
		}
	}
}
