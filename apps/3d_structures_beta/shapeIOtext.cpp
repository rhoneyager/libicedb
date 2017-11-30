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
				p.particle_constituent_number = this->particle_constituent_number;
				p.particle_scattering_element_composition = this->particle_scattering_element_composition;
				p.particle_scattering_element_coordinates = this->particle_scattering_element_coordinates;
				p.particle_scattering_element_number = this->particle_scattering_element_number;
			}
			void ShapeCommonOptionalData::apply(icedb::Shapes::NewShapeCommonOptionalProperties& p) const {
				p.particle_constituent_name = this->particle_constituent_name;
				p.particle_scattering_element_radius = this->particle_scattering_element_radius;
				p.particle_scattering_element_spacing = this->particle_scattering_element_spacing;
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
