#pragma once
#include <array>
#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <set>
#include <Eigen/Core>
#include <Eigen/Dense>
#include <memory>

#include <icedb/splitSet.hpp>
#include <icedb/registry.hpp>
#include <icedb/io.hpp>

namespace icedb {
	namespace io {
		namespace ddscat {
			namespace shapefile {
				class shapefile;
				class shapefile_IO_input_registry {};
				class shapefile_IO_output_registry {};
				//class shapefile_serialization {};
				class shapefile_Standard {};
				class shapefile_query_registry {};
			}
		}
	}
}
namespace icedb {
	namespace registry {

		extern template struct IO_class_registry_writer <
			::icedb::io::ddscat::shapefile::shapefile > ;

		extern template struct IO_class_registry_reader <
			::icedb::io::ddscat::shapefile::shapefile > ;

		extern template class usesDLLregistry <
			::icedb::io::ddscat::shapefile::shapefile_IO_input_registry,
			IO_class_registry_reader<::icedb::io::ddscat::shapefile::shapefile> > ;
		//::icedb::io::ddscat::shapefile::shapefile_IO_class_registry>;

		extern template class usesDLLregistry <
			::icedb::io::ddscat::shapefile::shapefile_IO_output_registry,
			IO_class_registry_writer<::icedb::io::ddscat::shapefile::shapefile> > ;

	}
	namespace io {
		template <>
		DL_ICEDB_IO_DDSCAT std::shared_ptr
			<::icedb::io::ddscat::shapefile::shapefile> customGenerator();
	}
}
namespace icedb {
	namespace io {
		namespace ddscat {
			/// Contains everything to do with low-level manipulation of shape files.
			namespace shapefile {
				class shapefile;

				/// Provides local readers and writers for ddscat ddpar data (it's a binder)
				class DL_ICEDB_IO_DDSCAT implementsDDSHP :
					private icedb::io::implementsIObasic<shapefile, shapefile_IO_output_registry,
					shapefile_IO_input_registry, shapefile_Standard>
				{
				public:
					virtual ~implementsDDSHP() {}
				protected:
					implementsDDSHP();
				private:
					static const std::set<std::string>& known_formats();
				};

				/// Class for reading / writing shapefiles. May be used in statistical calculations.
				class DL_ICEDB_IO_DDSCAT shapefile :
					virtual public std::enable_shared_from_this<shapefile>,
					virtual public ::icedb::registry::usesDLLregistry<
						::icedb::io::ddscat::shapefile::shapefile_IO_input_registry,
						::icedb::registry::IO_class_registry_reader<::icedb::io::ddscat::shapefile::shapefile> >,
					virtual public ::icedb::registry::usesDLLregistry<
						::icedb::io::ddscat::shapefile::shapefile_IO_output_registry,
						::icedb::registry::IO_class_registry_writer<::icedb::io::ddscat::shapefile::shapefile> >,
					virtual public ::icedb::io::implementsStandardWriter<shapefile, shapefile_IO_output_registry>,
					virtual public ::icedb::io::implementsStandardReader<shapefile, shapefile_IO_input_registry>,
					virtual public implementsDDSHP
					//virtual public ::rtmath::io::Serialization::implementsSerialization<
					//	shapefile, shapefile_IO_output_registry, shapefile_IO_input_registry, shapefile_serialization>,
				{
					// Need readVector as a friend class
					friend std::shared_ptr<shapefile> icedb::io::customGenerator<shapefile>();
					//shapefile(const std::string &filename); // Handled by the generator
					shapefile(std::istream &in);
					shapefile();
				public:
					virtual ~shapefile();
					static std::shared_ptr<shapefile> generate(const std::string &filename);
					static std::shared_ptr<shapefile> generate(std::istream &in);
					static std::shared_ptr<shapefile> generate();
					/// Copy from an existing object
					static std::shared_ptr<shapefile> generate(std::shared_ptr<const shapefile>);

					/// \todo Fix the shapefile inheritance copy constructor.
					/// Problem is caused by io.h implementsIObasic inF being a reference.
	//#if _MSC_FULL_VER
					shapefile& operator=(const shapefile&);
					//#endif

					//bool operator<(const shapefile &) const;
					//bool operator==(const shapefile &) const;
					//bool operator!=(const shapefile &) const;

					/// Function to output shapefile information
					void info(std::ostream &out = std::cout) const;

					/// Function to fix the shape center of mass to match calculated stats
					void fixStats();
					/// Write ddscat-formatted shapefile to the given output stream.
					void print(std::ostream &out = std::cout) const;
					/// Resizes arrays to hold the desired number of points
					void resize(size_t num);
					/// Read shape dipoles from a memory buffer
					void readContents(const char *in, size_t headerEnd);
					/// Read in ONLY a shape header (for speed with dipole matching) - string, NOT a filename
					void readHeaderOnly(const std::string &str);
					static void readDDSCAT(std::shared_ptr<shapefile>, std::istream&, std::shared_ptr<icedb::registry::IO_options>);
					/// Write a standard DDSCAT shapefile to a stream (no compression)
					static void writeDDSCAT(const shapefile*, std::ostream &, std::shared_ptr<icedb::registry::IO_options>);

					/// Recalculate stats after a manipulation operation
					void recalcStats();
				private:
					/// Read a shapefile from an uncompressed string
					void readString(const std::string &in, bool headerOnly = false);
					void _init();
					void readHeader(const char *in, size_t &headerEnd);
				public:
					EIGEN_MAKE_ALIGNED_OPERATOR_NEW

					/// Original filename
					std::string filename;
					/// When timported
					std::string ingest_timestamp;
					/// The system that was imported on
					std::string ingest_hostname;
					/// The user account that imported the shape
					//std::string ingest_username;
					/// Revision of the rtmath code for ingest
					//int ingest_rtmath_version;
					/// Standard dipole spacing for this shape (the value usually used)
					float standardD;
					typedef Eigen::Matrix<float,
						Eigen::Dynamic, 3>
						points_type;
					typedef Eigen::Matrix<size_t, Eigen::Dynamic, 1> index_type;
					typedef Eigen::Matrix<float, Eigen::Dynamic, 1> scalar_type;
					/// \todo Move latticePts and the rest into const shared_ptr containers
					points_type
						latticePts, // Untransformed points
						latticePtsStd, // Points with coord translation based on file properties
						latticePtsNorm, // Points with coord transform to mean center of shape
						latticePtsRi; // Dielectric information
					/// The first field in the point listings. Provides a nonunique point id.
					Eigen::Matrix<int, Eigen::Dynamic, 1> latticeIndex;
					/// Tags used to describe the shape (decimation, perturbations, ...). Not saved in .shp format.
					std::map<std::string, std::string> tags;
					/**
					 * Container for other, temporary tables, which reflect different information
					 * about the shapefile, such as number of dipoles from the surface, the point's
					 * contribution to the moment of inertia, or other composition classifiers.
					 *
					 * These tables are not saved in the standard shapefile format, though they may
					 * be serialized. The bov format should write them out.
					 **/
					mutable std::map < std::string, std::shared_ptr<const Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> > > latticeExtras;

					/// Container for temporary Voronoi diagrams (disabled due to high memory usage)
					//mutable std::map < std::string, std::shared_ptr<Voronoi::VoronoiDiagram> > voronoi_diagrams;

					size_t numPoints;
					std::set<size_t> Dielectrics;
					std::string desc;
					// Specified in shape.dat
					// a1 and a2 are the INITIAL vectors (before rotation!)
					// usually a1 = x_lf, a2 = y_lf
					// choice of a1 and a2 can reorient the shape (useful for KE, PE constraints)
					Eigen::Array3f a1, a2, a3, d, x0, xd;
					unsigned int ddscatShapeVersion;

					// These are RAW values (no mean or d scaling)
					Eigen::Array3f mins, maxs, means;
				};
			}
		}
	}
}

DL_ICEDB_IO_DDSCAT std::ostream & operator<<(std::ostream &stream, const icedb::io::ddscat::shapefile::shapefile &ob);
//std::istream & operator>>(std::istream &stream, icedb::io::ddscat::shapefile &ob);

