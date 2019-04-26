#include "icedb/defs.h"
#include <string>
#include <sstream>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include "icedb/Constants.hpp"
#include "icedb/Shapes.hpp"
#include "icedb/error.hpp"

namespace icedb {
	namespace registry {
		template struct IO_class_registry_writer <
			::icedb::Shapes::Shape >;
		template struct IO_class_registry_reader <
			::icedb::Shapes::NewShapeProperties >;
		template class usesDLLregistry <
			::icedb::Shapes::_impl::ShapeProps_IO_Input_Registry,
			IO_class_registry_reader<::icedb::Shapes::NewShapeProperties> >;
		template class usesDLLregistry <
			::icedb::Shapes::_impl::Shape_IO_Output_Registry,
			IO_class_registry_writer<::icedb::Shapes::Shape> >;
	}
	namespace Shapes {
		const std::string _obj_type_identifier = "shape";
		const uint16_t Shape::_current_schema_version = 0;

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
		const ICEDB_DL std::map<CN, Name_Type_t > Optional_Atts = {
			{CN::scattering_element_coordinates_scaling_factor, {"scattering_element_coordinates_scaling_factor", HH::Types::GetHDF5Type<float>()}},
			{CN::scattering_element_coordinates_units, {"scattering_element_coordinates_units", HH::Types::GetHDF5Type<std::string>()}},
			{CN::scattering_method, {"scattering_method", HH::Types::GetHDF5Type<std::string>()}}
		};
		//"scattering_method"
		const ICEDB_DL std::map<CN, Name_Type_t > Required_Dsets = {
			{CN::particle_scattering_element_number, {"particle_scattering_element_number", HH::Types::GetHDF5Type<int32_t>()}},
			{CN::particle_constituent_number, {"particle_constituent_number", HH::Types::GetHDF5Type<uint16_t>()}},
			{CN::particle_constituent_name, {"particle_constituent_name", HH::Types::GetHDF5Type<std::string>()}},
			{CN::particle_axis, {"particle_axis", HH::Types::GetHDF5Type<uint16_t>()}}
		};
		const ICEDB_DL std::map<CN, Name_Type_t > Optional_Dsets = {
			{CN::particle_scattering_element_coordinates_int, {"particle_scattering_element_coordinates", HH::Types::GetHDF5Type<decltype(NewShapeProperties::particle_scattering_element_coordinates_as_ints)::value_type>()}},
			{CN::particle_scattering_element_coordinates_float, {"particle_scattering_element_coordinates", HH::Types::GetHDF5Type<float>()}},
			{CN::particle_scattering_element_composition_fractional, {"particle_scattering_element_composition_fractional", HH::Types::GetHDF5Type<float>()}},
			{CN::particle_scattering_element_composition_whole, {"particle_scattering_element_composition_whole", HH::Types::GetHDF5Type<uint16_t>()}},
			{CN::particle_scattering_element_radius, {"particle_scattering_element_radius", HH::Types::GetHDF5Type<float>()}}
		};

		bool NewShapeProperties::isValid(std::ostream *out) const {
			bool good = true;

			if (!particle_scattering_element_coordinates_as_floats.size() &&
				!particle_scattering_element_coordinates_as_ints.size()) {
				good = false;
				if (out) (*out) << "The scattering elements are not set." << std::endl;
			}
			else if (particle_scattering_element_coordinates_as_floats.size() &&
				particle_scattering_element_coordinates_as_ints.size()) {
				good = false;
				if (out) (*out) << "The scattering elements must be either floats or ints, not both." << std::endl;
			}
			else if (particle_scattering_element_coordinates_as_floats.size()) {
				if (particle_scattering_element_coordinates_as_floats.size() % 3)
				{
					good = false;
					if (out) (*out) << "particle_scattering_element_coordinates_as_floats's size needs to be a multiple of 3 (X, Y, Z)." << std::endl;
				}
			}
			else if (particle_scattering_element_coordinates_as_ints.size()) {
				if (particle_scattering_element_coordinates_as_ints.size() % 3)
				{
					good = false;
					if (out) (*out) << "particle_scattering_element_coordinates_as_ints's size needs to be a multiple of 3 (X, Y, Z)." << std::endl;
				}
			}
			if (!particle_constituents.size()) {
				good = false;
				if (out) (*out) << "The particle constituents are not set." << std::endl;
			}
			if (!particle_id.size()) {
				good = false;
				if (out) (*out) << "Particle ID is not set." << std::endl;
			}

			if (this->particle_scattering_element_number.empty()) {
				good = false;
				if (out) (*out) << "particle_scattering_element_coordinates is not set. "
					"Particles need to have scattering elements." << std::endl;
			}
			else {
				size_t crdsSize = particle_scattering_element_coordinates_as_ints.size();
				if (particle_scattering_element_coordinates_as_ints.size() == 0)
					crdsSize = particle_scattering_element_coordinates_as_floats.size();
				if ((crdsSize / 3) != this->particle_scattering_element_number.size()) {
					good = false;
					if (out) (*out) << "particle_scattering_element_coordinates has "
						"the wrong size. It should have dimensions of "
						"[number_of_particle_scattering_elements][3], yielding a total of "
						<< particle_scattering_element_number.size() * 3 << " elements. "
						"Instead, it currently has " << crdsSize
						<< " elements." << std::endl;
				}
			}

			if (this->particle_scattering_element_composition_whole.empty()
				&& this->particle_scattering_element_composition_fractional.empty())
			{
				good = false;
				if (out) (*out) << "particle_scattering_element_composition_whole and "
					"particle_scattering_element_composition_fractional are not set, but one is required. "
					<< std::endl;
			}
			if (!this->particle_scattering_element_composition_whole.empty()
				&& !this->particle_scattering_element_composition_fractional.empty())
			{
				good = false;
				if (out) (*out) << "particle_scattering_element_composition_whole and "
					"particle_scattering_element_composition_fractional are both set, but only one is allowed. "
					<< std::endl;
			}
			if (!this->particle_scattering_element_composition_whole.empty()) {
				if (this->particle_scattering_element_composition_whole.size()
					!= particle_scattering_element_number.size())
				{
					good = false;
					if (out) (*out) << "particle_scattering_element_composition_whole "
						"has the wrong size. It should have a size of number_of_particle_scattering_elements."
						<< std::endl;
				}
			}
			if (!this->particle_scattering_element_composition_fractional.empty()) {
				if (this->particle_scattering_element_composition_fractional.size() !=
					(particle_scattering_element_number.size() * particle_constituents.size())) {
					good = false;
					if (out) (*out) << "particle_scattering_element_composition_fractional has "
						"the wrong dimensions. It should have dimensions of "
						"[number_of_particle_scattering_elements][number_of_particle_constituents], yielding a total of "
						<< particle_scattering_element_number.size() * particle_constituents.size() << " elements. "
						"Instead, it currently has " << this->particle_scattering_element_composition_fractional.size()
						<< " elements." << std::endl;
				}
			}

			if (this->particle_scattering_element_radius.size()) {
				if (this->particle_scattering_element_radius.size() != particle_scattering_element_number.size()) {
					good = false;
					if (out) (*out) << "particle_scattering_element_radius has the wrong size. "
						"It should have dimensions of [number_of_particle_scattering_elements]. "
						"particle_scattering_element_radius has a current size of " << particle_scattering_element_radius.size()
						<< ", and this should be " << particle_scattering_element_number.size()
						<< "." << std::endl;
				}
			}

			return good;
		}

		Shape Shape::createShape(
			HH::Group newLocationAsEmptyGroup,
			const NewShapeProperties& props)
		{
			// Check for validity. If a shape entry is invalid, throw an exception
			// to the user.
			if (!props.isValid(&(std::cerr)))
				ICEDB_throw(error::error_types::xBadInput)
				.add("Reason", "Cannot create a shape from the passed data. See above error "
					"messages regarding why the shape is invalid.")
				.add("Particle-id", props.particle_id);

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
			res.atts.add<uint16_t>(Required_Atts.at(CN::Schema_Version).first, Shape::_current_schema_version);
			res.atts.add<std::string>(Required_Atts.at(CN::particle_id).first, props.particle_id);
			res.atts.add<string>(Required_Atts.at(CN::dataset_id).first, props.dataset_id);
			res.atts.add<string>(Required_Atts.at(CN::author).first, props.author);
			res.atts.add<string>(Required_Atts.at(CN::contact).first, props.contact);
			res.atts.add<unsigned int>(Required_Atts.at(CN::version).first, { props.version[0], props.version[1], props.version[2] }, { 3 });

			const size_t numScattElems = (props.particle_scattering_element_coordinates_as_floats.size())
				? props.particle_scattering_element_coordinates_as_floats.size() / 3
				: props.particle_scattering_element_coordinates_as_ints.size() / 3;
			constexpr size_t max_x = 40000;
			
			auto Chunking = [&max_x, &numScattElems](const std::vector<hsize_t> & in, std::vector<hsize_t> & out) -> bool
			{
				out = in;
				out = std::vector<hsize_t>{ (max_x < numScattElems) ? max_x : numScattElems, 3 };
				return true;
			};
			HH::DatasetParameterPack dp_ui64;
			dp_ui64.fChunkingStrategy = Chunking;
			//dp_ui64.datasetCreationProperties.setFill<uint64_t>(Constants::default_val<float>());

			// Write required dimensions
			auto tblPSEN = res.dsets.create<int32_t>(
				Required_Dsets.at(CN::particle_scattering_element_number).first,
				{ static_cast<size_t>(numScattElems) });
			{
				tblPSEN.atts.add<std::string>("description", "ID number of scattering element");
				tblPSEN.atts.add<std::string>("units", "None");

				if (!props.particle_scattering_element_number.empty()) {
					tblPSEN.write<int32_t>(props.particle_scattering_element_number);
				}
				else {
					// Create "dummy" element numbers and write.
					std::vector<int64_t> dummyPSENs(numScattElems);
					for (size_t i = 0; i < numScattElems; ++i)
						dummyPSENs[i] = i + 1;
					tblPSEN.write<int64_t>(dummyPSENs);
				}
				// NOTE: The HDF5 dimension scale specification explicitly allows for dimensions to not have assigned values.
				// However, netCDF should have these values.
				tblPSEN.setIsDimensionScale("particle_scattering_element_number");
			}

			auto tblPCN = res.dsets.create<uint16_t>(
				Required_Dsets.at(CN::particle_constituent_number).first,
				{ static_cast<size_t>(props.particle_constituents.size()) });
			{
				tblPCN.atts.add<std::string>("description", "ID number of the constituent material");
				std::vector<uint16_t> data_pcns(props.particle_constituents.size());
				std::vector<std::string> data_pcn_names(props.particle_constituents.size());
				for (size_t i = 0; i < props.particle_constituents.size(); ++i) {
					data_pcns[i] = props.particle_constituents[i].first;
					data_pcn_names[i] = props.particle_constituents[i].second;
				}
				tblPCN.write<uint16_t>(data_pcns);
				tblPCN.setIsDimensionScale("particle_constituent_number");

				auto tblPCNnames = res.dsets.create<string>(Required_Dsets.at(CN::particle_constituent_name).first,
					{ static_cast<size_t>(props.particle_constituents.size()) });
				tblPCNnames.write<string>(data_pcn_names);
				tblPCNnames.setDims(tblPCN);
			}

			auto tblXYZ = res.dsets.create<uint16_t>(Required_Dsets.at(CN::particle_axis).first, { 3 });
			tblXYZ.write<uint16_t>({ 0, 1, 2 });
			tblXYZ.setIsDimensionScale("particle_axis");

			HH::Dataset tblPSEC(HH::HH_hid_t::dummy()); // = res.dsets.create<uint8_t>("particle_axis", { 3 });
			if (props.particle_scattering_element_coordinates_as_ints.size()) {
				// Shape coordinates are integers
				typedef decltype(props.particle_scattering_element_coordinates_as_ints)::value_type int_type;
				tblPSEC = res.dsets.create<int_type>(
					Optional_Dsets.at(CN::particle_scattering_element_coordinates_int).first,
					{ static_cast<size_t>(numScattElems), 3 })
					;
				tblPSEC.write<int_type>(props.particle_scattering_element_coordinates_as_ints);

			}
			else {
				// Shape coordinates are floats
				tblPSEC = res.dsets.create<float>(
					Optional_Dsets.at(CN::particle_scattering_element_coordinates_float).first,
					{ static_cast<size_t>(numScattElems), 3 }
					);
				tblPSEC.write<float>(props.particle_scattering_element_coordinates_as_floats);
			}
			tblPSEC.setDims(tblPSEN, tblXYZ);
			tblPSEC.atts.add<std::string>("description", "Cartesian coordinates (x,y,z) of the center of the scattering element (dipole position, center of sphere, etc.)");
			tblPSEC.atts.add<std::string>("units", "None");
			//tblPSEC->writeAttribute<std::string>("comments", { 1 }, {
			//	"Equivalent to the coordinates given in a DDA shape-file (x,y,z-dimension) "
			//	"so scattering computations can be easily repeated with the same structure; "
			//	"for sphere methods the coordinates describe the center location of the sphere." });

			if (props.particle_scattering_element_composition_fractional.size()) {
				//const std::vector<size_t> cs{
				//	(max_x < required->number_of_particle_scattering_elements) ?
				//	max_x : required->number_of_particle_scattering_elements,
				//	static_cast<size_t>(required->number_of_particle_constituents)
				//};
				auto tblPSEC2a = res.dsets.create<float>(
					Optional_Dsets.at(CN::particle_scattering_element_composition_fractional).first,
					{ static_cast<size_t>(numScattElems),
						static_cast<size_t>(props.particle_constituents.size()) }
					);
				tblPSEC2a.write<float>(props.particle_scattering_element_composition_fractional);
				tblPSEC2a.setDims(tblPSEN, tblPCN);
				tblPSEC2a.atts.add<std::string>("description", "Mass fractions of each constituent for each scattering element.");
				tblPSEC2a.atts.add<std::string>("units", "None");
			}

			if (props.particle_scattering_element_composition_whole.size()) {
				auto tblPSEC2b = res.dsets.create<uint16_t>(
					Optional_Dsets.at(CN::particle_scattering_element_composition_whole).first,
					{ static_cast<size_t>(numScattElems) }
					);
				tblPSEC2b.setDims(tblPSEN);
				tblPSEC2b.atts.add<std::string>("description", "The constituent material ID for each scattering element.");
				tblPSEC2b.atts.add<std::string>("units", "None");
				tblPSEC2b.write<uint16_t>(props.particle_scattering_element_composition_whole);
			}

			res.atts.add<float>(Optional_Atts.at(CN::scattering_element_coordinates_scaling_factor).first, props.scattering_element_coordinates_scaling_factor);
			res.atts.add<std::string>(Optional_Atts.at(CN::scattering_element_coordinates_units).first, props.scattering_element_coordinates_units);

			if (props.scattering_method.size())
				res.atts.add<string>(Optional_Atts.at(CN::scattering_method).first, props.scattering_method);

			if (props.particle_scattering_element_radius.size()) {
				auto tblPSER = res.dsets.create<float>(
					Optional_Dsets.at(CN::particle_scattering_element_radius).first,
					{ static_cast<size_t>(numScattElems) }
					);
				tblPSER.write<float>(props.particle_scattering_element_radius);
				tblPSER.setDims(tblPSEN);
				tblPSER.atts.add<std::string>("description", "Physical radius of the scattering sphere.");
				tblPSER.atts.add<std::string>("units", "m");
				//tblPSER.atts.add<std::string>("comments", "TODO: Units are under discussion. Either in meters or dimensionless and scaled by particle_scattering_element_spacing." });
			}


			return Shape(res.get());
		}

	}
}
