#include "icedb/exv.hpp"
#include "icedb/versioning/versioningForwards.hpp"
#include "icedb/error.hpp"
#include "HH/Attributes.hpp"
#include "HH/Types/Complex.hpp"

namespace icedb {
	namespace registry {
		template struct IO_class_registry_writer <
			::icedb::exv::EXV >;
		template struct IO_class_registry_reader <
			::icedb::exv::NewEXVrequiredProperties >;
		template class usesDLLregistry <
			::icedb::exv::_impl::EXVProps_IO_Input_Registry,
			IO_class_registry_reader<::icedb::exv::NewEXVrequiredProperties> >;
		template class usesDLLregistry <
			::icedb::exv::_impl::EXV_IO_Output_Registry,
			IO_class_registry_writer<::icedb::exv::EXV> >;
	}
	namespace exv {
		struct RIs {
			int constituent_id;
			std::string substance_name;
			const char* substance_name_c;
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
			// TODO: Fix this one.
			//H5Tinsert(typ, "substance_name", HOFFSET(RIs, substance_name_c),
			//	GetHDF5Type<decltype(RIs::substance_name_c)>()());
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

		size_t NewEXVrequiredProperties::ScattProps::get_unidimensional_index(float rot1, float rot2, float rot3, float i_p, float i_a,
			float s_p, float s_a, size_t asm_rank) const
		{

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
				ri.substance_name_c = ri.substance_name.c_str();
				ri.m = std::get<2>(r);
				ris.push_back(ri);
			}
			// TODO: Teach HH how to write this type of object.
			// TODO: Teach HH how to write complex numbers.
			auto dRIs = res.dsets.create<RIs>("Constituent_Refractive_Indices", { ris.size() });
			Expects(0 <= dRIs.write<RIs>(ris));

			/// \todo Add in rotation scheme information!
			/// \todo Add in rotation information!
			auto g_sps = res.create("Scattering_Properties");
			for (size_t spnum = 0; spnum < data->scattering_properties.size(); ++spnum) {
				// Generate a name like 030.
				std::ostringstream sspid;
				sspid.fill('0');
				size_t newwidth = (size_t) std::log10((double)data->scattering_properties.size()) + 1;
				sspid.width(newwidth);
				sspid << spnum;
				std::string spid = sspid.str();

				// Write to the named group.
				auto g = g_sps.create(spid.c_str());

				const auto &s = data->scattering_properties[spnum];

				g.atts.add("Rotation_Scheme", s.rotation_scheme);
				std::vector<string> Rotation_Names(3);
				if (s.rotation_scheme == icedb::exv::NewEXVrequiredProperties::ScattProps::Rotation_Scheme::DDSCAT)
					Rotation_Names = { "Theta", "Phi", "Beta" };
				else if (s.rotation_scheme == icedb::exv::NewEXVrequiredProperties::ScattProps::Rotation_Scheme::EULER)
					Rotation_Names = { "Alpha", "Beta", "Gamma" };
				else {
					ICEDB_throw(error::error_types::xUnimplementedFunction)
						.add("Reason", "Unimplemented write for this rotation scheme.");
				}
				auto d_rot1 = g.dsets.createFromSpan(Rotation_Names[0].c_str(), gsl::make_span(s.rot1))
					.setIsDimensionScale(Rotation_Names[0]).AddSimpleAttributes("units","degrees");
				auto d_rot2 = g.dsets.createFromSpan(Rotation_Names[1].c_str(), gsl::make_span(s.rot2))
					.setIsDimensionScale(Rotation_Names[1]).AddSimpleAttributes("units", "degrees");
				auto d_rot3 = g.dsets.createFromSpan(Rotation_Names[2].c_str(), gsl::make_span(s.rot3))
					.setIsDimensionScale(Rotation_Names[2]).AddSimpleAttributes("units", "degrees");

				auto d_ia = g.dsets.createFromSpan("incident_azimuth_angle", gsl::make_span(s.incident_azimuth_angle))
					.setIsDimensionScale("incident_azimuth_angle").AddSimpleAttributes("units", "degrees");
				d_ia.AddSimpleAttributes("long_name", "incident azimuth angle of the radiation in the particle reference frame", "units", "degrees");

				auto d_ip = g.dsets.createFromSpan("incident_polar_angle", gsl::make_span(s.incident_polar_angle))
					.setIsDimensionScale("incident_polar_angle").AddSimpleAttributes("units", "degrees");
				d_ip.AddSimpleAttributes("long_name", "incident polar angle of the radiation in the particle reference frame", "units", "degrees");

				auto d_sa = g.dsets.createFromSpan("scattering_azimuth_angle", gsl::make_span(s.scattering_azimuth_angle))
					.setIsDimensionScale("scattering_azimuth_angle").AddSimpleAttributes("units", "degrees");
				d_sa.AddSimpleAttributes("long_name", "scattering azimuth angle of the radiation in the particle reference frame", "units", "degrees");

				auto d_sp = g.dsets.createFromSpan("scattering_polar_angle", gsl::make_span(s.scattering_polar_angle))
					.setIsDimensionScale("scattering_polar_angle").AddSimpleAttributes("units", "degrees");
				d_sp.AddSimpleAttributes("long_name", "scattering polar angle of the radiation in the particle reference frame", "units", "degrees");

				const std::vector<uint16_t> vscs{ 11, 12, 21, 22 };
				auto d_scs = res.dsets.createFromSpan<uint16_t>("Scattering_Coefficient_Indices", vscs);
				d_scs.setIsDimensionScale("Scattering_Coefficient_Indices");
				d_scs.AddSimpleAttributes("long_name", "Indices of the amplitude scattering matrix element [following Bohren and Huffman (1983)]");

				//auto d_asm = g.dsets.createFromSpan(
				//	"amplitude_scattering_matrix", gsl::make_span(s.amplitude_scattering_matrix), )
				//	.setDims({ d_rot1,d_rot2,d_rot3,d_ia,d_ip,d_sa,d_sp,d_scs });
				auto d_s = res.dsets.create<std::complex<double>>("amplitude_scattering_matrix",
					{ s.rot1.size(), s.rot2.size(), s.rot3.size(),
					s.incident_azimuth_angle.size(), s.incident_polar_angle.size(),
					s.scattering_azimuth_angle.size(), s.scattering_polar_angle.size(), 
					vscs.size() });
				d_s.setDims({ d_rot1, d_rot2, d_rot3, d_ia, d_sa, d_ip, d_sp, d_scs });
				d_s.AddSimpleAttributes("long_name", "Amplitude scattering matrix [following Bohren and Huffman (1983)]", "units", "dimensionless");

				Expects(0 <= d_s.write<std::complex<double>>(s.amplitude_scattering_matrix));
			}

			return EXV(res.get());
		}
	}
}
