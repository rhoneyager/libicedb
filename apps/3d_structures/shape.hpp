#pragma once
//#include <Eigen/Dense>
#include <memory>
#include <map>
#include <string>

#include <icedb/shape.hpp>
namespace icedb {
	namespace Examples {
		namespace Shapes {
			typedef std::vector<uint64_t> IntData_t;
			typedef std::vector<float> FloatData_t;
			typedef std::vector<std::string> StringData_t;

			struct ShapeRequiredData {
				IntData_t particle_scattering_element_number;
				IntData_t particle_constituent_number;
				FloatData_t particle_scattering_element_coordinates;
				FloatData_t particle_scattering_element_composition;
				std::string particle_id;
				void apply(icedb::Shapes::NewShapeRequiredProperties&) const;
				void fromShapeObject(const icedb::Shapes::Shape&);
			};

			struct ShapeCommonOptionalData {
				FloatData_t particle_scattering_element_radius;
				StringData_t particle_constituent_name;
				float particle_scattering_element_spacing = -1;
				void apply(icedb::Shapes::NewShapeCommonOptionalProperties&) const;
				void fromShapeObject(const icedb::Shapes::Shape&);
			};
			struct ShapeDataBasic {
				ShapeRequiredData required;
				ShapeCommonOptionalData optional;
				icedb::Shapes::Shape::Shape_Type toShape(icedb::Groups::Group::Group_ptr) const;
				void fromShape(icedb::Groups::Group::Group_ptr);
			};
		}
	}
}
