#include "icedb/defs.h"
#include <string>
#include <sstream>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include "icedb/Constants.hpp"
#include "icedb/Shapes.hpp"
#include "icedb/error.hpp"
#include "icedb/PPP.hpp"
#include "icedb/ShapeAlgs.hpp"

namespace icedb {
	namespace registry {
		template struct IO_class_registry_writer <
			::icedb::PPP::PPP >;
		template class usesDLLregistry <
			::icedb::PPP::_impl::PPP_IO_Output_Registry,
			IO_class_registry_writer<::icedb::PPP::PPP> >;
	}
	namespace PPP {
		const std::string _obj_type_identifier = "ppp";
		const uint16_t PPP::_current_schema_version = 0;

		using CN = e_Common_Obj_Names;
		const ICEDB_DL std::map<CN, Name_Type_t > Required_Atts = {
			{CN::icedb_Version, {"_icedb_version", HH::Types::GetHDF5Type<uint16_t>()}},
			{CN::Schema_Version, {"_icedb_shape_schema_version", HH::Types::GetHDF5Type<uint16_t>()}},
			{CN::icedb_git_hash, {"_icedb_git_hash", HH::Types::GetHDF5Type<std::string>()}},
			{CN::particle_id, {"particle_id", HH::Types::GetHDF5Type<std::string>()}},
			{CN::dataset_id, {"dataset_id", HH::Types::GetHDF5Type<std::string>()}},
			{CN::author, {"author", HH::Types::GetHDF5Type<std::string>()}},
			{CN::contact, {"contact", HH::Types::GetHDF5Type<std::string>()}},
			{CN::version, {"version", HH::Types::GetHDF5Type<uint32_t>()}}
		};

		const ICEDB_DL std::map<CN, Name_Type_t > Required_Dsets = {
			{CN::particle_scattering_element_number, {"particle_scattering_element_number", HH::Types::GetHDF5Type<int32_t>()}},
			{CN::particle_constituent_number, {"particle_constituent_number", HH::Types::GetHDF5Type<uint16_t>()}},
			{CN::particle_axis, {"particle_axis", HH::Types::GetHDF5Type<uint16_t>()}}
		};

		PPP PPP::createPPP(
			HH::Group newLocationAsEmptyGroup,
			const HH::Group& gShape)
		{
			// Check for validity. If a shape entry is invalid, throw an exception
			// to the user.
			const Shapes::Shape srcShape(gShape);

			// newLocationAsEmptyGroup
			HH::Group res(newLocationAsEmptyGroup);

			using std::string;
			// Write debug information
			{
				res.atts.add<string>(Constants::AttNames::icedb_git_hash, Constants::GitHash);
				res.atts.add<uint64_t>(Constants::AttNames::icedb_version, { Constants::version[0], Constants::version[1], Constants::version[2] }, { 3 });
			}

			// Write required attributes
			res.atts.add<std::string>(Constants::AttNames::icedb_object_type, _obj_type_identifier);
			res.atts.add<uint16_t>(Required_Atts.at(CN::Schema_Version).first, PPP::_current_schema_version);
			
			/*res.atts.add<std::string>(Required_Atts.at(CN::particle_id).first, props.particle_id);
			res.atts.add<string>(Required_Atts.at(CN::dataset_id).first, props.dataset_id);
			res.atts.add<string>(Required_Atts.at(CN::author).first, props.author);
			res.atts.add<string>(Required_Atts.at(CN::contact).first, props.contact);
			res.atts.add<unsigned int>(Required_Atts.at(CN::version).first, { props.version[0], props.version[1], props.version[2] }, { 3 });
			*/

			return PPP(res.get());
		}

		PPP PPP::apply(
			const std::vector< std::reference_wrapper<const ShapeAlgs::Algorithms::Algorithm>> &alg,
			const HH::Group& shp)
		{
			for (const auto& a : alg)
				apply(a.get(), shp);
			return *this;
		}

		PPP PPP::apply(gsl::span<const ShapeAlgs::Algorithms::Algorithm> alg, const HH::Group& shp) {
			for (const auto& a : alg)
				apply(a, shp);
			return *this;
		}

		PPP PPP::apply(const ShapeAlgs::Algorithms::Algorithm& alg, const HH::Group& shp)
		{
			HH::Group g(get());
			gsl::span<const HH::Group> sg(&g, 1);
			alg.apply(*this, shp, sg);
			return *this;
		}


	}
}
