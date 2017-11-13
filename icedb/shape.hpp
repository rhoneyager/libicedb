#pragma once
#include "fs.hpp"
#include "Table.hpp"
#include "Attribute.hpp"
#include "Group.hpp"

namespace icedb {
	namespace Shapes {
		struct NewShapeRequiredProperties {
			// The DIMENSIONS

			/// DIMENSION: The id number for each scattering element. Single dimension.
			gsl::span<uint64_t> particle_scattering_element_number;
			/// DIMENSION: The id number of each particle's constituent. Single dimension.
			gsl::span<uint64_t> particle_constituent_number;

			// The VARIABLES

			/// VARIABLE: Cartesian coordinates of the center of each scattering element
			/// Written in form of x_1, y_1, z_1, x_2, y_2, z_2, ...
			/// Dimensions of [particle_scattering_element_number][axis]
			gsl::span<float> particle_scattering_element_coordinates;
			/// VARIABLE: Mass fractions of each constituent for each scattering element.
			/// Dimensions of [particle_scattering_element_number][particle_constituent_number]
			gsl::span<float> particle_scattering_element_composition;

			// The ATTRIBUTES

			/// ATTRIBUTE: Unique Particle Identifier
			std::string particle_id;


			/// Validate that all required properties are set, and that they have the correct dimensions.
			/// Writes diagnostic messages to the output stream.
			bool isValid(std::ostream *errout = nullptr) const;
		};
		struct NewShapeCommonOptionalProperties {
			/// OPTIONAL VARIABLE: Physical radius of the scattering sphere, in meters.
			/// Dimensions: [particle_scattering_element_number]
			gsl::span<float> particle_scattering_element_radius;

			/// VARIABLE: The name of each particle's constituent. Single dimension.
			/// Dimensions of [particle_constituent_number]
			/// \note Currently ignored because I have to redo the HDF5 string writing functions.
			/// \todo Make this required.
			gsl::span<std::string> particle_constituent_name;

			/// OPTIONAL ATTRIBUTE: Physical spacing between adjacent grid points. Used in DDA.
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