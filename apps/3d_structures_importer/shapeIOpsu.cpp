#include <icedb/defs.h>
#include <icedb/error.hpp>
#include <iostream>
#include <HH/Files.hpp>
#include <HH/Datasets.hpp>
#include <boost/filesystem.hpp> // Should switch to the boost-independent version
#include <memory>
#include <map>
#include <set>
#include <string>
#include <vector>
#include "shapeIOtext.hpp"

namespace icedb {
	namespace Examples {
		namespace Shapes {
			template <typename T>
			void readDataset(HH::Dataset dset, std::vector<T> &outdata)
			{
				auto dims = dset.getDimensions();
				//Expects(dims.dimensionality == 2);
				outdata.resize(dims.numElements);
				if (dset.read<T>(outdata) < 0)
					ICEDB_throw(icedb::error::error_types::xBadInput)
					.add("Reason", "Cannot read a dataset.");
			}

			/** \brief Reads a Penn State-style geometry file
			*
			* \todo Convert to using my "HDF for Humans" interface once it is ready.
			**/
			ShapeDataBasic readPSUfile(const std::string &filename)
			{
				using namespace std;
				ShapeDataBasic shpdata;
				// The particle index is not specific enough. Let's use the filename for an id.
				boost::filesystem::path p(filename);
				auto pfile = p.filename();
#if BOOST_VERSION < 104600
				string id = pfile.c_str(); // Needed for older RHEL machines
#else
				string id = pfile.string().c_str(); // Totally assuming a lack of non-Latin characters in the path.
#endif
				std::cout << "Reading id " << id << std::endl;

				auto hFile = HH::File::openFile(filename.c_str(), H5F_ACC_RDONLY);
				// A valid PSU file has these tables: particle_index, sphere_index, r, x, y, z.
				// particle_index has one row, one column.
				// The rest have one column, and a number of rows that correspond to the number of spheres used to represent the particle.
				
				if (!hFile.dsets.exists("/particle_index")
					|| !hFile.dsets.exists("/sphere_index")
					|| !hFile.dsets.exists("/r")
					|| !hFile.dsets.exists("/x")
					|| !hFile.dsets.exists("/y")
					|| !hFile.dsets.exists("/z"))
					ICEDB_throw(icedb::error::error_types::xBadInput)
					.add("Reason", "This file does not have the proper structure for a Penn State geometry file.")
					.add("Filename", filename);

				// Open all of the datasets. Make sure that they have the correct dimensionality.
				// Read the data into vectors. Verify that the data have the appropriate sizes.
				vector<float> xs, ys, zs, rs;
				vector<int32_t> sphere_indices;

				// No need to read particle_index. Not being used.
				//hFile.dsets["/sphere_index"].read()
				readDataset<int32_t>(hFile.dsets["/sphere_index"], sphere_indices);
				readDataset<float>(hFile.dsets["/r"], rs);
				readDataset(hFile.dsets["/x"], xs);
				readDataset(hFile.dsets["/y"], ys);
				readDataset(hFile.dsets["/z"], zs);

				// Check that the read arrays have matching sizes.
				const size_t numPoints = rs.size();
				if (numPoints != xs.size()) ICEDB_throw(icedb::error::error_types::xAssert);
				if (numPoints != ys.size()) ICEDB_throw(icedb::error::error_types::xAssert);
				if (numPoints != zs.size()) ICEDB_throw(icedb::error::error_types::xAssert);
				if (numPoints != sphere_indices.size()) ICEDB_throw(icedb::error::error_types::xAssert);

				// Finally, pack the data in the shpdata structure.
				shpdata.required.number_of_particle_scattering_elements = static_cast<uint64_t>(numPoints);
				shpdata.required.number_of_particle_constituents = 1;
				shpdata.required.particle_id = id;
				shpdata.required.particle_scattering_element_coordinates_are_integral = false;

				/// VARIABLE: Cartesian coordinates of the center of each scattering element
				/// Written in form of x_1, y_1, z_1, x_2, y_2, z_2, ...
				/// Dimensions of [number_of_particle_scattering_elements][axis]
				shpdata.required.particle_scattering_element_coordinates.resize(3 * numPoints);
				for (size_t i = 0; i < numPoints; ++i) {
					shpdata.required.particle_scattering_element_coordinates[(3 * i) + 0] = xs[i];
					shpdata.required.particle_scattering_element_coordinates[(3 * i) + 1] = ys[i];
					shpdata.required.particle_scattering_element_coordinates[(3 * i) + 2] = zs[i];
				}

				shpdata.optional.particle_constituent_single_name = "ice";
				shpdata.optional.particle_scattering_element_spacing = 0.001f; // 1 mm
				shpdata.optional.particle_scattering_element_radius = rs;


				return shpdata;
			}
		}
	}
}
