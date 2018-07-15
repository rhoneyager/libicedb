#include "../icedb/exv.hpp"
#include "../icedb/versioning/versioning.hpp"
#include "../icedb/error.hpp"

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
			struct RIs {
				int constituent_id;
				std::string substance_name;
				std::complex<double> m;
			};
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
			const size_t numScattEntries = data->angles.size();
			// TODO: Finish the implementation. Create a dataset for
			// each of the output fields.
			// Incid_azi, incid_pol, scatt_azi, scatt_pol
			// amplitude scatt matrix has s1-4 as columns, and type is complex<double>
			// TODO: check compressibility

		}
	}
}
