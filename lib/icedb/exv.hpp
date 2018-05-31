#pragma once
#include <array>
#include "Table.hpp"
#include "Attribute.hpp"
#include "Group.hpp"

namespace icedb {
	/// Contains all of the methods to manipulate the extended scattering variables
	namespace EXV {
		/// Structure for all of the required properties needed to write an extended scattering variables table.
		struct NewEXVRequiredProperties {
			/** @name Dimensions
			*  These are dimensions.
			*/
			///@{



			///@}

			/** @name Variables
			*  These are variables.
			*/
			///@{
			///@}
			/** @name Attributes
			*  These are attributes.
			*/
			///@{
			/// The unique identifier for the particle / dataset / version.
			std::string structure_id_standard;
			///@}
		};
	}
}