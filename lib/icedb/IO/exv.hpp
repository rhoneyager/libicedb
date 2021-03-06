#pragma once
#include "../defs.h"
#include <array>
#include <complex>
#include <string>
#include <vector>
#include <HH/Groups.hpp>
#include "../Plugins/registry.hpp"
#include "io.hpp"

namespace icedb {
	namespace exv {
		struct NewEXVrequiredProperties;
		class EXV;
		namespace _impl {
			class EXVProps_IO_Input_Registry {};
			class EXV_IO_Output_Registry {};
		}
	}
	namespace registry {
		extern template struct IO_class_registry_writer <
			::icedb::exv::EXV >;
		extern template struct IO_class_registry_reader <
			::icedb::exv::NewEXVrequiredProperties >;
		extern template class usesDLLregistry <
			::icedb::exv::_impl::EXVProps_IO_Input_Registry,
			IO_class_registry_reader<::icedb::exv::NewEXVrequiredProperties> >;
		extern template class usesDLLregistry <
			::icedb::exv::_impl::EXV_IO_Output_Registry,
			IO_class_registry_writer<::icedb::exv::EXV> >;
	}
	namespace exv {
		/// Structure containing all of the required data for the extended scattering properties of an object
		struct ICEDB_DL NewEXVrequiredProperties :
			virtual public registry::usesDLLregistry<
			_impl::EXVProps_IO_Input_Registry,
			registry::IO_class_registry_reader<NewEXVrequiredProperties> >,
			virtual public io::implementsStandardReader<NewEXVrequiredProperties, _impl::EXVProps_IO_Input_Registry>
		{
			std::string particle_id;				///< Particle id
			std::string dataset_id;					///< Dataset id
			std::string author;						///< The author
			std::string contact;					///< Contact info
			std::string scattMeth;					///< Scattering method
			std::string ingest_timestamp;			///< Ingest timestamp
			std::array<unsigned int, 3> version;	///< Version information

			/// Validate that all required properties are set, and that they have the correct dimensions.
			/// Writes diagnostic messages to the output stream.
			/// \todo Write this function
			inline bool isValid(std::ostream * = nullptr) const { return true; }

			float frequency_Hz = -1; ///< Frequency (Hz)
			float temperature_K = -1; ///< Particle temperature (K)

			float alpha = -1; ///< First rotation angle (degrees)
			float beta = -1; ///< Second rotation angle (degrees)
			float gamma = -1; ///< Third rotation angle (degrees)

			/// Constituent refractive indices.
			/// Ordering is constituent_id, constituent_name, refractive index.
			std::vector<std::tuple<int, std::string, std::complex<double> > > constituent_refractive_indices;

			std::vector<float> incident_polar_angle; ///< Incident polar angle (degrees)
			std::vector<float> incident_azimuth_angle; ///< Incident azimuth angle (degrees)
			std::vector<float> scattering_polar_angle; ///< Scattering polar angle (degrees)
			std::vector<float> scattering_azimuth_angle; ///< Scattering azimuth angle (degrees)

			/// Type for the amplitude scattering matrices (i.e. the Jones matrix). Ordering is S11, S12, S21, S22.
			typedef std::array<std::complex<double>, 4> amplitude_scattering_matrix_t;
			/// \brief The amplitude scattering matrix.
			/// \todo Clarify the dimension ordering.
			std::vector< amplitude_scattering_matrix_t> amplitude_scattering_matrix;
		};

		/// \brief A high-level class to manipulate extended scattering variables
		class ICEDB_DL EXV : 
			virtual public HH::Group,
			virtual public registry::usesDLLregistry<
			_impl::EXV_IO_Output_Registry,
			registry::IO_class_registry_writer<EXV> >,
			virtual public io::implementsStandardWriter<EXV, _impl::EXV_IO_Output_Registry>
		{
		public:
			static const std::string _icedb_obj_type_exv_identifier;
			static const uint16_t _icedb_current_exv_schema_version;

			// OPEN an already-existing exv. Validity not guaranteed.
			EXV(HH::HH_hid_t hnd_grp) : HH::Group(hnd_grp) {  }

			const std::string particle_id;

			virtual ~EXV() {}
			/// \brief Is this object an EXV?
			/// \returns true if it is, false otherwise.
			static bool isEXV(HH::HH_hid_t group);
			/// \brief Is this object actually an EXV object?
			inline bool isEXV() const { return isEXV(this->get());  }
			/// \brief Is "group" a valid EXV object, according to the spec.?
			/// \param group is the HDF5 group
			/// \throws if the object pointed to by group is not a valid HDF5 group.
			/// \param out is an output stream to which diagnostic messages can be written. Diagnostics include why an object is not a shape (e.g. missing an essential parameter).
			/// \throws if the output stream is somehow invalid
			static bool isValid(HH::HH_hid_t grp, std::ostream *out = nullptr);
			/// \brief Is this object valid, according to the spec?
			/// \param out is an output stream to which diagnostic messages can be written. Diagnostics include why an object is not a shape (e.g. missing an essential parameter).
			/// \throws if the output stream is somehow invalid
			inline bool isValid(std::ostream *out = nullptr) const { return isValid(this->get(), out); }

			/// \brief Create a new EXV object
			/// \param newLocationAsEmptyGroup is the empty group that is converted into a shape
			/// \throws if the group has any already-existing tables or attributes that conflict with the new shape object
			/// \param required is a pointer to the NewShapeRequiredProperties structure, that provides the "required" shape data.
			/// \param optional is a pointer to the Common Optional Properties structure, that provides optional, supplementary data.
			/// \returns the new shape on success
			/// \throws on failure
			static EXV createEXV(
				HH::HH_hid_t newLocationAsEmptyGroup,
				gsl::not_null<const NewEXVrequiredProperties*> data);
			
			static EXV createEXV(
				HH::HH_hid_t baseGrpID,
				const std::string& exvGrp,
				gsl::not_null<const NewEXVrequiredProperties*> data)
			{
				HH::Group base(baseGrpID);
				if (base.exists(exvGrp)) {
					const auto grp = base.open(exvGrp);
					return createEXV(grp.get(), data);
				}
				else {
					const auto grp = base.create(exvGrp);
					return createEXV(grp.get(), data);
				}
			}
		};
	}
}
