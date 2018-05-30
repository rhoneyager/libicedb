#include <string>
#include <sstream>
#include <iostream>
#include "../icedb/shape.hpp"
#include "../icedb/versioning/versioning.hpp"
#include "../private/hdf5_supplemental.hpp"
#include "../private/Shape_impl.hpp"

namespace icedb {
	namespace Shapes {
		bool NewShapeRequiredProperties::isValid(std::ostream *out) const {
			bool good = true;

			if (number_of_particle_scattering_elements == 0) {
				good = false;
				if (out) (*out) << "The number of scattering elements is not set." << std::endl;
			}
			if (number_of_particle_constituents == 0) {
				good = false;
				if (out) (*out) << "The number of particle constituents is not set." << std::endl;
			}
			if (!particle_id.size()) {
				good = false;
				if (out) (*out) << "Particle ID is not set." << std::endl;
			}
			if (!dataset_id.size()) {
				good = false;
				if (out) (*out) << "Dataset ID is not set." << std::endl;
			}
			
			if (this->particle_scattering_element_coordinates.empty()) {
				good = false;
				if (out) (*out) << "particle_scattering_element_coordinates is not set. "
					"Particles need to have scattering elements." << std::endl;
			}
			else if (particle_scattering_element_coordinates.size() != (3 * number_of_particle_scattering_elements)) {
				good = false;
				if (out) (*out) << "particle_scattering_element_coordinates has "
					"the wrong dimensions. It should have dimensions of "
					"[number_of_particle_scattering_elements][3], yielding a total of "
					<< 3 * number_of_particle_scattering_elements << " elements. "
					"Instead, it currently has " << particle_scattering_element_coordinates.size()
					<< " elements." << std::endl;
			}
			if (this->number_of_particle_constituents == 0) {
				good = false;
				if (out) (*out) << "number_of_particle_constituents is not set. "
					"Particles need to have a composition." << std::endl;
			}
			if (this->particle_scattering_element_coordinates_scaling_factor <= 0) {
				good = false;
				if (out) (*out) << "particle_scattering_element_coordinates_scaling_factor is < 0. " << std::endl;
			}
			if (this->particle_scattering_element_coordinates_units == "") {
				good = false;
				if (out) (*out) << "particle_scattering_element_coordinates_units must be set. " << std::endl;
			}
			
			return good;
		}

		bool NewShapeRequiredProperties::requiresOptionalPropertiesStruct() const {
			bool req = false;
			if (this->number_of_particle_constituents > 1) req = true;
			return req;
		}

		bool NewShapeCommonOptionalProperties::isValid(
			gsl::not_null<const NewShapeRequiredProperties*> required, std::ostream *out) const
		{
			bool good = true;

			if (!this->particle_scattering_element_number.empty()) {
				if (required->number_of_particle_scattering_elements != this->particle_scattering_element_number.size()) {
					good = false;
					if (out) (*out) << "number_of_particle_scattering_elements is not equal to particle_scattering_element_number." << std::endl;
				}
			}
			if (required->number_of_particle_constituents > 1) {
				if (this->particle_constituent_number.empty() || this->particle_constituent_number.size() != required->number_of_particle_constituents) {
					good = false;
					if (out) (*out) << "particle_constituent_number is not set. "
						"This is an essential dimension scale that the rest of the particle "
						"data depends on." << std::endl;
				}

				
				if (!this->particle_scattering_element_composition.empty()) {
					if (this->particle_scattering_element_composition.size() !=
						(required->number_of_particle_scattering_elements * required->number_of_particle_constituents)) {
						good = false;
						if (out) (*out) << "particle_scattering_element_composition has "
							"the wrong dimensions. It should have dimensions of "
							"[number_of_particle_scattering_elements][number_of_particle_constituents], yielding a total of "
							<< required->number_of_particle_scattering_elements * required->number_of_particle_constituents << " elements. "
							"Instead, it currently has " << this->particle_scattering_element_composition.size()
							<< " elements." << std::endl;
					}
				}
				else {
					good = false;
					if (out) (*out) << "particle_scattering_element_composition has "
						"needs to be set for this shape. It should have dimensions of "
						"[number_of_particle_scattering_elements][number_of_particle_constituents]." << std::endl;
				}
			}
			
			if (this->particle_scattering_element_radius.size()) {
				if (this->particle_scattering_element_radius.size() != required->number_of_particle_scattering_elements) {
					good = false;
					if (out) (*out) << "particle_scattering_element_radius has the wrong size. "
						"It should have dimensions of [number_of_particle_scattering_elements]. "
						"particle_scattering_element_radius has a current size of " << particle_scattering_element_radius.size()
						<< ", and this should be " << required->number_of_particle_scattering_elements
						<< "." << std::endl;
				}
			}

			if (particle_constituent_single_name.size() && required->number_of_particle_constituents != 1) {
				good = false;
				if (out) (*out) << "particle_constituent_single_name is a valid attribute only when a single non-ice constituent exists."
					<< std::endl;
			}

			if (particle_constituent_single_name.size() && this->particle_constituent_name.size()) {
				good = false;
				if (out) (*out) << "particle_constituent_single_name and particle_constituent_name are mutually exclusive."
					<< std::endl;
			}

			if (this->particle_constituent_name.size()) {
				if (this->particle_constituent_name.size() != required->number_of_particle_constituents) {
					good = false;
					if (out) (*out) << "particle_constituent_name has the wrong size. "
						"It should have dimensions of [number_of_particle_constituents]. "
						<< std::endl;
				}
			}

			if (required->number_of_particle_constituents > 1 && this->particle_constituent_name.empty()) {
				good = false;
				if (out) (*out) << "number_of_particle_constituents > 1, so particle_constituent_name "
					"is required." << std::endl;
			}

			return good;
		}

		Shape_impl::Shape_impl(const std::string &id, Groups::Group::Group_HDF_shared_ptr grp)
			: Shape{ id }, Groups::Group_impl( grp ) {}
		Shape_impl::~Shape_impl() {}
		Shape::~Shape() {}
		Shape::Shape(const std::string &uid) : particle_unique_id{ uid } {}
		const std::string Shape::_icedb_obj_type_shape_identifier = "shape";
		const uint16_t Shape::_icedb_current_shape_schema_version = 0;

		bool Shape::isShape(Groups::Group &owner, const std::string &name) {
			if (!owner.doesGroupExist(name)) return false;
			auto grp = owner.openGroup(name);
			return isShape(grp->getHDF5Group().get());
		}
		bool Shape::isShape(gsl::not_null<H5::Group*> group) {
			if (!Attributes::CanHaveAttributes::doesAttributeExist(group, Group::_icedb_obj_type_identifier)) return false;
			if (Attributes::CanHaveAttributes::getAttributeTypeId(group, Group::_icedb_obj_type_identifier) != typeid(std::string)) return false;
			
			Attributes::Attribute<std::string> obj_type
				= Attributes::CanHaveAttributes::readAttribute<std::string>(group, Group::_icedb_obj_type_identifier);
			if (obj_type.data.size() != 1) return false;
			if (obj_type.data[0] != Shape::_icedb_obj_type_shape_identifier) return false;
			return true;
		}
		bool Shape::isShape() const {
			return isShape(getHDF5Group().get());
		}

		bool Shape::isValid(std::ostream *out) const { return isValid(this->getHDF5Group().get(), out); }
		bool Shape::isValid(gsl::not_null<H5::Group*> group, std::ostream *out) {
			/// Check for the existence of the standard tables, dimensions and attributes, and that
			/// they have the appropriate sizes.

			/// \todo Finish this, and tucn checks on attributes and variables into template functions!!!

			bool good = true;
			if (!isShape(group)) {
				good = false;
				if (out) (*out) << "This is not a valid shape. Missing the appropriate " 
					<< Groups::Group::_icedb_obj_type_identifier << " attribute." << std::endl;
				return good;
			}

			if (!Attributes::CanHaveAttributes::doesAttributeExist(group, "particle_id")) {
				good = false;
				if (out) (*out) << "Missing the particle_id attribute." << std::endl;
			}
			else if (Attributes::CanHaveAttributes::getAttributeTypeId(group, "particle_id") != typeid(std::string)) {
				good = false;
				if (out) (*out) << "The particle_id attribute has the wrong type." << std::endl;
			}
			else {
				auto attr = Attributes::CanHaveAttributes::readAttribute<std::string>(group, "particle_id");
				if (attr.data.size() != 1) {
					good = false;
					if (out) (*out) << "The particle_id attribute has the wrong size. It should be a scalar." << std::endl;
				}
				else {
					if (!attr.data[0].size()) {
						good = false;
						if (out) (*out) << "The particle_id attribute is empty." << std::endl;
					}
				}
			}
			if (out) (*out) << "TODO: Finish these checks!" << std::endl;
			return good;
		}

		Shape::Shape_Type Shape::openShape(Groups::Group &owner, const std::string &name) {
			Expects(owner.doesGroupExist(name));
			return openShape(owner.openGroup(name)->getHDF5Group());
		}
		
		Shape::Shape_Type Shape::openShape(Groups::Group &grpshp) {
			return openShape(grpshp.getHDF5Group());
		}

		Shape::Shape_Type Shape::createShape(Groups::Group &grpshp,
			const std::string &uid,
			gsl::not_null<const NewShapeRequiredProperties*> required,
			const NewShapeCommonOptionalProperties* optional)
		{
			return createShape(grpshp.getHDF5Group(), uid, required, optional);
		}

		Shape::Shape_Type Shape::createShape(Groups::Group &owner, const std::string &name,
			const std::string &uid,
			gsl::not_null<const NewShapeRequiredProperties*> required,
			const NewShapeCommonOptionalProperties* optional)
		{
			if (owner.doesGroupExist(name)) {
				const auto grp = owner.openGroup(name);
				return createShape(grp->getHDF5Group(), uid, required, optional);
			}
			else {
				const auto grp = owner.createGroup(name);
				return createShape(grp->getHDF5Group(), uid, required, optional);
			}
		}

		Shape::Shape_Type Shape::createShape(Groups::Group::Group_HDF_shared_ptr newShapeLocation,
			const std::string &uid,
			gsl::not_null<const NewShapeRequiredProperties*> required,
			const NewShapeCommonOptionalProperties* optional)
		{
			Expects(required->isValid(&(std::cerr)));
			if (required->requiresOptionalPropertiesStruct()) Expects(optional);
			if (optional) Expects(optional->isValid(required));

			Shape::Shape_Type res = std::make_unique<Shape_impl>(uid, newShapeLocation);

			// Write debug information
			{
				using namespace icedb::versioning;
				using std::string;
				auto libver = getLibVersionInfo();
				string sLibVer(libver->vgithash);
				res->writeAttribute<string>("_icedb_git_hash", { 1 }, { sLibVer });
				res->writeAttribute("_icedb_version", { 3 },
					{ libver->vn[versionInfo::V_MAJOR], libver->vn[versionInfo::V_MINOR], libver->vn[versionInfo::V_REVISION] });
			}

			// Write required attributes
			res->writeAttribute<std::string>(Group::_icedb_obj_type_identifier, { 1 }, { Shape::_icedb_obj_type_shape_identifier });
			res->writeAttribute<uint16_t>("_icedb_shape_schema_version", { 1 }, { Shape::_icedb_current_shape_schema_version });
			res->writeAttribute<std::string>("particle_id", { 1 }, { required->particle_id });
			res->writeAttribute<std::string>("dataset_id", { 1 }, { required->dataset_id });
			res->writeAttribute<uint64_t>("dataset_version", { 3 },
				{ required->dataset_version[0], required->dataset_version[1], required->dataset_version[2] });

			res->writeAttribute<float>("particle_scattering_element_coordinates_scaling_factor", { 1 }, { required->particle_scattering_element_coordinates_scaling_factor });
			res->writeAttribute<std::string>("particle_scattering_element_coordinates_scaling_factor__description", { 1 }, { "Factor to scale scattering element coordinates into physical dimensions." });
			res->writeAttribute<std::string>("particle_scattering_element_coordinates_units", { 1 }, { required->particle_scattering_element_coordinates_units });
			res->writeAttribute<std::string>("particle_scattering_element_coordinates_units__description", { 1 },
				{"Units of the physical dimensions of the particle_scattering_element_coordinates."});
			
			{ // Candidate UUID
				std::ostringstream oUUID;
				oUUID << required->dataset_id << "_" << required->particle_id << "_"
					<< required->dataset_version[0] << "." << required->dataset_version[1]
					<< "." << required->dataset_version[2];
				std::string sUUID = oUUID.str();
				res->writeAttribute<std::string>("_icedb_particle_uuid_candidate", { 1 }, { sUUID });
			}

			// Write required dimensions

			std::unique_ptr<icedb::Tables::Table> tblPSEN;
			{
				tblPSEN = res->createTable<uint64_t>("particle_scattering_element_number",
				{ static_cast<size_t>(required->number_of_particle_scattering_elements) });
				tblPSEN->writeAttribute<std::string>("description", { 1 }, { "ID number of scattering element" });
				tblPSEN->writeAttribute<std::string>("units", { 1 }, { "None" });
				//tblPSEN->writeAttribute<std::string>("comments", { 1 }, { "" });

				bool added = false;
				if (optional) {
					if (!optional->particle_scattering_element_number.empty()) {
						tblPSEN->writeAll(optional->particle_scattering_element_number);
						added = true;
					}
				}
				if (!added) {
					// Create "dummy" element numbers and write.
					std::vector<uint64_t> dummyPSENs(required->number_of_particle_scattering_elements);
					for (size_t i = 0; i < required->number_of_particle_scattering_elements; ++i)
						dummyPSENs[i] = i + 1;
					tblPSEN->writeAll(dummyPSENs);
				}
				// NOTE: The HDF5 dimension scale specification explicitly allows for dimensions to not have assigned values.
				// However, netCDF should have these values.
				tblPSEN->setDimensionScale("particle_scattering_element_number");
			}

			std::unique_ptr<icedb::Tables::Table> tblPCN;
			{
				tblPCN = res->createTable<uint8_t>("particle_constituent_number",
				{ static_cast<size_t>(required->number_of_particle_constituents) });
				tblPCN->writeAttribute<std::string>("description", { 1 }, { "ID number of the constituent material" });
				tblPCN->writeAttribute<std::string>("units", { 1 }, { "None" });
				//tblPCN->writeAttribute<std::string>("comments", { 1 }, { "" });
				bool added = false;
				if (optional) {
					if (!optional->particle_constituent_number.empty()) {
						tblPCN->writeAll<uint8_t>(optional->particle_constituent_number);
						added = true;
					}
				}
				if (!added) {
					// Create "dummy" constituent numbers and write.
					std::vector<uint8_t> dummyPCNs(required->number_of_particle_constituents);
					for (size_t i = 0; i < required->number_of_particle_constituents; ++i)
						dummyPCNs[i] = static_cast<uint8_t>(i + 1);
					tblPCN->writeAll<uint8_t>(dummyPCNs);
				}
				tblPCN->setDimensionScale("particle_constituent_number");
			}

			std::unique_ptr<icedb::Tables::Table> tblXYZ;
			if (required->NC4_compat) {
				tblXYZ = res->createTable<uint8_t>("particle_axis", { 3 }, { 0, 1, 2 });
				tblXYZ->setDimensionScale("particle_axis");
				tblXYZ->writeAttribute<std::string>("description", { 1 }, { "The {X,Y,Z} axes are used as dimensions for shape data." });
			}

			constexpr size_t max_x = 20000;
			const std::vector<size_t> chunks{ 
				(max_x < required->number_of_particle_scattering_elements) ? 
				max_x : required->number_of_particle_scattering_elements, 3 };
			
			// Determine if we can store the data as integers.
			// If non-integral coordinates, no.
			// If we can store the data as integers, then write the variable using the smallest
			// available integer storage type.
			/// \todo With HDFforHumans, be sure to add support for using minimal-sized types.
			/// \todo Auto-detect if we are using integral scattering element coordinates.
			bool useInts = (required->particle_scattering_element_coordinates_are_integral) ? true : false;
			
			if (useInts) {
				std::vector<int32_t> crds_ints(required->number_of_particle_scattering_elements*3);
				for (size_t i = 0; i < crds_ints.size(); ++i) {
					// Narrow cast will check that the representation remains the same.
					// If the values change, then the cast will fail and the program will abort.
					crds_ints[i] = gsl::narrow_cast<int32_t>(required->particle_scattering_element_coordinates[i]);
				}
				auto tblPSEC = res->createTable<int32_t>(
					"particle_scattering_element_coordinates",
					{ static_cast<size_t>(required->number_of_particle_scattering_elements), 3 },
					crds_ints, &chunks);
				if (tblPSEN) tblPSEC->attachDimensionScale(0, tblPSEN.get());
				if (tblXYZ) tblPSEC->attachDimensionScale(1, tblXYZ.get());
				tblPSEC->writeAttribute<std::string>("description", { 1 }, { "Cartesian coordinates (x,y,z) of the center of the scattering element (dipole position, center of sphere, etc.)" });
				tblPSEC->writeAttribute<std::string>("units", { 1 }, { "None" });
				//tblPSEC->writeAttribute<std::string>("comments", { 1 }, {
				//	"Equivalent to the coordinates given in a DDA shape-file (x,y,z-dimension) "
				//	"so scattering computations can be easily repeated with the same structure; "
				//	"for sphere methods the coordinates describe the center location of the sphere." });
			} else {
				auto tblPSEC = res->createTable<float>("particle_scattering_element_coordinates",
				{ static_cast<size_t>(required->number_of_particle_scattering_elements), 3 },
					required->particle_scattering_element_coordinates, &chunks);
				if (tblPSEN) tblPSEC->attachDimensionScale(0, tblPSEN.get());
				if (tblXYZ) tblPSEC->attachDimensionScale(1, tblXYZ.get());
				tblPSEC->writeAttribute<std::string>("description", { 1 }, { "Cartesian coordinates (x,y,z) of the center of the scattering element (dipole position, center of sphere, etc.)" });
				tblPSEC->writeAttribute<std::string>("units", { 1 }, { "None" });
				//tblPSEC->writeAttribute<std::string>("comments", { 1 }, {
				//	"Equivalent to the coordinates given in a DDA shape-file (x,y,z-dimension) "
				//	"so scattering computations can be easily repeated with the same structure; "
				//	"for sphere methods the coordinates describe the center location of the sphere." });
			}

			if (optional) {

				// TODO: if (optional->particle_constituent_name.size()) {}
				if (optional->particle_constituent_single_name.size()) {
					res->writeAttribute<std::string>("particle_single_constituent_name",
						{ 1 }, { optional->particle_constituent_single_name });
					res->writeAttribute<std::string>("particle_single_constituent_name__description",
						{ 1 }, { "This 3d structure is entirely composed of this material." });
				}

				if (optional->particle_scattering_element_composition.size()) {
					const std::vector<size_t> cs{
						(max_x < required->number_of_particle_scattering_elements) ?
						max_x : required->number_of_particle_scattering_elements,
						static_cast<size_t>(required->number_of_particle_constituents)
					};
					auto tblPSEC2a = res->createTable<float>("particle_scattering_element_composition",
					{ static_cast<size_t>(required->number_of_particle_scattering_elements),
						static_cast<size_t>(required->number_of_particle_constituents) },
						optional->particle_scattering_element_composition, &cs);
					if (tblPSEN) tblPSEC2a->attachDimensionScale(0, tblPSEN.get());
					if (tblPCN) tblPSEC2a->attachDimensionScale(1, tblPCN.get());
					tblPSEC2a->writeAttribute<std::string>("description", { 1 }, { "Mass fractions of each constituent for each scattering element." });
					tblPSEC2a->writeAttribute<std::string>("units", { 1 }, { "None" });
					//tblPSEC2a->writeAttribute<std::string>("comments", { 1 }, { "" });
				}

				/*
				if (optional->particle_scattering_element_composition_whole.size()) {
					const std::vector<size_t> cs{
						(max_x < required->number_of_particle_scattering_elements) ?
						max_x : required->number_of_particle_scattering_elements
					};
					auto tblPSEC2b = res->createTable<uint8_t>(
						"particle_scattering_element_composition_whole",
						{ static_cast<size_t>(required->number_of_particle_scattering_elements) },
						optional->particle_scattering_element_composition_whole, &cs);
					if (tblPSEN) tblPSEC2b->attachDimensionScale(0, tblPSEN.get());
					tblPSEC2b->writeAttribute<std::string>("description", { 1 }, { "The constituent material ID for each scattering element." });
					tblPSEC2b->writeAttribute<std::string>("units", { 1 }, { "None" });
					//tblPSEC2b->writeAttribute<std::string>("comments", { 1 }, { "" });
				}
				*/

				

				// Write common optional attributes
				

				// Write common optional variables
				if (optional->particle_scattering_element_radius.size()) {
					auto tblPSER = res->createTable<float>("particle_scattering_element_radius",
					{ static_cast<size_t>(required->number_of_particle_scattering_elements) },
						optional->particle_scattering_element_radius);
					if (tblPSEN) tblPSER->attachDimensionScale(0, tblPSEN.get());
					tblPSER->writeAttribute<std::string>("description", { 1 }, { "Physical radius of the scattering sphere." });
					tblPSER->writeAttribute<std::string>("units", { 1 }, { "m" });
					tblPSER->writeAttribute<std::string>("comments", { 1 }, { "TODO: Units are under discussion. Either in meters or dimensionless and scaled by particle_scattering_element_spacing." });
				}

			}

			return res;
		}

		Shape::Shape_Type Shape::openShape(Groups::Group::Group_HDF_shared_ptr shape) {
			// Get UUID
			//res->writeAttribute<std::string>("particle_id", { 1 }, { required->particle_id });
			Expects(isShape(shape.get()));
			//Expects(shape->attrExists("particle_id")); // Expects(isShape(shape.get())) subsumes this.
			auto id = Attributes::CanHaveAttributes::readAttribute<std::string>(shape.get(),"particle_id");

			Shape::Shape_Type res = std::make_unique<Shape_impl>(id.data[0], shape);
			return res;
		}
	}
}
