#pragma once
#include "fs.hpp"
#include "Table.hpp"
#include "Attribute.hpp"
#include "Group.hpp"

namespace icedb {
	namespace Shapes {
		struct NewShapeRequiredProperties {
			// The DIMENSIONS

			/// DIMENSION: The number of scattering elements.
			/// If these have non-trivial ids (i.e. not 1, 2, 3, 4, ...), then define the
			/// optional attribute particle_scattering_element_number.
			uint64_t number_of_particle_scattering_elements;

			/// DIMENSION: The number of distinct constituents in the particle.
			/// If these have non-trivial ids (i.e. not 1, 2, 3, 4, ...), then define the
			/// optional attribute particle_constituent_number.
			uint64_t number_of_particle_constituents;

			// Each particle has three axes (X, Y and Z). As these are trivial, only the
			// scale lables are written; the scale does not have an explicit dataset filled with values.
			
			// The VARIABLES

			/// VARIABLE: Cartesian coordinates of the center of each scattering element
			/// Written in form of x_1, y_1, z_1, x_2, y_2, z_2, ...
			/// Dimensions of [number_of_particle_scattering_elements][axis]
			gsl::span<const float> particle_scattering_element_coordinates;
			

			// The ATTRIBUTES

			/// ATTRIBUTE: Unique Particle Identifier
			std::string particle_id;


			/// Validate that all required properties are set, and that they have the correct dimensions.
			/// Writes diagnostic messages to the output stream.
			bool isValid(std::ostream *errout = nullptr) const;

			/// Based on the required properties, must the optional properties structure be required?
			/// i.e. Extra information is needed to properly construct the shape.
			bool requiresOptionalPropertiesStruct() const;
		};
		struct NewShapeCommonOptionalProperties {
			/// DIMENSION: The id number for each scattering element. Single dimension.
			gsl::span<const uint64_t> particle_scattering_element_number;
			/// DIMENSION: The id number of each particle's constituent. Single dimension.
			gsl::span<const uint64_t> particle_constituent_number;


			/// OPTIONAL VARIABLE: Physical radius of the scattering sphere, in meters.
			/// Dimensions: [particle_scattering_element_number]
			gsl::span<const float> particle_scattering_element_radius;

			/// VARIABLE: The name of each particle's constituent. Single dimension.
			/// Becomes REQUIRED if there is more than one constituent. If there is
			/// only one constituent, then this constituent can be written as an attribute,
			/// or it can be just assumed to be 'ice'.
			/// Dimensions of [particle_constituent_number]
			/// \note Currently ignored because I have to redo the HDF5 string writing functions.
			gsl::span<const std::string> particle_constituent_name;

			/// OPTIONAL ATTRIBUTE: The name of the single constituent of the particle
			/// Used only if there is a single constituent, and if this constituent is NOT ice.
			std::string particle_constituent_single_name;

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
			gsl::span<const uint64_t> particle_scattering_element_composition_whole;

			/// OPTIONAL ATTRIBUTE: Physical spacing between adjacent grid points (in meters). Used in DDA.
			float particle_scattering_element_spacing = -1;


			/// Validate that all required properties are set, and that they have the correct dimensions.
			/// Writes diagnostic messages to the output stream.
			bool isValid(gsl::not_null<const NewShapeRequiredProperties*> required, std::ostream *errout = nullptr) const;
		};

		class Shape : virtual public Groups::Group
		{
		protected:
			Shape(const std::string &uid);
		public:
			static std::string _icedb_obj_type_shape_identifier;
			virtual ~Shape();
			static bool isShape(Groups::Group &owner, const std::string &name);
			static bool isShape(gsl::not_null<H5::Group*> group);
			bool isShape() const;
			static bool isValid(gsl::not_null<H5::Group*> group, std::ostream *out = nullptr);
			bool isValid(std::ostream *out = nullptr) const;
			typedef std::unique_ptr<Shape> Shape_Type;

			static Shape_Type openShape(Groups::Group &grpshp);
			static Shape_Type openShape(Groups::Group &owner, const std::string &name);
			static Shape_Type openShape(Groups::Group::Group_HDF_shared_ptr shape);

			static Shape_Type createShape(Groups::Group &grpshp, const std::string &uid, 
				gsl::not_null<const NewShapeRequiredProperties*> required,
				const NewShapeCommonOptionalProperties* optional = nullptr);
			static Shape_Type createShape(Groups::Group &owner, const std::string &name,
				const std::string &uid,
				gsl::not_null<const NewShapeRequiredProperties*> required,
				const NewShapeCommonOptionalProperties* optional = nullptr);
			static Shape_Type createShape(Groups::Group::Group_HDF_shared_ptr newShapeLocation,
				const std::string &uid,
				gsl::not_null<const NewShapeRequiredProperties*> required,
				const NewShapeCommonOptionalProperties* optional = nullptr);

			const std::string particle_unique_id;

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