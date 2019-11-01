#pragma once
#include "defs.hpp"
#include "plugin-io-ddscat.hpp"
#include <array>
#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <set>
#include <complex>
#include <memory>

#include <Eigen/Core>
#include <Eigen/Dense>

#include "icedb/Plugins/registry.hpp"
#include "icedb/Utils/splitSet.hpp"
#include "icedb/IO/io.hpp"

#if defined(_MSC_FULL_VER)
#pragma warning(push)
#pragma warning( disable : 4251 ) // DLL interface
#pragma warning( disable : 4661 ) // Exporting vector
#endif

namespace boost { namespace filesystem { class path; } }


namespace icedb {
	namespace io {
		namespace ddscat {
			namespace shapefile {
				class shapefile_index;
				class shapefile;
			}
			class ddOutput;
			class ddOutput_IO_output_registry {};
			class ddOutput_IO_input_registry {};
			//class ddOutput_serialization {};
			namespace weights { class OrientationWeights3d; }
		}
	}
}
namespace icedb {
	namespace registry {
		extern template struct IO_class_registry_writer <
			::icedb::io::ddscat::ddOutput > ;

		extern template class usesDLLregistry <
			::icedb::io::ddscat::ddOutput_IO_output_registry,
			IO_class_registry_writer<::icedb::io::ddscat::ddOutput> > ;

		extern template struct IO_class_registry_reader <
			::icedb::io::ddscat::ddOutput > ;

		extern template class usesDLLregistry <
			::icedb::io::ddscat::ddOutput_IO_input_registry,
			IO_class_registry_reader<::icedb::io::ddscat::ddOutput> >;
	}
}
namespace icedb {
	namespace io {
		namespace ddscat {

			class ddOriData;
			//class ddOutputGenerator;
			namespace shapefile { class shapefile; }
			class ddPar;
			//class ddOutputGeneratorConnector;

			/** \brief Expresses the result of a ddscat run.
			 *
			 * This class can contain the complete output of a ddscat run. This includes
			 * the shapefile, stats, sca, fml and avg files.
			 *
			 * This provides output for a single frequency, refractive index set and
			 * effective radius. The most general type of ddscat run may contain many
			 * permutations of these.
			 **/
			class ICEDB_DL_IO_DDSCAT ddOutput :
				virtual public ::icedb::registry::usesDLLregistry<
					::icedb::io::ddscat::ddOutput_IO_output_registry,
					::icedb::registry::IO_class_registry_writer<::icedb::io::ddscat::ddOutput> >,
				virtual public ::icedb::io::implementsStandardWriter<ddOutput, ddOutput_IO_output_registry>,
				virtual public ::icedb::registry::usesDLLregistry<
					::icedb::io::ddscat::ddOutput_IO_input_registry,
					::icedb::registry::IO_class_registry_reader<::icedb::io::ddscat::ddOutput> >,
				virtual public ::icedb::io::implementsStandardReader<ddOutput, ddOutput_IO_input_registry>
			{
				void resize(size_t numOris, size_t numTotAngles);
				void resizeFML(size_t numTotAngles);
				void finalize();
				std::mutex mtxUpdate;
				friend class ddOriData;
			public:
				ddOutput();
				ddOutput(const ddOutput&);

				
				/// Regenerates ddOutputSingle entries from tables (used in hdf5 read)
				//void doImport();
				/// A brief description of the run
				std::string description;
				/// When the run was imported
				std::string ingest_timestamp;
				/// The system that the run was imported on
				//std::string ingest_hostname;
				/// The user account that imported the run
				//std::string ingest_username;
				/// The host that the run was on
				//std::string hostname;
				/// Revision of the rtmath code for ingest
				//int ingest_rtmath_version;
				/// Frequency
				double freq;
				/// Effective radius
				double aeff;
				/// Temperature
				double temp;
				std::string units_length = "um";
				std::array<unsigned int, 3> version = std::array<unsigned int, 3>{ 1, 0, 0 };
				std::string units_temperature = "K";
				std::string author, contact, scattMeth, datasetID;

				/// Paths of source files. Used in consolidation.
				std::multiset<std::string> sources;
				/// User-set brief description snippets. Used in isolating sets of runs.
				std::multimap<std::string, std::string> tags;
				/// DDSCAT run version tag
				std::string ddvertag;
				/// Encapsulating enum in namespace, as an enum class is too restrictive
				class stat_entries {
					/// \note Every time these are changed, the stringify code also needs to be updated
					/// \see stringify
				public:
					enum stat_entries_doubles
					{
						// D/AEFF
						D,
						////////XMIN, XMAX, YMIN, YMAX, ZMIN, ZMAX, 
						AEFF, WAVE, ////////FREQ,
						// K*AEFF
						//NAMBIENT,
						//TOL,
						////////////TA1TFX, TA1TFY, TA1TFZ,
						////////////TA2TFX, TA2TFY, TA2TFZ,
						TFKX, TFKY, TFKZ,
						IPV1TFXR, IPV1TFXI, IPV1TFYR, IPV1TFYI, IPV1TFZR, IPV1TFZI,
						IPV2TFXR, IPV2TFXI, IPV2TFYR, IPV2TFYI, IPV2TFZR, IPV2TFZI,
						TA1LFX, TA1LFY, TA1LFZ,
						TA2LFX, TA2LFY, TA2LFZ,
						////////////LFKX, LFKY, LFKZ,
						////////////IPV1LFXR, IPV1LFXI, IPV1LFYR, IPV1LFYI, IPV1LFZR, IPV1LFZI,
						////////////IPV2LFXR, IPV2LFXI, IPV2LFYR, IPV2LFYI, IPV2LFZR, IPV2LFZI,
						BETA, THETA, PHI, //ETASCA,
						QEXT1, QABS1, QSCA1, G11, G21, QBK1, QPHA1,
						QEXT2, QABS2, QSCA2, G12, G22, QBK2, QPHA2,
						QEXTM, QABSM, QSCAM, G1M, G2M, QBKM, QPHAM,
						QPOL, DQPHA,
						QSCAG11, QSCAG21, QSCAG31, ITER1, MXITER1, NSCA1,
						QSCAG12, QSCAG22, QSCAG32, ITER2, MXITER2, NSCA2,
						QSCAG1M, QSCAG2M, QSCAG3M,
						///////////////DOWEIGHT,
						NUM_STAT_ENTRIES_DOUBLES
					};

					static ICEDB_DL_IO_DDSCAT std::string stringify(int val);
				};

				struct shared_data
				{
					shared_data();
					size_t version, num_dipoles, navg;
					std::string target; // , ddameth, ccgmeth, hdr_shape;
					std::array<double, 3> mins;
					std::array<double, 3> maxs;
					std::array<double, 3> TA1TF;
					std::array<double, 3> TA2TF;
					std::array<double, 3> LFK;
					std::array<std::complex<double>, 3> IPV1LF, IPV2LF;
					//size_t iter1, mxiter1, nsca1,
					//	   iter2, mxiter2, nsca2;
				} s;

				typedef Eigen::Matrix<double, Eigen::Dynamic, stat_entries::NUM_STAT_ENTRIES_DOUBLES> doubleType;
				/// Table containing orientation data (cross-sections, etc.)
				/// Set when listing folder.
				doubleType oridata_d;

				/// Refractive indices
				std::vector<std::vector<std::complex<double> > > ms;

				struct Avgdata
				{
					double beta_min, beta_max, theta_min, theta_max, phi_min, phi_max;
					// NOTE: num_avg_entries missing for data assimilated before 2016/2/2.
					size_t beta_n, theta_n, phi_n, num_avg_entries;
					bool hasAvg;
					std::vector < std::complex<double> > avg_ms;

					/// \brief Storage for avg file data - used in reading runs that aren't mine
					/// \note Using Eigen::Dynamic to preserve type compatabiity with oridata_d
					doubleType avg;
					Avgdata();

					/// Encapsulating enum in namespace, as an enum class is too restrictive
					class avgScaColDefs
					{
					public:
						/// Table containing fml data
						enum avgScaDefs
						{
							/// Scattering-angle specific
							THETAB, PHIB, POL,
							S11, S12, S13, S14, S21, S22, S23, S24,
							S31, S32, S33, S34, S41, S42, S43, S44,
							NUM_AVGSCACOLDEFS
						};
						static ICEDB_DL_IO_DDSCAT std::string stringify(int val);
					};
					/// Table containing mueller data. Delayed allocation because the size resides within a file being read.
					std::shared_ptr<Eigen::Matrix<float, Eigen::Dynamic,
						avgScaColDefs::NUM_AVGSCACOLDEFS> > pdata;

					/*
					enum avg_entries {
						BETA_MIN, BETA_MAX, BETA_N,
						THETA_MIN, THETA_MAX, THETA_N,
						PHI_MIN, PHI_MAX, PHI_N,
						NUM_AVG_ENTRIES
					};

					typedef Eigen::Matrix<double, Eigen::Dynamic, NUM_AVG_ENTRIES> avgType;
					/// Table containing orientation data (cross-sections, etc.)
					/// Set when listing folder.
					avgType d;
					*/
				} avgdata;

				/// Number of stored orientations
				size_t numOriData;

				/// Encapsulating enum in namespace, as an enum class is too restrictive
				class fmlColDefs
				{
				public:
					/// Table containing fml data
					enum fmlDefs
					{
						/// Match to a ORI index (TODO: use an integer)
						ORIINDEX,
						/// Scattering-angle specific
						THETAB, PHIB,
						F00R, F00I, F01R, F01I, F10R, F10I, F11R, F11I,
						NUM_FMLCOLDEFS
					};
					static ICEDB_DL_IO_DDSCAT std::string stringify(int val);
				};
				/// Table containing fml data. Delayed allocation because the size resides within a file being read.
				std::shared_ptr<Eigen::Matrix<float, Eigen::Dynamic, fmlColDefs::NUM_FMLCOLDEFS> > fmldata;

				/// Find row matching the specified orientation.
				bool getRow(double beta, double theta, double phi, size_t &row) const;

				/// The shape file (may load fully later)
				mutable std::shared_ptr<const ::icedb::io::ddscat::shapefile::shapefile> shape;
				
				/// The ddscat parameter file
				std::shared_ptr<ddPar> parfile;

				/// Expand output to a given directory
				void expand(const std::string &outdir, bool writeShape = false); // const;


				/// Generate ddOutput from a ddscat output directory
				static std::shared_ptr<ddOutput> generate(
					const std::string &dir, bool noLoadRots = false);
				// Generate ddOutput from raw files
				static std::shared_ptr<ddOutput> generate(
					const std::string &avgfile, const std::string &parfile,
					const std::string &shpfile);
			};

		}
	}
}
#if defined(_MSC_FULL_VER)
#pragma warning(pop)
#endif

