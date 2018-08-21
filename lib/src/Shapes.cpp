#include <string>
#include <sstream>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include "../icedb/shape.hpp"
#include "../icedb/versioning/versioning.hpp"
#include "../icedb/error.hpp"

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

		Shape::~Shape() {}
		const std::string Shape::_icedb_obj_type_shape_identifier = "shape";
		const uint16_t Shape::_icedb_current_shape_schema_version = 0;

		/// \todo BUG: This primarily checks if the shape was _written_ _by_ _icedb_. Fix.
		bool Shape::isShape(HH::HH_hid_t group) {
			// Get link type. If this is a group, then check the attributes.
			H5O_info_t oinfo;
			herr_t err = H5Oget_info(group(), &oinfo);
			if (err < 0) return false;
			if (oinfo.type != H5O_type_t::H5O_TYPE_GROUP) return false;

			HH::Group g(group);

			if (!g.atts.exists("_icedb_obj_type")) return false;
			auto aType = g.atts["_icedb_obj_type"];
			if (H5Tget_class(aType.type()()) != H5T_STRING) return false;
			std::string sOType = aType.read<std::string>();
			if (sOType != Shape::_icedb_obj_type_shape_identifier) return false;
			return true;
		}
		bool Shape::isShape() const {
			return isShape(get());
		}

		bool Shape::isValid(std::ostream *out) const { return isValid(this->get(), out); }

		bool Shape::isValid(HH::HH_hid_t gid, std::ostream *out) {
			/// Check for the existence of the standard tables, dimensions and attributes, and that
			/// they have the appropriate sizes.

			/// \todo Finish this, and turn checks on attributes and variables into template functions!!!

			bool good = true;
			if (!isShape(gid)) {
				good = false;
				if (out) (*out) << "This is not a valid shape. Missing the appropriate icedb identifying attribute." << std::endl;
				return good;
			}
			auto group = HH::Group(gid);

			if (!group.atts.exists("particle_id")) {
				good = false;
				if (out) (*out) << "Missing the particle_id attribute." << std::endl;
			}
			else if (H5Tget_class(group.atts["particle_id"].type()()) != H5T_STRING) {
				good = false;
				if (out) (*out) << "The particle_id attribute has the wrong type." << std::endl;
			}
			if (out) (*out) << "TODO: Finish these checks!" << std::endl;
			return good;
		}

		Shape Shape::createShape(
			HH::HH_hid_t baseGrpID,
			gsl::not_null<const char*> shapeGrp,
			gsl::not_null<const NewShapeProperties*> props)
		{
			Group base(baseGrpID);
			if (base.exists(shapeGrp)) {
				const auto grp = base.open(shapeGrp);
				return createShape(grp.get(), props);
			}
			else {
				const auto grp = base.create(shapeGrp);
				return createShape(grp.get(), props);
			}
		}

		Shape Shape::createShape(
			HH::HH_hid_t newLocationAsEmptyGroup,
			gsl::not_null<const NewShapeProperties*> props)
		{
			// Check for validity. If a shape entry is invalid, throw an exception
			// to the user.
			if (!props->isValid(&(std::cerr)))
				ICEDB_throw(error::error_types::xBadInput)
				.add("Reason", "Cannot create a shape from the passed data. See above error "
					"messages regarding why the shape is invalid.")
				.add("Particle-id", props->particle_id);

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
			res.atts.add<std::string>("_icedb_obj_type", Shape::_icedb_obj_type_shape_identifier);
			res.atts.add<uint16_t>("_icedb_shape_schema_version", Shape::_icedb_current_shape_schema_version);
			res.atts.add<std::string>("particle_id", props->particle_id);
			res.atts.add<string>("dataset_id", props->dataset_id);
			res.atts.add<string>("author", props->author);
			res.atts.add<string>("contact", props->contact);
			res.atts.add<unsigned int>("version", { props->version[0], props->version[1], props->version[2] }, { 3 });


			using namespace HH::Tags;
			using namespace HH::Tags::PropertyLists;
			const size_t numScattElems = (props->particle_scattering_element_coordinates_as_floats.size())
				? props->particle_scattering_element_coordinates_as_floats.size()
				: props->particle_scattering_element_coordinates_as_ints.size();
			constexpr size_t max_x = 40000;
			const std::vector<hsize_t> chunks2d{
				(max_x < numScattElems) ?
				max_x : numScattElems, 3 };

			auto pl2d = HH::PL::PL::createDatasetCreation().setDatasetCreationPList<uint64_t>(
				//t_CompressionType(HH::PL::CompressionType::ANY)
				t_Chunking({ chunks2d[0], chunks2d[1] })
				);
			auto pl1d = pl2d.clone().setDatasetCreationPList<uint64_t>(t_Chunking({ chunks2d[0] }));

			// Write required dimensions
			auto tblPSEN = res.dsets.create<int32_t>(
				"particle_scattering_element_number",
				{ static_cast<size_t>(numScattElems) });
			{
				tblPSEN.atts.add<std::string>("description", "ID number of scattering element");
				tblPSEN.atts.add<std::string>("units", "None");

				if (!props->particle_scattering_element_number.empty()) {
					Expects(0 <= tblPSEN.write<int32_t>(props->particle_scattering_element_number));
				}
				else {
					// Create "dummy" element numbers and write.
					std::vector<int64_t> dummyPSENs(numScattElems);
					for (size_t i = 0; i < numScattElems; ++i)
						dummyPSENs[i] = i + 1;
					Expects(0 <= tblPSEN.write<int64_t>(dummyPSENs));
				}
				// NOTE: The HDF5 dimension scale specification explicitly allows for dimensions to not have assigned values.
				// However, netCDF should have these values.
				tblPSEN.setIsDimensionScale("particle_scattering_element_number");
			}

			auto tblPCN = res.dsets.create<uint16_t>("particle_constituent_number",
				{ static_cast<size_t>(props->particle_constituents.size()) });
			{
				tblPCN.atts.add<std::string>("description", "ID number of the constituent material");
				std::vector<uint16_t> data_pcns(props->particle_constituents.size());
				std::vector<std::string> data_pcn_names(props->particle_constituents.size());
				for (size_t i = 0; i < props->particle_constituents.size(); ++i) {
					data_pcns[i] = props->particle_constituents[i].first;
					data_pcn_names[i] = props->particle_constituents[i].second;
				}
				Expects(0 <= tblPCN.write<uint16_t>(data_pcns));
				tblPCN.setIsDimensionScale("particle_constituent_number");

				auto tblPCNnames = res.dsets.create<string>("particle_constituent_name",
					{ static_cast<size_t>(props->particle_constituents.size()) });
				Expects(0 <= tblPCNnames.write<string>(data_pcn_names));
				tblPCNnames.setDims(tblPCN);
			}

			auto tblXYZ = res.dsets.create<uint16_t>("particle_axis", { 3 });
			Expects(0 <= tblXYZ.write<uint16_t>({ 0, 1, 2 }));
			tblXYZ.setIsDimensionScale("particle_axis");

			HH::Dataset tblPSEC(HH::HH_hid_t::dummy()); // = res.dsets.create<uint8_t>("particle_axis", { 3 });
			if (props->particle_scattering_element_coordinates_as_ints.size()) {
				typedef decltype(props->particle_scattering_element_coordinates_as_ints)::value_type int_type;
				tblPSEC = res.dsets.create<int_type>(
					t_name("particle_scattering_element_coordinates"),
					t_dimensions({ static_cast<size_t>(numScattElems), 3 }),
					t_DatasetCreationPlist(pl2d())
					);
				Expects(0 <= tblPSEC.write<int_type>(props->particle_scattering_element_coordinates_as_ints));

			}
			else {
				tblPSEC = res.dsets.create<float>(
					t_name("particle_scattering_element_coordinates"),
					t_dimensions({ static_cast<size_t>(numScattElems), 3 }),
					t_DatasetCreationPlist(pl2d())
					);
				Expects(0 <= tblPSEC.write<float>(props->particle_scattering_element_coordinates_as_floats));
			}
			tblPSEC.setDims(tblPSEN, tblXYZ);
			tblPSEC.atts.add<std::string>("description", "Cartesian coordinates (x,y,z) of the center of the scattering element (dipole position, center of sphere, etc.)");
			tblPSEC.atts.add<std::string>("units", "None");
			//tblPSEC->writeAttribute<std::string>("comments", { 1 }, {
			//	"Equivalent to the coordinates given in a DDA shape-file (x,y,z-dimension) "
			//	"so scattering computations can be easily repeated with the same structure; "
			//	"for sphere methods the coordinates describe the center location of the sphere." });

			if (props->particle_scattering_element_composition_fractional.size()) {
				//const std::vector<size_t> cs{
				//	(max_x < required->number_of_particle_scattering_elements) ?
				//	max_x : required->number_of_particle_scattering_elements,
				//	static_cast<size_t>(required->number_of_particle_constituents)
				//};
				auto tblPSEC2a = res.dsets.create<float>(
					t_name("particle_scattering_element_composition_fractional"),
					t_dimensions({ static_cast<size_t>(numScattElems),
						static_cast<size_t>(props->particle_constituents.size()) }),
					t_DatasetCreationPlist(pl2d())
					);
				Expects(0 <= tblPSEC2a.write<float>(props->particle_scattering_element_composition_fractional));
				tblPSEC2a.setDims(tblPSEN, tblPCN);
				tblPSEC2a.atts.add<std::string>("description", "Mass fractions of each constituent for each scattering element.");
				tblPSEC2a.atts.add<std::string>("units", "None");
			}

			if (props->particle_scattering_element_composition_whole.size()) {
				auto tblPSEC2b = res.dsets.create<uint16_t>(
					t_name("particle_scattering_element_composition_whole"),
					t_dimensions({ static_cast<size_t>(numScattElems) }),
					t_DatasetCreationPlist(pl1d())
					);
				tblPSEC2b.setDims(tblPSEN);
				tblPSEC2b.atts.add<std::string>("description", "The constituent material ID for each scattering element.");
				tblPSEC2b.atts.add<std::string>("units", "None");
				Expects(0 <= tblPSEC2b.write<uint16_t>(props->particle_scattering_element_composition_whole));
			}

			if (props->scattering_element_coordinates_scaling_factor > 0) {
				res.atts.add<float>("scattering_element_coordinates_scaling_factor", props->scattering_element_coordinates_scaling_factor);
				//res.atts.add<std::string>("scattering_element_coordinates_scaling_factor__description", "Physical spacing between adjacent grid points");
			}
			if (props->scattering_element_coordinates_units.size())
				res.atts.add<std::string>("scattering_element_coordinates_units", props->scattering_element_coordinates_units);

			if (props->scattering_method.size())
				res.atts.add<string>("scattering_method", props->scattering_method);

			if (props->particle_scattering_element_radius.size()) {
				auto tblPSER = res.dsets.create<float>(
					t_name("particle_scattering_element_radius"),
					t_dimensions({ static_cast<size_t>(numScattElems) }),
					t_DatasetCreationPlist(pl1d())
					);
				Expects(0 <= tblPSER.write<float>(props->particle_scattering_element_radius));
				tblPSER.setDims(tblPSEN);
				tblPSER.atts.add<std::string>("description", "Physical radius of the scattering sphere.");
				tblPSER.atts.add<std::string>("units", "m");
				//tblPSER.atts.add<std::string>("comments", "TODO: Units are under discussion. Either in meters or dimensionless and scaled by particle_scattering_element_spacing." });
			}


			return Shape(res.get());
		}

	}
}
