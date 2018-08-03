#pragma once
#include "defs.h"
#include <array>
#include <complex>
#include <string>
#include <vector>
#include <HH/Groups.hpp>

namespace icedb {
	namespace exv {
		/// Structure containing all of the required data for the extended scattering properties of an object
		struct DL_ICEDB NewEXVrequiredProperties {
			std::string particle_id;				///< Particle id
			std::string dataset_id;					///< Dataset id
			std::string author;						///< The author
			std::string contact;					///< Contact info
			std::array<unsigned int, 3> version;	///< Version information

			/// Validate that all required properties are set, and that they have the correct dimensions.
			/// Writes diagnostic messages to the output stream.
			/// \todo Write this function
			inline bool isValid(std::ostream *errout = nullptr) const { return true; }

			float frequency_Hz; ///< Frequency (Hz)
			float temperature_K; ///< Particle temperature (K)

			/// Constituent refractive indices.
			/// Ordering is constituent_id, constituent_name, refractive index.
			std::vector<std::tuple<int, std::string, std::complex<double> > > constituent_refractive_indices;

			struct angle_obs {
				float incident_polar_angle; ///< Incident polar angle (degrees)
				float incident_azimuth_angle; ///< Incident azimuth angle (degrees)
				float scattering_polar_angle; ///< Scattering polar angle (degrees)
				float scattering_azimuth_angle; ///< Scattering azimuth angle (degrees)
				/// Type for the amplitude scattering matrices (i.e. the Jones matrix). Ordering is S1, S2, S3, S4.
				typedef std::array<std::complex<double>, 4> amplitude_scattering_matrix_t;
				amplitude_scattering_matrix_t amplitude_scattering_matrix;
			};
			/// The observations for each set of angles.
			std::vector<angle_obs> angles;
		};

		/// \brief A high-level class to manipulate extended scattering variables
		class DL_ICEDB EXV : virtual public HH::Group
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
				gsl::not_null<const char*> exvGrp,
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
