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
			typedef std::vector<uint8_t> Int8Data_t;
			typedef std::vector<float> FloatData_t;
			typedef std::vector<std::string> StringData_t;

			struct ShapeRequiredData {
				bool NC4_compat = true;
				uint64_t number_of_particle_scattering_elements = 0;
				uint8_t number_of_particle_constituents = 0;

				uint8_t particle_scattering_element_coordinates_are_integral = 0;
				FloatData_t particle_scattering_element_coordinates;
				std::string particle_id;
				void apply(icedb::Shapes::NewShapeRequiredProperties&) const;
				//void fromShapeObject(const icedb::Shapes::Shape&);
			};

			struct ShapeCommonOptionalData {
				IntData_t particle_scattering_element_number;
				Int8Data_t particle_constituent_number;
				FloatData_t particle_scattering_element_radius;
				StringData_t particle_constituent_name;
				std::string particle_constituent_single_name;

				FloatData_t particle_scattering_element_composition_fractional;
				Int8Data_t particle_scattering_element_composition_whole;
				float hint_max_scattering_element_dimension = -1;
				float particle_scattering_element_spacing = -1;
				void apply(icedb::Shapes::NewShapeCommonOptionalProperties&) const;
				//void fromShapeObject(const icedb::Shapes::Shape&);
			};
			struct ShapeDataBasic {
				ShapeRequiredData required;
				ShapeCommonOptionalData optional;
				/// Make a new shape under the group
				icedb::Shapes::Shape::Shape_Type toShape(const std::string &name, std::shared_ptr<H5::Group>) const;
				//void fromShape(icedb::Groups::Group::Group_ptr);
			};
		}
	}
}
