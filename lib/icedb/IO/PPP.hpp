#pragma once
#include "../defs.h"
#include <array>
#include <functional>
#include <vector>
#include <HH/Groups.hpp>
#include "Common.hpp"
#include "../Plugins/registry.hpp"
#include "io.hpp"

namespace icedb {
	namespace ShapeAlgs {
		namespace Algorithms {
			class Algorithm;
		}
	}
	namespace PPP {
		class PPP;
		
		enum class e_Common_Obj_Names {
			// Attributes
			icedb_Version, Schema_Version, icedb_git_hash, particle_id,
			dataset_id, author, contact, version,

			// Dimensions - these get copied over because the PPP group might not be a 
			// child of the shape object. It might reside in a separate file.
			particle_scattering_element_number,
			particle_constituent_number,
			particle_axis
		};
		/// Each PPP 'group' has an attribute with this identifier.
		extern ICEDB_DL const std::string _obj_type_identifier;
		extern ICEDB_DL const std::map<e_Common_Obj_Names, Name_Type_t > Required_Atts;
		extern ICEDB_DL const std::map<e_Common_Obj_Names, Name_Type_t > Required_Dsets;

		namespace _impl {
			class PPP_IO_Output_Registry {};
		}
	}
	namespace registry {
		extern template struct IO_class_registry_writer <
			::icedb::PPP::PPP >;
		extern template class usesDLLregistry <
			::icedb::PPP::_impl::PPP_IO_Output_Registry,
			IO_class_registry_writer<::icedb::PPP::PPP> >;
	}

	template<>
	struct Common_HH_Base < PPP::PPP, PPP::e_Common_Obj_Names>
	{
		using CN = PPP::e_Common_Obj_Names;
		typedef const std::map<CN, Name_Type_t> Validator_map_t;
		static Validator_map_t getRequiredAttributes() { return PPP::Required_Atts; }
		static Validator_map_t getRequiredDatasets() { return PPP::Required_Dsets; }
		static std::string getIdentifier() { return PPP::_obj_type_identifier; }
	};

	/// All facilities to manipulate particle physical properties
	namespace PPP {

		/// \brief A high-level class to manipulate particle shapes
		///
		/// Shapes are implemented as a set of tables and attributes, contained within a discrete Group.
		/// This class provides a higl-level interface to accessing and manipulating shapes.
		/// It acts as an "overlay" to an alreagy-existing group. It adds additional functions and "value".
		class ICEDB_DL PPP :
			virtual public HH::Group,
			virtual public registry::usesDLLregistry<
			_impl::PPP_IO_Output_Registry,
			registry::IO_class_registry_writer<PPP> >,
			virtual public io::implementsStandardWriter<PPP, _impl::PPP_IO_Output_Registry>
		{
		public:
			virtual ~PPP() {}
			//static const std::string _obj_type_identifier;
			static const uint16_t _current_schema_version;

			/// OPEN an HDF5 handle as a PPP. Validity not guaranteed.
			PPP(HH::Group grp = HH::HH_hid_t::dummy()) : HH::Group(grp) {  }

			/// \brief Create a new PPP group
			/// \param newLocationAsEmptyGroup is the empty group that is converted into a PPP
			/// \param srcShape is the source shape.
			/// \throws if the group has any already-existing tables or attributes that conflict with the new PPP object
			/// \returns the new PPP on success
			/// \throws on failure
			static PPP createPPP(
				HH::Group newLocationAsEmptyGroup,
				const HH::Group& srcShape);

			/// \brief Apply an algorithm to this PPP (convenience function).
			PPP apply(
				const std::vector< std::reference_wrapper<const ShapeAlgs::Algorithms::Algorithm>> &alg, 
				const HH::Group& shp = {});
			PPP apply(
				gsl::span<const ShapeAlgs::Algorithms::Algorithm> alg, 
				const HH::Group& shp = {});
			PPP apply(
				const ShapeAlgs::Algorithms::Algorithm& alg, 
				const HH::Group& shp = {});

			// Find algorithms to generate the requested datasets and attributes.
			// returns an ordered vector of algorithmic operations that will produce the desired result.
			// param alg_set is the map of algorithms that will be searched.
			// param shp is the source shape, if available.
			/*
			std::vector<std::reference_wrapper<const ShapeAlgs::Algorithms::Algorithm>>
				findAlgs(
					const std::vector<std::string>& requiredAtts = {},
					const std::vector<std::string>& requiredDsets = {},
					const std::vector<std::string>& optionalAtts = {},
					const std::vector<std::string>& optionalDsets = {}
					) const;
					*/
		};
	}
}
