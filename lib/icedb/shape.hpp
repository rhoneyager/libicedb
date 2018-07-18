#pragma once
#include "fs.hpp"
#include <array>
#include <HH/Groups.hpp>
#include "registry.hpp"
#include "io.hpp"

namespace icedb {
	namespace Shapes {
		class Shape;
		namespace _impl {
			class Shape_IO_Input_Registry {};
			class Shape_IO_Output_Registry {};
		}
	}
	namespace registry {
		extern template struct IO_class_registry_writer <
			::icedb::Shapes::Shape >;
		extern template struct IO_class_registry_reader <
			::icedb::Shapes::Shape >;
		extern template class usesDLLregistry <
			::icedb::Shapes::_impl::Shape_IO_Input_Registry,
			IO_class_registry_reader<::icedb::Shapes::Shape> >;
		extern template class usesDLLregistry <
			::icedb::Shapes::_impl::Shape_IO_Output_Registry,
			IO_class_registry_writer<::icedb::Shapes::Shape> >;
	}

	/// All facilities to manipulate particle shapes
	namespace Shapes {

		/// Strucure containing a list of all of the required data needed to create a new shape in the database
		struct NewShapeRequiredProperties {
			// The DIMENSIONS

			/// DIMENSION: The number of scattering elements.
			/// If these have non-trivial ids (i.e. not 1, 2, 3, 4, ...), then define the
			/// optional attribute particle_scattering_element_number.
			uint64_t number_of_particle_scattering_elements = 0;

			/// DIMENSION: The number of distinct constituents in the particle.
			/// If these have non-trivial ids (i.e. not 1, 2, 3, 4, ...), then define the
			/// optional attribute particle_constituent_number.
			uint16_t number_of_particle_constituents = 0;

			// Each particle has three axes (X, Y and Z). As these are trivial, only the
			// scale lables are written; the scale does not have an explicit dataset filled with values.
			
			// The VARIABLES

			/// VARIABLE: Cartesian coordinates of the center of each scattering element
			/// Written in form of x_1, y_1, z_1, x_2, y_2, z_2, ...
			/// Dimensions of [number_of_particle_scattering_elements][axis]
			gsl::span<const float> particle_scattering_element_coordinates;

			/// Are the particle_scattering_element_coordinates integers?
			/// This allows for optimization when writing.
			/// If they are integers, then particle_scattering_element_coordinates_ints is written instead.
			uint16_t particle_scattering_element_coordinates_are_integral = 0;
			

			// The ATTRIBUTES

			/// ATTRIBUTE: Unique Particle Identifier
			std::string particle_id;

			/// ATTRIBUTE: Dataset identifier
			std::string dataset_id;

			/// ATTRIBUTE: Author
			std::string author;

			/// ATTRIBUTE: Contact info
			std::string contact;

			/// ATTRIBUTE: Version information
			std::array<unsigned int, 3> version;


			/// Validate that all required properties are set, and that they have the correct dimensions.
			/// Writes diagnostic messages to the output stream.
			bool isValid(std::ostream *errout = nullptr) const;

			/// Based on the required properties, must the optional properties structure be required?
			/// i.e. Extra information is needed to properly construct the shape.
			bool requiresOptionalPropertiesStruct() const;
		};
		/// Structure containing a list of all of the common optional data for creating a new shape in the database.
		struct NewShapeCommonOptionalProperties {
			/// DIMENSION: The id number for each scattering element. Single dimension.
			gsl::span<const uint64_t> particle_scattering_element_number;
			/// DIMENSION: The id number of each particle's constituent. Single dimension.
			gsl::span<const uint16_t> particle_constituent_number;


			/// OPTIONAL VARIABLE: Physical radius of the scattering sphere, in meters.
			/// Dimensions: [particle_scattering_element_number]
			gsl::span<const float> particle_scattering_element_radius;

			/// VARIABLE: The name of each particle's constituent. Single dimension.
			/// Becomes REQUIRED if there is more than one constituent. If there is
			/// only one constituent, and if this variable is not written, then it can be assumed to be ice.
			/// Dimensions of [particle_constituent_number]
			gsl::span<const std::string> particle_constituent_name;

			/// OPTIONAL VARIABLE: Mass fractions of each constituent for each scattering element.
			/// Either this or particle_scattering_element_composition_whole is
			/// is required _only_ if there is more than one constituent
			/// CANNOT COEXIST with particle_scattering_element_composition_whole.
			/// Dimensions of [particle_scattering_element_number][particle_constituent_number]
			gsl::span<const float> particle_scattering_element_composition_fractional;
			/// OPTIONAL VARIABLE: The constituent of each scattering element
			/// This table is used when scattering elements can only be a single substance (not fractional).
			/// This CANNOT COEXIST with particle_scattering_element_composition_fractional.
			/// It exists to save space when non-compressed data is stored.
			gsl::span<const uint16_t> particle_scattering_element_composition_whole;

			/// OPTIONAL ATTRIBUTE: Physical spacing between adjacent grid points.
			float scattering_element_coordinates_scaling_factor = 1.0f;

			/// OPTIONAL ATTRIBUTE: Physical spacing units.
			std::string scattering_element_coordinates_units = "m";

			/// OPTIONAL ATTRIBUTE: Scattering method used with the particle
			std::string scattering_method;

			// REMOVED HINT: Specify the maximum scattering element dimension
			//[[deprecated]] float hint_max_scattering_element_dimension = -1;

			/// Validate that all required properties are set, and that they have the correct dimensions.
			/// Writes diagnostic messages to the output stream.
			bool isValid(gsl::not_null<const NewShapeRequiredProperties*> required, std::ostream *errout = nullptr) const;
		};

		/// \brief A high-level class to manipulate particle shapes
		///
		/// Shapes are implemented as a set of tables and attributes, contained within a discrete Group.
		/// This class provides a higl-level interface to accessing and manipulating shapes.
		/// It acts as an "overlay" to an alreagy-existing group. It adds additional functions and "value".
		class Shape : 
			virtual public HH::Group,
			virtual public registry::usesDLLregistry<
				_impl::Shape_IO_Output_Registry,
				registry::IO_class_registry_writer<Shape> >,
			virtual public io::implementsStandardWriter<Shape, _impl::Shape_IO_Output_Registry>
		{
		public:
			/// Each shape 'group' has an attribute with this identifier. Used for shape collection and searching.
			static const std::string _icedb_obj_type_shape_identifier;
			static const uint16_t _icedb_current_shape_schema_version;

			// OPEN an already-existing shape. Validity not guaranteed.
			Shape(HH::HH_hid_t hnd_grp = HH::HH_hid_t::dummy()) : HH::Group(hnd_grp) {  }

			/// This is the unique identifier for this shape
			const std::string particle_unique_id;

			virtual ~Shape();
			/// \brief Is this object a shape?
			/// \returns true if it is a shape, false otherwise.
			static bool isShape(HH::HH_hid_t group);
			/// \brief Is this object actually a shape?
			bool isShape() const;
			/// \brief Is "group" a valid shape, according to the spec.?
			/// \param group is the HDF5 group
			/// \throws if the object pointed to by group is not a valid HDF5 group.
			/// \param out is an output stream to which diagnostic messages can be written. Diagnostics include why an object is not a shape (e.g. missing an essential parameter).
			/// \throws if the output stream is somehow invalid
			static bool isValid(HH::HH_hid_t grp, std::ostream *out = nullptr);
			/// \brief Is this object a valid shape, according to the spec?
			/// \param out is an output stream to which diagnostic messages can be written. Diagnostics include why an object is not a shape (e.g. missing an essential parameter).
			/// \throws if the output stream is somehow invalid
			bool isValid(std::ostream *out = nullptr) const;

			/// The preferred C++ type for referencing a shape
			//typedef std::shared_ptr<Shape> Shape_Type;

			/// \brief Create a new shape
			/// \param newLocationAsEmptyGroup is the empty group that is converted into a shape
			/// \throws if the group has any already-existing tables or attributes that conflict with the new shape object
			/// \param required is a pointer to the NewShapeRequiredProperties structure, that provides the "required" shape data.
			/// \param optional is a pointer to the Common Optional Properties structure, that provides optional, supplementary data.
			/// \returns the new shape on success
			/// \throws on failure
			static Shape createShape(
				HH::HH_hid_t newLocationAsEmptyGroup,
				gsl::not_null<const NewShapeRequiredProperties*> required,
				const NewShapeCommonOptionalProperties* optional = nullptr);
			static Shape createShape(
				HH::HH_hid_t baseGrpID,
				gsl::not_null<const char*> shapeGrp,
				gsl::not_null<const NewShapeRequiredProperties*> required,
				const NewShapeCommonOptionalProperties* optional = nullptr);

			/*
			Tables::Table::Table_Type getTable_ParticleScatteringElementNumber() const;
			Tables::Table::Table_Type getTable_ParticleScatteringElementCoordinates() const;
			Tables::Table::Table_Type getTable_ParticleConstituentName() const;
			Tables::Table::Table_Type getTable_ParticleScatteringElementComposition() const;

			Tables::Table::Table_Type getTable_ParticleScatteringElementRadius() const;

			Attributes::Attribute<float> getParticleScatteringElementSpacing_m() const;

			size_t getNumScatteringElements() const;
			*/
		};
	}
}
