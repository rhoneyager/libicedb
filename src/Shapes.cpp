#include <string>
#include <sstream>
#include <iostream>
#include "../icedb/shape.hpp"
#include "../private/hdf5_supplemental.hpp"
#include "../private/Shape_impl.hpp"

namespace icedb {
	namespace Shapes {
		bool NewShapeRequiredProperties::isValid(std::ostream *out) const {
			bool good = true;
			if (!particle_id.size()) {
				good = false;
				if (out) (*out) << "Particle ID is not set." << std::endl;
			}
			if (this->particle_scattering_element_number.empty()) {
				good = false;
				if (out) (*out) << "particle_scattering_element_number is not set."
					" This is an essential dimension scale that the rest of the particle "
					"data depends on." << std::endl;
			}
			if (this->particle_constituent_number.empty()) {
				good = false;
				if (out) (*out) << "particle_constituent_number is not set. "
					"This is an essential dimension scale that the rest of the particle "
					"data depends on." << std::endl;
			}
			if (this->particle_scattering_element_coordinates.empty()) {
				good = false;
				if (out) (*out) << "particle_scattering_element_coordinates is not set. "
					"Particles need to have scattering elements." << std::endl;
			}
			else if (particle_scattering_element_coordinates.size() != (3 * particle_scattering_element_number.size())) {
				good = false;
				if (out) (*out) << "particle_scattering_element_coordinates has "
					"the wrong dimensions. It should have dimensions of "
					"[particle_scattering_element_number][3], yielding a total of "
					<< 3 * particle_scattering_element_number.size() << " elements. "
					"Instead, it currently has " << particle_scattering_element_coordinates.size()
					<< " elements." << std::endl;
			}
			if (this->particle_scattering_element_composition.empty()) {
				good = false;
				if (out) (*out) << "particle_scattering_element_composition is not set. "
					"Particles need to have a composition." << std::endl;
			}
			else if (this->particle_scattering_element_composition.size() !=
				(particle_scattering_element_number.size() * particle_constituent_number.size())) {
				good = false;
				if (out) (*out) << "particle_scattering_element_composition has "
					"the wrong dimensions. It should have dimensions of "
					"[particle_scattering_element_number][particle_constituent_number], yielding a total of "
					<< particle_constituent_number.size() * particle_scattering_element_number.size() << " elements. "
					"Instead, it currently has " << particle_scattering_element_composition.size()
					<< " elements." << std::endl;
			}

			return good;
		}

		bool NewShapeCommonOptionalProperties::isValid(
			gsl::not_null<const NewShapeRequiredProperties*> required, std::ostream *out) const
		{
			bool good = true;
			if (this->particle_scattering_element_radius.size()) {
				if (this->particle_scattering_element_radius.size() != required->particle_scattering_element_number.size()) {
					good = false;
					if (out) (*out) << "particle_scattering_element_radius has the wrong size. "
						"It should have dimensions of [particle_scattering_element_number]. "
						"particle_scattering_element_radius has a current size of " << particle_scattering_element_radius.size()
						<< ", and this should be " << required->particle_scattering_element_number.size()
						<< "." << std::endl;
				}
			}

			if (this->particle_constituent_name.size()) {
				if (this->particle_constituent_name.size() != required->particle_constituent_number.size()) {
					good = false;
					if (out) (*out) << "particle_constituent_name has the wrong size. "
						"It should have dimensions of [particle_constituent_number]. "
						"particle_constituent_name has a current size of " << particle_constituent_name.size()
						<< ", and this should be " << required->particle_constituent_number.size()
						<< "." << std::endl;
				}
			}


			return good;
		}

		Shape_impl::Shape_impl(const std::string &id, Groups::Group::Group_HDF_shared_ptr grp)
			: Shape{ id }, Groups::Group_impl{ grp } {}
		Shape_impl::~Shape_impl() {}
		Shape::~Shape() {}
		Shape::Shape(const std::string &uid) : particle_unique_id{ uid } {}
		std::string Shape::_icedb_obj_type_shape_identifier = "shape";

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
			assert(owner.doesGroupExist(name));
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
			Expects(required->isValid());
			if (optional) Expects(optional->isValid(required));
			
			Shape::Shape_Type res = std::make_unique<Shape_impl>(uid, newShapeLocation);
			
			// Write required attributes
			res->writeAttribute<std::string>(Group::_icedb_obj_type_identifier, { 1 }, { Shape::_icedb_obj_type_shape_identifier });
			res->writeAttribute<std::string>("particle_id", { 1 }, { required->particle_id });

			// Write required dimensions
			auto tblPSEN = res->createTable<uint64_t>("particle_scattering_element_number",
				{ static_cast<size_t>(required->particle_scattering_element_number.size()) },
				required->particle_scattering_element_number);
			tblPSEN->setDimensionScale("particle_scattering_element_number");

			auto tblPCN = res->createTable<uint64_t>("particle_constituent_number",
				{ static_cast<size_t>(required->particle_constituent_number.size()) },
				required->particle_constituent_number);
			tblPCN->setDimensionScale("particle_constituent_number");

			auto tblXYZ = res->createTable<uint64_t>("particle_axis",
			{ 3 }, { 0, 1, 2 });
			tblXYZ->setDimensionScale("particle_axis");

			// Write required variables

			auto tblPSEC = res->createTable<float>("particle_scattering_element_coordinates",
			{ static_cast<size_t>(required->particle_scattering_element_number.size()),
				3 },
				required->particle_scattering_element_coordinates);
			tblPSEC->attachDimensionScale(0, tblPSEN.get());
			tblPSEC->attachDimensionScale(1, tblXYZ.get());

			auto tblPSEC2 = res->createTable<float>("particle_scattering_element_composition",
			{ static_cast<size_t>(required->particle_scattering_element_number.size()),
				static_cast<size_t>(required->particle_constituent_number.size()) },
				required->particle_scattering_element_composition);
			tblPSEC2->attachDimensionScale(0, tblPSEN.get());
			tblPSEC2->attachDimensionScale(1, tblPCN.get());

			if (optional) {
				// Write common optional attributes
				if (optional->particle_scattering_element_spacing > 0)
					res->writeAttribute<float>("particle_scattering_element_spacing", { 1 }, { optional->particle_scattering_element_spacing });

				// Write common optional variables
				if (optional->particle_scattering_element_radius.size()) {
					auto tblPSER = res->createTable<float>("particle_scattering_element_radius",
					{ static_cast<size_t>(required->particle_scattering_element_number.size()) },
						optional->particle_scattering_element_radius);
					tblPSER->attachDimensionScale(0, tblPSEN.get());
				}

				// particle_constituent_name
				/// \todo Add particle_constituent_name write.
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
