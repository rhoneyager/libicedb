#pragma once
#include "defs.h"
#include <array>
#include <complex>
#include <string>
#include <vector>
#include <HH/Groups.hpp>
#include "registry.hpp"
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
		struct DL_ICEDB NewEXVrequiredProperties :
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
			inline bool isValid(std::ostream *errout = nullptr) const { return true; }

			float frequency_Hz = -1; ///< Frequency (Hz)
			float temperature_K = -1; ///< Particle temperature (K)
			
									  /// Constituent refractive indices.
			/// Ordering is constituent_id, constituent_name, refractive index.
			std::vector<std::tuple<int, std::string, std::complex<double> > > constituent_refractive_indices;

			struct ScattProps {
				enum class Rotation_Scheme { EULER, DDSCAT } rotation_scheme = Rotation_Scheme::EULER; ///< Ordering of the rotation angles.

				const std::vector<float> rot1; ///< First rotation angle (degrees). In Euler, Alpha. In DDSCAT, Theta.
				const std::vector<float> rot2; ///< Second rotation angle (degrees). In Euler, Beta. In DDSCAT, Phi.
				const std::vector<float> rot3; ///< Third rotation angle (degrees). In Euler, Gamma. In DDSCAT, Beta.

				const std::vector<float> incident_polar_angle; ///< Incident polar angle (degrees)
				const std::vector<float> incident_azimuth_angle; ///< Incident azimuth angle (degrees)
				const std::vector<float> scattering_polar_angle; ///< Scattering polar angle (degrees)
				const std::vector<float> scattering_azimuth_angle; ///< Scattering azimuth angle (degrees)

				const size_t numElems = 0;

				/// Type for the amplitude scattering matrices (i.e. the Jones matrix). Ordering is S11, S12, S21, S22.
				//typedef std::array<std::complex<double>, 4> amplitude_scattering_matrix_t;
				/// \brief The amplitude scattering matrix.
				/// This is horribly multi-dimensional. Dimension ordering is:
				/// [rot1][rot2][rot3][incid_polar_angle][incid_azimuth_angle][scatt_polar_angle][scatt_azimuth_angle][matrix_element_number].
				std::vector< std::complex<double> > amplitude_scattering_matrix;
				size_t get_unidimensional_index(size_t rot1, size_t rot2, size_t rot3, size_t i_p, size_t i_a,
					size_t s_p, size_t s_a, size_t asm_rank) const;
				size_t getRot_index(size_t rotNumber, float rot) const;
				size_t getAngle_index(size_t angleNumber, float angle) const;
				ScattProps() {}
				ScattProps(Rotation_Scheme scheme,
					gsl::span<const float> rot1, gsl::span<const float> rot2, gsl::span<const float> rot3,
					gsl::span<const float> incid_pol, gsl::span<const float> incid_azi,
					gsl::span<const float> scatt_pol, gsl::span<const float> scatt_azi)
					: rot1(rot1.begin(), rot1.end()), rot2(rot2.begin(), rot2.end()), rot3(rot3.begin(), rot3.end()),
					incident_polar_angle(incid_pol.begin(), incid_pol.end()),
					incident_azimuth_angle(incid_azi.begin(), incid_azi.end()),
					scattering_polar_angle(scatt_pol.begin(), scatt_pol.end()),
					scattering_azimuth_angle(scatt_azi.begin(), scatt_azi.end()),
					numElems(rot1.size() * rot2.size() * rot3.size() * incid_azi.size() * incid_pol.size()
						* scatt_azi.size() * scatt_pol.size()),
					amplitude_scattering_matrix(numElems),
					rotation_scheme(scheme)
				{}
			};
			std::vector<ScattProps> scattering_properties;
		};

		/// \brief A high-level class to manipulate extended scattering variables
		class DL_ICEDB EXV : 
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
