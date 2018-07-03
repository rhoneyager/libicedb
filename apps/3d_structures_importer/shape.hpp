#pragma once
//#include <Eigen/Dense>
#include <array>
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
				uint64_t number_of_particle_scattering_elements = 0;
				uint8_t number_of_particle_constituents = 0;

				uint8_t particle_scattering_element_coordinates_are_integral = 0;
				FloatData_t particle_scattering_element_coordinates;
				std::string particle_id;
				std::string dataset_id;
				std::string author;
				std::string contact;
				std::array<unsigned int, 3> version;

				void apply(icedb::Shapes::NewShapeRequiredProperties&) const;
				//void fromShapeObject(const icedb::Shapes::Shape&);
			};

			struct ShapeCommonOptionalData {
				IntData_t particle_scattering_element_number;
				Int8Data_t particle_constituent_number;
				FloatData_t particle_scattering_element_radius;
				StringData_t particle_constituent_name;
				//std::string particle_constituent_single_name;
				std::string scattering_method;

				FloatData_t particle_scattering_element_composition_fractional;
				Int8Data_t particle_scattering_element_composition_whole;
				float scattering_element_coordinates_scaling_factor = 1.0f;
				std::string scattering_element_coordinates_units = "m";
				void apply(icedb::Shapes::NewShapeCommonOptionalProperties&) const;
				//void fromShapeObject(const icedb::Shapes::Shape&);
			};
			struct ShapeDataBasic {
				ShapeRequiredData required;
				ShapeCommonOptionalData optional;
				/// Make a new shape under the group
				icedb::Shapes::Shape toShape(
					HH::HH_hid_t parentGrpID,
					const std::string &name) const;
				//void fromShape(icedb::Groups::Group::Group_ptr);
			};
		}
	}
}
