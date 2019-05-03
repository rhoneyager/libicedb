#include "defs.hpp"
#include "plugin-io-ddscat.hpp"
#include "icedb/Errors/error.hpp"
#include "icedb/IO/Shapes.hpp"
#include "icedb/Plugins/registry.hpp"
#include "icedb/IO/io.hpp"
#include <iostream>
#include <boost/filesystem.hpp> // Should switch to the boost-independent version
#include <memory>
#include <map>
#include <set>
#include <string>
#include <vector>
#include "shapefile.h"

namespace icedb {
	namespace plugins {
		namespace ddscat {
			namespace Shapes {

				icedb::Shapes::NewShapeProperties readDDSCATfile(std::shared_ptr<icedb::plugins::ddscat::ddscat_text_handle> h,
					std::shared_ptr<icedb::registry::IO_options> opts)
				{
					// error_info holds a stack of diagnostic error messages.
					::icedb::registry::options_ptr error_info = ::icedb::registry::options::generate();
					error_info->add("Filename", h->filename);
					try {
						using namespace std;
						icedb::Shapes::NewShapeProperties shpdata; // We are reading the shape into this object.

						const auto ddshp = icedb::io::ddscat::shapefile::shapefile::generate(h->filename);

						//--------------------------------------------------------------------------------------//
						// Pack the data in the shpdata structure.
						//--------------------------------------------------------------------------------------//

						//shpdata.particle_id = opts->filename(); // Placeholder particle id. Should be set by importer program.
						shpdata.author = "TESTING---MUST-SET-IN-PRODUCTION"; // Should be set by the importer program.
						shpdata.contact = "TESTING---MUST-SET-IN-PRODUCTION"; // Should be set by the importer program.
						shpdata.scattering_method = "DDSCAT";
						//shpdata.version = { 0,0,0 }; // Should be set by the importer program.
						//shpdata.scattering_element_coordinates_scaling_factor; // Should be set by the importer program.
						//shpdata.scattering_element_coordinates_units; // Should be set by the importer program.
						shpdata.dataset_id = "TESTING---MUST-SET-IN-PRODUCTION"; // Should be set by the importer program.

						shpdata.particle_scattering_element_number.resize(ddshp->numPoints);
						shpdata.particle_scattering_element_composition_whole.resize(ddshp->numPoints);
						shpdata.particle_scattering_element_coordinates_as_ints.resize(ddshp->numPoints * 3);
						for (size_t i = 0; i < ddshp->numPoints; ++i) {
							shpdata.particle_scattering_element_number[i] = (int32_t)(ddshp->latticeIndex(i));
							shpdata.particle_scattering_element_coordinates_as_ints[(3 * i) + 0] = (int32_t) ddshp->latticePts(i, 0);
							shpdata.particle_scattering_element_coordinates_as_ints[(3 * i) + 1] = (int32_t)ddshp->latticePts(i, 1);
							shpdata.particle_scattering_element_coordinates_as_ints[(3 * i) + 2] = (int32_t)ddshp->latticePts(i, 2);
							shpdata.particle_scattering_element_composition_whole[i] = (int32_t)ddshp->latticePtsRi(i, 0);
						}

						for (const auto &d : ddshp->Dielectrics) {
							shpdata.particle_constituents.push_back(std::pair<uint16_t, std::string>((uint16_t)d, "unknown"));
						}
						// If only one constituent, and if its id is 1, then this is ice.
						if (ddshp->Dielectrics.size() == 1) 
							if (*ddshp->Dielectrics.begin() == 1)
								shpdata.particle_constituents = { std::pair<uint16_t, std::string>(1, "ice") };

						//--------------------------------------------------------------------------------------//
						// Return the shpdata structure for writing to the new HDF5 output file.
						//--------------------------------------------------------------------------------------//

						return shpdata;

						//--------------------------------------------------------------------------------------//
						// Done.
						//--------------------------------------------------------------------------------------//
				}
				// Error tagging
				catch (icedb::error::xError &err) {
					error_info->add<std::string>("Reason", "This file does not have the proper structure for a Penn State geometry file.");
					err.push(error_info);
					throw err;
				}
			}
		}
	}
}

using std::shared_ptr;
using namespace icedb::plugins::ddscat::Shapes;

namespace registry {
	template<> shared_ptr<IOhandler>
		read_file_type_multi<icedb::Shapes::NewShapeProperties>
			(shared_ptr<IOhandler> sh, shared_ptr<IO_options> opts,
				shared_ptr<icedb::Shapes::NewShapeProperties > s,
				shared_ptr<const icedb::registry::collectionTyped<icedb::Shapes::NewShapeProperties> > filter)
		{
			// Prepare to read the shape - open a "handle" to the file if it is not already open.
			std::string filename = opts->filename();
			IOhandler::IOtype iotype = opts->getVal<IOhandler::IOtype>("iotype", IOhandler::IOtype::READONLY);
			using std::shared_ptr;
			std::shared_ptr<icedb::plugins::ddscat::ddscat_text_handle> h = registry::construct_handle
				<registry::IOhandler, icedb::plugins::ddscat::ddscat_text_handle>(
					sh, PLUGINID, [&]() {return std::shared_ptr<icedb::plugins::ddscat::ddscat_text_handle>(
						new icedb::plugins::ddscat::ddscat_text_handle(filename.c_str(), iotype)); });

			// Actually read the shape
			*s = readDDSCATfile(h, opts);

			// Return the opened "handle".
			return h;
		}
}
}
