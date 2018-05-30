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
				p.NC4_compat = this->NC4_compat;
				p.particle_id = particle_id;
				p.number_of_particle_constituents = number_of_particle_constituents;
				p.number_of_particle_scattering_elements = number_of_particle_scattering_elements;
				p.particle_scattering_element_coordinates = this->particle_scattering_element_coordinates;
				p.particle_scattering_element_coordinates_are_integral = this->particle_scattering_element_coordinates_are_integral;
				p.author = this->author;
				p.contact_information = this->contact_information;
				p.dataset_id = this->dataset_id;
				p.dataset_version = this->dataset_version;
				p.particle_scattering_element_coordinates_scaling_factor = this->particle_scattering_element_coordinates_scaling_factor;
				p.particle_scattering_element_coordinates_units = this->particle_scattering_element_coordinates_units;

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
				
				p.particle_scattering_element_composition = particle_scattering_element_composition;

				
			}
			icedb::Shapes::Shape::Shape_Type ShapeDataBasic::toShape(
				const std::string &name, std::shared_ptr<H5::Group> grp) const
			{
				icedb::Shapes::NewShapeRequiredProperties nreq;
				required.apply(nreq);
				icedb::Shapes::NewShapeCommonOptionalProperties nopt;
				optional.apply(nopt);
				return icedb::Shapes::Shape::createShape(grp, name, &nreq, &nopt);
			}
		}
	}
}
