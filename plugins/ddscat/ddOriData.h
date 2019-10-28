#pragma once
#include "plugin-io-ddscat.hpp"
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <mutex>
#include <complex>
#include <Eigen/Core>
#include <Eigen/Dense>
#include "ddOutput.h"
#include "ddVersions.h"
#include "ddScattMatrix.h"

namespace icedb {
	namespace io {
		namespace ddscat {
			class ddOriData;
			class rotations;
			class ddOriData_IO_input_registry {};
			class ddOriData_IO_output_registry {};
			class ddOriData_Standard {};
			class ddScattMatrixConnector;
		}
	}
}
namespace icedb {
	namespace registry {
		extern template struct IO_class_registry_writer <
			::icedb::io::ddscat::ddOriData > ;

		extern template class usesDLLregistry <
			::icedb::io::ddscat::ddOriData_IO_output_registry,
			IO_class_registry_writer<::icedb::io::ddscat::ddOriData> > ;

		extern template struct IO_class_registry_reader <
			::icedb::io::ddscat::ddOriData > ;

		extern template class usesDLLregistry <
			::icedb::io::ddscat::ddOriData_IO_input_registry,
			IO_class_registry_reader<::icedb::io::ddscat::ddOriData> > ;
	}
}
namespace icedb {
	namespace io {
		namespace ddscat {

			/// \brief Converts the stat_entries id to a string for display.
			/// \todo Add reverse case, converting from string to id.
			//std::string ICEDB_DL getStatNameFromId(stat_entries);
			//std::string ICEDB_DL getStatNameFromId(stat_entries_size_ts);

			/// Provides local readers and writers for ddscat data (it's a binder)
			class ICEDB_DL_IO_DDSCAT implementsDDRES :
				private icedb::io::implementsIObasic<ddOriData, ddOriData_IO_output_registry,
				ddOriData_IO_input_registry, ddOriData_Standard>
			{
			public:
				virtual ~implementsDDRES() {}
			protected:
				implementsDDRES();
			private:
				static const std::set<std::string>& known_formats();
			};


			/** Class contains the output of a single ddscat fml / sca or avg file
			* This means one radius, wavelength, and orientation.
			**/
			class ICEDB_DL_IO_DDSCAT ddOriData :
				virtual public std::enable_shared_from_this<ddOriData>,
				virtual public ::icedb::registry::usesDLLregistry<
					::icedb::io::ddscat::ddOriData_IO_output_registry,
					::icedb::registry::IO_class_registry_writer<ddOriData> >,
				virtual public ::icedb::registry::usesDLLregistry<
					::icedb::io::ddscat::ddOriData_IO_input_registry,
					::icedb::registry::IO_class_registry_reader<ddOriData> >,
				virtual public ::icedb::io::implementsStandardWriter<ddOriData, ddOriData_IO_output_registry>,
				virtual public ::icedb::io::implementsStandardSingleReader<ddOriData, ddOriData_IO_input_registry>,
				virtual public implementsDDRES
			{
				friend class ddOutput;

				void doExportFMLs(size_t startIndex) const;
				void doImportFMLs(size_t startIndex, size_t n);
				void doImportFMLs();
				ddOriData(ddOutput &parent, size_t row, bool isAvg);
			public:
				static std::shared_ptr<ddOriData> generate(ddOutput &parent, size_t row,
					const std::string &filenameSCA = "", const std::string &filenameFML = "");
				static std::shared_ptr<ddOriData> generate(ddOutput &parent, const std::string &filenameAVG = "");
				virtual ~ddOriData();

				// Binders for the standard ddscat formats
				static void readDDSCAT(std::shared_ptr<ddOriData>, std::istream&, std::shared_ptr<icedb::registry::IO_options>);
				static void writeDDSCAT(const ddOriData*, std::ostream &, std::shared_ptr<icedb::registry::IO_options>);

				double guessTemp(size_t dielIndex = 0) const;

				typedef std::vector<std::pair<size_t, size_t> > mMuellerIndices;

				void writeFML(std::ostream &out) const;
				void writeSCA(std::ostream &out) const;
				void writeAVG(std::ostream &out) const;
				static const mMuellerIndices& mMuellerIndicesDefault();
				void writeMuellerDDSCAT(std::ostream &out, const mMuellerIndices &mi = mMuellerIndicesDefault()) const;
				void writeS(std::ostream &out) const;
				void writeF_DDSCAT(std::ostream &out) const;
				void writeStatTable(std::ostream &out) const;

				void readFML(std::istream &in);
				void readSCA(std::istream &in);
				void readAVG(std::istream &in);
				//void readHeader(std::istream &in, const std::string &sstop = "Qext");
				void readStatTable(std::istream &in);
				void readMuellerDDSCAT(std::istream &in);
				void readF_DDSCAT(std::istream &in, std::shared_ptr<const ddScattMatrixConnector>);

				void readS_ADDA(std::istream &in);


#define accessorRW(name,id,valtype) \
	inline valtype name() const { return __getSimpleDouble((int) id); } \
	inline void name(const valtype &v) { __setSimpleDouble((int) id, v); }
#define accessorRO(name,id,valtype) \
	inline valtype name() const { return __getSimpleDouble((int) id); }

				size_t version() const;
				//accessorRW(version, ddOutput::stat_entries::VERSION, size_t);
				accessorRW(beta, ddOutput::stat_entries::BETA, double);
				accessorRW(theta, ddOutput::stat_entries::THETA, double);
				accessorRW(phi, ddOutput::stat_entries::PHI, double);
				// Wavelength and frequency set need a special override (affects WAVE, FREQ and D)
				accessorRO(wave, ddOutput::stat_entries::WAVE, double);
				double freq() const;
				//accessorRO(freq, ddOutput::stat_entries::FREQ, double);
				accessorRO(aeff, ddOutput::stat_entries::AEFF, double);
				accessorRO(dipoleSpacing, ddOutput::stat_entries::D, double);
				//accessorRO(numDipoles, ddOutput::stat_entries::NUM_DIPOLES, size_t);
				size_t numDipoles() const;


				std::complex<double> M(size_t dielIndex = 0) const;
				void M(const std::complex<double>&, size_t dielIndex = 0);
				size_t numM() const;

				/// Convenience function to extract the rotation information
				/// \todo Add rotation information reader and writer, and extend storage table
				//void getRots(rotations &rots) const;

				/// Get a connector representing the polarization state
				std::shared_ptr<const ddScattMatrixConnector> getConnector() const;
				void setConnector(std::shared_ptr<const ddScattMatrixConnector> cn);


				/// Provides ordering in sets, based on wavelength, aeff, and rotations.
				bool operator<(const ddOriData &rhs) const;
				//typedef std::map<size_t, std::pair<size_t, size_t> > mMuellerIndices;

				/** Need sorting only on load. **/
				/// \todo Need special handling for SCA matrix! Needed for cases where only an avg file gets loaded!!!!!
				typedef std::vector < ddscat::ddScattMatrixF >
					scattMatricesContainer;
				typedef std::vector < ddscat::ddScattMatrixP >
					avgMatricesContainer;
				//typedef std::vector < std::shared_ptr<const ddscat::ddScattMatrix> >
				//	scattMatricesContainer;
				//typedef std::set<std::shared_ptr<const ddscat::ddScattMatrix>,
				//	sharedComparator<std::shared_ptr<const ddscat::ddScattMatrix> > >
				//	scattMatricesContainer;

				/// Extract all scattering matrices. Used in ddscat-test.
				//void getScattMatrices(scattMatricesContainer&) const;
				/// Get internal scattering matrix object. Allows direct manipulation of stored data.
				/// \note This can be dangerous, as the returned object will be 
				/// invalidated when the ddOriData object is destroyed.
				//scattMatricesContainer& getScattMatrices();

				/// Count the scattering P matrices
				//size_t numP() const;
				/// Count the scattering F matrices
				//size_t numF() const;
				/// Count the scattering matrices
				size_t numMat() const;

				// All of the stat entries are part of a ddOutput object! This increases performance 
				// by reducing cache misses. As such, this class is an overlay.
				//typedef std::array<std::string, stat_entries::NUM_STAT_ENTRIES_STRINGS> statTableStringType;
				//typedef std::array<double, stat_entries::NUM_STAT_ENTRIES_DOUBLES> statTableDoubleType;
				//typedef std::array<size_t, stat_entries::NUM_STAT_ENTRIES_INTS> statTableSizetType;
				//typedef std::vector<std::complex<double> > refrTableType;
				/// Extract the entire stat table. Used in ddscat-test, io and fast weighting.
				//void getStatTable(statTableDoubleType&) const;
				//void getStatTable(statTableSizetType&) const;
				//void getStatTable(statTableStringType&) const;
				/// Get an individual stat entry.
				/// \see stat_entries
				//double getStatEntry(stat_entries::stat_entries_doubles e) const;
				//size_t getStatEntry(stat_entries::stat_entries_size_ts e) const;
				//std::string getStatEntry(stat_entries::stat_entries_strings e) const;

				/// Retrieves the block in which the relevant data is stored
				Eigen::Block<ddOutput::doubleType,
					1, ddOutput::stat_entries::NUM_STAT_ENTRIES_DOUBLES,
					false> selectData() const;
				//	false, true> selectData() const;
			protected:
				double __getSimpleDouble(int id) const { return selectData()(id); }
				template<class valtype>
				void __setSimpleDouble(int id, const valtype val) { __setSimpleRefDouble(id, val); }
				void __setSimpleRefDouble(int id, const double &val) { selectData()(id) = val; }


				/// Handles role of delegated constructor
				void _init();
				//void _populateDefaults();
				/// Container for the file header

				/// Containers for the stat table
				/// \see stat_entries
				//statTableType _statTable;
				//statTableTypeSize_ts _statTable_Size_ts;
				//statTableStringType _statTable_Strings;
				//refrTableType _refrs;


				mutable std::shared_ptr<const ddScattMatrixConnector> _connector;

				/// Container for sca and fml scattering matrices
				/// \see ddScattMatrix
				scattMatricesContainer _scattMatricesRaw;
				/// Only used for intermediate i/o.
				avgMatricesContainer _avgMatricesRaw;
				/// Container for refractive indices
				//std::vector<std::complex<double> > ms;
				/// Binding to the relevant ddOutput object
				ddOutput &_parent;
				/// Row in the ddOutput tables
				size_t _row;
				bool isAvg;


			};


		}
	}
}
