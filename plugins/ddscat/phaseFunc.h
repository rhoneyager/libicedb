#pragma once
#include "defs.hpp"
#pragma warning( disable : 4251 ) // DLL interface
#include <functional>
#include <complex>
#include <map>
#include <string>
#include <Eigen/Core>

#include <icedb/registry.hpp>
#include <icedb/io.hpp>

namespace icedb {
	namespace io {
		namespace ddscat {
			namespace phaseFuncs {
				struct pfRunSetContainer;
				class pfRunSetContainer_IO_input_registry {};
				class pfRunSetContainer_IO_output_registry {};
			}
		}
	}
	// Registry declarations
	namespace registry {
		extern template struct IO_class_registry_writer <
			io::ddscat::phaseFuncs::pfRunSetContainer >;

		extern template struct IO_class_registry_reader <
			io::ddscat::phaseFuncs::pfRunSetContainer >;

		extern template class usesDLLregistry <
			io::ddscat::phaseFuncs::pfRunSetContainer_IO_input_registry,
			IO_class_registry_reader<io::ddscat::phaseFuncs::pfRunSetContainer> >;

		extern template class usesDLLregistry <
			io::ddscat::phaseFuncs::pfRunSetContainer_IO_output_registry,
			IO_class_registry_writer<io::ddscat::phaseFuncs::pfRunSetContainer> >;

	}

	namespace io {
		namespace ddscat {
			/** \brief This namespace provides the different type of radiative
			* transfer matrix manipulations.
			*
			* This includes several Mueller matrix generation methods and
			* the ability to generate an extinction matrix. Eventually, Mueller matrix
			* inversion routines will also go here.
			*
			* \todo Need to move pf class elsewhere and add interpolation
			**/
			namespace phaseFuncs
			{
				/// Provides a reference to the desired phase function 
				/// routine. This is implemented to allow user choice in Mueller method.
				DL_ICEDB_IO_DDSCAT void selectMueller(const std::string &id,
					std::function<void(const Eigen::Matrix2cd&, Eigen::Matrix4d&)>&);

				// Note following conventions: matrix is [[S2, S3][S4,S1]] = [[Sn0, Sn1][Sn2, Sn3]]
				// Sn is the matrix in linear form {S1, S2, S3, S4}, so it should avoid any 
				// of the subsequent issues with forgetting the index transformations.

				DL_ICEDB_IO_DDSCAT void muellerBH(const Eigen::Matrix2cd& Sn, Eigen::Matrix4d& Snn);
				DL_ICEDB_IO_DDSCAT void muellerTMATRIX(const Eigen::Matrix2cd& Sn, Eigen::Matrix4d& Snn);

				DL_ICEDB_IO_DDSCAT void convertFtoS(const Eigen::Matrix2cd &f, Eigen::Matrix2cd& Sn, double phi,
					std::complex<double> a, std::complex<double> b, std::complex<double> c, std::complex<double> d);

				DL_ICEDB_IO_DDSCAT void invertS(const Eigen::Matrix4d &Snn, const Eigen::Matrix4d &Knn, double fGHz, Eigen::Matrix2cd& Sn);

				DL_ICEDB_IO_DDSCAT void genExtinctionMatrix(Eigen::Matrix4d &Knn, const Eigen::Matrix2cd &Sn, double fGHz);

				/// \brief This class is used for registration of phase function and cross-section
				/// providers.
				/// \note Full stats will use a connector when passed to this code.
				struct DL_ICEDB_IO_DDSCAT pf_class_registry
				{
					virtual ~pf_class_registry();
					/// Module name. Tagging is handled elsewhere.
					const char* name;

					/// Indicates whether the module handles random or aligned orientations.
					/// DEPRECATED
					//enum class orientation_type {
					//	ISOTROPIC, ORIENTED
					//} orientations;

					/// Angle setup
					struct setup {
						setup();
						double beta, theta, phi; // ddscat-based rotation angles
						double sTheta, sTheta0, sPhi, sPhi0; // incident and scattered beam angles (degrees)
						double wavelength; // wavelength of incident light
						std::string lengthUnits;
					};

					/// Used to specify basic stats for constructing a run
					struct inputParamsPartial {
						inputParamsPartial();
						double aeff; // equivalent-sphere radius
						enum class aeff_version_type
						{
							EQUIV_V_SPHERE,
							EQUIV_SA_SPHERE
						} aeff_version;
						/// Base refractive index
						std::complex<double> m;
						/// Refractive index scaling method
						std::function<void(std::complex<double>, std::complex<double>,
							double, std::complex<double> &)> rmeth;
						//std::string refrMeth;
						//std::map<std::string, double> refrVals;
						/// Rescale effective radius
						bool aeff_rescale;
						/// Volume fraction
						double vFrac;
						/// Run description (usually a shape hash to indicate the target)
						std::string ref;
						/// Maximum diameter (used by RG-based methods)
						/// Some conversion routines between effective radius and max diameter
						/// can be found in density.h.
						double maxDiamFull;
						std::string lengthUnits;

						enum class shape_type
						{
							SPHEROID,
							CYLINDER
						} shape;
						/// spheroid / cylinder aspect ratio
						double eps;

						/// Other parameters
						/// For SSRG, can set the beta, kappa and gamma values for fitting.
						std::shared_ptr<registry::options> other;
					};

					/// Cross-section return structure
					struct cross_sections {
						cross_sections();
						double Cbk, Cext, Csca, Cabs, g;
						std::string Cunits;
						// Persistant state vector
						//std::shared_ptr<nullptr> state;
						bool valid;
					};

					/// Phase function return structure
					struct pfs {
						typedef Eigen::Matrix4d PnnType;
						typedef Eigen::Matrix2cd FType;
						PnnType mueller;
						FType S;
						bool valid;
					};

					typedef std::function<void(const setup&, const inputParamsPartial&, cross_sections&)> small_c_type;
					typedef std::function<void(const setup&, const inputParamsPartial&, pfs&)> small_p_type;

					/// Get cross-sections from small stats
					small_c_type fCrossSections;
					/// Get pfs from small stats
					small_p_type fPfs;
				};

				/// Dummy class as a component for usesDLLregistry - keeps different registration types separate
				class pf_registry {};

				/// Provides phase function and cross-sectional information 
				/// from multiple sources, such as DDA, Tmatrix, ...
				///
				/// \todo Add stats-conversion code here
				class DL_ICEDB_IO_DDSCAT pf_provider :
					virtual public registry::usesDLLregistry <pf_registry, pf_class_registry >
				{
				public:
					pf_provider(const pf_class_registry::inputParamsPartial&);
					virtual ~pf_provider();

					/// \brief Find the first matching handler
					/// \param oriType specifies isotropic or oriented run.
					/// \param name forces match to a specific plugin. NULL causes this parameter to be skipped.
					/// \param res is a container for all matching modules.
					static void findHandler(
						const char* name, const pf_class_registry *res);


					typedef std::vector<std::pair<const char*, pf_class_registry::cross_sections> > resCtype;
					typedef std::vector<std::pair<const char*, pf_class_registry::pfs> > resPtype;
					void getCrossSections(const pf_class_registry::setup&, resCtype& res, const std::string &forceProvider = "") const;
					void getPfs(const pf_class_registry::setup&, resPtype& res, const std::string &forceProvider = "") const;
				private:
					const pf_class_registry::inputParamsPartial& iparams;
				};

				struct DL_ICEDB_IO_DDSCAT pfRunSetContainer :
					virtual public registry::usesDLLregistry<
					pfRunSetContainer_IO_input_registry,
					registry::IO_class_registry_reader<pfRunSetContainer> >,
					virtual public registry::usesDLLregistry<
					pfRunSetContainer_IO_output_registry,
					registry::IO_class_registry_writer<pfRunSetContainer> >,
					virtual public io::implementsStandardWriter<pfRunSetContainer, pfRunSetContainer_IO_output_registry>,
					virtual public io::implementsStandardReader < pfRunSetContainer, pfRunSetContainer_IO_input_registry >//,
				{
					pfRunSetContainer();
					virtual ~pfRunSetContainer();

					struct csContainer
					{
						const char* providerName;
						pf_class_registry::setup setup;
						pf_class_registry::inputParamsPartial i;
						pf_class_registry::cross_sections cs;
					};
					std::vector<csContainer> runs;
					/// When the run was imported
					std::string ingest_timestamp;
					/// The system that the run was imported on
					//std::string ingest_hostname;
					/// The user account that imported the run
					//std::string ingest_username;
					/// Revision of the code for ingest
					std::string ingest_code_version;
					/// Version of the phaseFunc code
					int phaseFunc_version;
					/// Current version of the phasefunc code
					const static int max_phaseFunc_version;
					/// Should the results be recalculated in the newest version?
					//bool needsUpgrade() const;
					/// Recalculate all results, using the newest version of the code
					//void upgrade();
				private:
					void _init();
				};
			}

		}
	}
}
