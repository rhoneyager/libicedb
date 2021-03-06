#pragma once
#include "../defs.h"
#include "fs.hpp"
#include <array>
#include <vector>
#include <HH/Groups.hpp>
#include "Common.hpp"
#include "../Plugins/registry.hpp"
#include "io.hpp"

namespace icedb {
	namespace Shapes {
		class Shape;
		struct NewShapeProperties;

		enum class e_Common_Obj_Names {
			// Attributes
			icedb_Version, Schema_Version, icedb_git_hash, particle_id,
			dataset_id, author, contact, version,
			scattering_element_coordinates_scaling_factor,
			scattering_element_coordinates_units,
			scattering_method,
			particle_scattering_element_radius,

			// Datasets
			particle_scattering_element_number,
			particle_constituent_number,
			particle_constituent_name,
			particle_axis,
			// particle_scattering_element_coordinates - can be a float or an int!
			particle_scattering_element_coordinates_int,
			particle_scattering_element_coordinates_float,
			particle_scattering_element_composition_fractional,
			particle_scattering_element_composition_whole
		};
		/// Each Swath 'group' has an attribute with this identifier.
		/// Used for Swath collection and searching.
		extern ICEDB_DL const std::string _obj_type_identifier;
		extern ICEDB_DL const std::map<e_Common_Obj_Names, Name_Type_t > Required_Atts;
		extern ICEDB_DL const std::map<e_Common_Obj_Names, Name_Type_t > Optional_Atts;
		extern ICEDB_DL const std::map<e_Common_Obj_Names, Name_Type_t > Required_Dsets;
		extern ICEDB_DL const std::map<e_Common_Obj_Names, Name_Type_t > Optional_Dsets;

		namespace _impl {
			class ShapeProps_IO_Input_Registry {};
			class Shape_IO_Output_Registry {};
		}
	}
	namespace registry {
		extern template struct IO_class_registry_writer <
			::icedb::Shapes::Shape >;
		extern template struct IO_class_registry_reader <
			::icedb::Shapes::NewShapeProperties >;
		extern template class usesDLLregistry <
			::icedb::Shapes::_impl::ShapeProps_IO_Input_Registry,
			IO_class_registry_reader<::icedb::Shapes::NewShapeProperties> >;
		extern template class usesDLLregistry <
			::icedb::Shapes::_impl::Shape_IO_Output_Registry,
			IO_class_registry_writer<::icedb::Shapes::Shape> >;
	}

	template<>
	struct Common_HH_Base < Shapes::Shape, Shapes::e_Common_Obj_Names>
	{
		using CN = Shapes::e_Common_Obj_Names;
		typedef const std::map<CN, Name_Type_t> Validator_map_t;
		static Validator_map_t getRequiredAttributes() { return Shapes::Required_Atts; }
		static Validator_map_t getRequiredDatasets() { return Shapes::Required_Dsets; }
		static std::string getIdentifier() { return Shapes::_obj_type_identifier; }
	};

	/// All facilities to manipulate particle shapes
	namespace Shapes {

		/// Strucure containing a list of all of the data needed to create a new shape in the database
		struct ICEDB_DL NewShapeProperties :
			virtual public registry::usesDLLregistry<
				_impl::ShapeProps_IO_Input_Registry,
				registry::IO_class_registry_reader<NewShapeProperties> >,
			virtual public io::implementsStandardReader<NewShapeProperties, _impl::ShapeProps_IO_Input_Registry>
		{
			// Each particle has three axes (X, Y and Z). As these are trivial, only the
			// scale lables are written; the scale does not have an explicit dataset filled with values.
			
			// The required VARIABLES

			/// VARIABLE: Cartesian coordinates of the center of each scattering element
			/// Written in form of x_1, y_1, z_1, x_2, y_2, z_2, ...
			/// Dimensions of [number_of_particle_scattering_elements][axis]
			std::vector<float> particle_scattering_element_coordinates_as_floats;
			std::vector<int32_t> particle_scattering_element_coordinates_as_ints;


			// The required ATTRIBUTES

			/// ATTRIBUTE: Unique Particle Identifier
			std::string particle_id;

			/// ATTRIBUTE: Dataset identifier
			std::string dataset_id;

			/// ATTRIBUTE: Author
			std::string author;

			/// ATTRIBUTE: Contact info
			std::string contact;

			/// ATTRIBUTE: Version information
			std::array<unsigned int, 3> version = std::array<unsigned int, 3>{ 0, 0, 0 };

			/// VARIABLE: The id number for each scattering element. Single dimension.
			std::vector<int32_t> particle_scattering_element_number;
			/// VARIABLE: The id number of each particle's constituent. Single dimension.
			std::vector<std::pair<uint16_t, std::string> > particle_constituents;

			/// OPTIONAL VARIABLE: Physical radius of the scattering sphere, in meters.
			/// Dimensions: [particle_scattering_element_number]
			std::vector<float> particle_scattering_element_radius;

			/// OPTIONAL VARIABLE: Mass fractions of each constituent for each scattering element.
			/// Either this or particle_scattering_element_composition_whole is
			/// is required _only_ if there is more than one constituent
			/// CANNOT COEXIST with particle_scattering_element_composition_whole.
			/// Dimensions of [particle_scattering_element_number][particle_constituent_number]
			std::vector<float> particle_scattering_element_composition_fractional;
			/// OPTIONAL VARIABLE: The constituent of each scattering element
			/// This table is used when scattering elements can only be a single substance (not fractional).
			/// This CANNOT COEXIST with particle_scattering_element_composition_fractional.
			/// It exists to save space when non-compressed data is stored.
			std::vector<uint16_t> particle_scattering_element_composition_whole;

			/// OPTIONAL ATTRIBUTE: Physical spacing between adjacent grid points.
			float scattering_element_coordinates_scaling_factor = 1.0f;

			/// OPTIONAL ATTRIBUTE: Physical spacing units.
			std::string scattering_element_coordinates_units = "m";

			/// OPTIONAL ATTRIBUTE: Scattering method used with the particle
			std::string scattering_method;


			/// Validate that all required properties are set, and that they have the correct dimensions.
			/// Writes diagnostic messages to the output stream.
			bool isValid(std::ostream *errout = nullptr) const;
		};

		/// \brief A high-level class to manipulate particle shapes
		///
		/// Shapes are implemented as a set of tables and attributes, contained within a discrete Group.
		/// This class provides a higl-level interface to accessing and manipulating shapes.
		/// It acts as an "overlay" to an alreagy-existing group. It adds additional functions and "value".
		class ICEDB_DL Shape :
			virtual public HH::Group,
			virtual public registry::usesDLLregistry<
				_impl::Shape_IO_Output_Registry,
				registry::IO_class_registry_writer<Shape> >,
			virtual public io::implementsStandardWriter<Shape, _impl::Shape_IO_Output_Registry>
		{
		public:
			//static const std::string _obj_type_identifier; // Has to be forward-declarable.
			static const uint16_t _current_schema_version;

			/// OPEN an HDF5 handle as a shape. Validity not guaranteed.
			Shape(HH::Group grp = HH::HH_hid_t::dummy()) : HH::Group(grp) {  }

			/// This is the unique identifier for this shape
			//const std::string particle_unique_id;

			virtual ~Shape() {}
			
			/// \brief Create a new shape
			/// \param newLocationAsEmptyGroup is the empty group that is converted into a shape
			/// \param props are the shape properties.
			/// \throws if the group has any already-existing tables or attributes that conflict with the new shape object
			/// \returns the new shape on success
			/// \throws on failure
			static Shape createShape(
				HH::Group newLocationAsEmptyGroup,
				const NewShapeProperties& props);
		};
	}
}
