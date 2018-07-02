#include <map>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <cstdio>

#include "shape.hpp"
#include "shapeIOtext.hpp"

namespace icedb {
	namespace Examples {
		namespace Shapes {
			void ShapeRequiredData::apply(icedb::Shapes::NewShapeRequiredProperties& p) const {
				p.particle_id = particle_id;
				p.number_of_particle_constituents = number_of_particle_constituents;
				p.number_of_particle_scattering_elements = number_of_particle_scattering_elements;
				p.particle_scattering_element_coordinates = this->particle_scattering_element_coordinates;
				p.particle_scattering_element_coordinates_are_integral = this->particle_scattering_element_coordinates_are_integral;
			}
			void ShapeCommonOptionalData::apply(icedb::Shapes::NewShapeCommonOptionalProperties& p) const {
				// When applying these, check for the trivial cases. If these occur, then do not
				// set the output field.

				// If p.particle_scattering_element_number is a linear sequence 1...N, then it is trivial
				bool scat_elem_num_is_trivial = true;
				for (size_t i = 0; i < this->particle_scattering_element_number.size(); ++i) {
					if (this->particle_scattering_element_number[i] != i + 1) scat_elem_num_is_trivial = false;
				}
				if (!scat_elem_num_is_trivial)
					p.particle_scattering_element_number = this->particle_scattering_element_number;
				// Matters only if more than one constituent
				if (this->particle_constituent_number.size() > 1)
					p.particle_constituent_number = this->particle_constituent_number;


				p.particle_scattering_element_radius = this->particle_scattering_element_radius;
				p.particle_constituent_name = this->particle_constituent_name;
				p.particle_constituent_single_name = this->particle_constituent_single_name;
				
				p.particle_scattering_element_composition_fractional = particle_scattering_element_composition_fractional;

				// If p.particle_scattering_element_composition_whole is specified, and it is all ones, then it is trivial.
				bool comp_whole_is_trivial = false;
				if (std::all_of(particle_scattering_element_composition_whole.begin(),
					particle_scattering_element_composition_whole.end(),
					[](uint8_t v)->bool { return (v == 1) ? true : false; }))
					comp_whole_is_trivial = true;
				if (!comp_whole_is_trivial)
					p.particle_scattering_element_composition_whole = particle_scattering_element_composition_whole;

				p.particle_scattering_element_spacing = this->particle_scattering_element_spacing;
				p.hint_max_scattering_element_dimension = this->hint_max_scattering_element_dimension;
			}
			icedb::Shapes::Shape ShapeDataBasic::toShape(
				HH::HH_hid_t parentGrpID,
				const std::string &name) const
			{
				icedb::Shapes::NewShapeRequiredProperties nreq;
				required.apply(nreq);
				icedb::Shapes::NewShapeCommonOptionalProperties nopt;
				optional.apply(nopt);
				HH::Group gParent(parentGrpID);
				Expects(0 == gParent.exists(name.c_str()));
				auto gShp = gParent.create(name.c_str());
				return icedb::Shapes::Shape::createShape(gShp.get(), &nreq, &nopt);
			}
		}
	}
}
