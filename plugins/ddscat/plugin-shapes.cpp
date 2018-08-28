#include "defs.hpp"
#include "plugin-io-ddscat.hpp"
#include <icedb/error.hpp>
#include <icedb/shape.hpp>
#include <icedb/registry.hpp>
#include <icedb/io.hpp>
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

				icedb::Shapes::NewShapeProperties readDDSCATfile(std::shared_ptr<icedb::plugins::ddscat::ddscat_handle> h,
					std::shared_ptr<icedb::registry::IO_options> opts)
				{
					// error_info holds a stack of diagnostic error messages.
					::icedb::registry::options_ptr error_info = ::icedb::registry::options::generate();
					error_info->add("Filename", h->filename);
					try {
						using namespace std;
						icedb::Shapes::NewShapeProperties shpdata; // We are reading the shape into this object.

						//--------------------------------------------------------------------------------------//
						// Gain access to the data file.
						//--------------------------------------------------------------------------------------//

						// Read the full contents of the file from the ifstream into a buffer.
						std::vector<char> buffer;
						if (bIO::readFileToBuffer<char>(h->filename, buffer))
							ICEDB_throw(icedb::error::error_types::xBadInput)
							.add<std::string>("Reason", "Cannot read file to buffer.");



						const char* pa = buffer.data(); // Start of the file
						const char* pb = strchr(pa + 1, '\0'); // End of the file
						const char* pNumStart = pa;


						//--------------------------------------------------------------------------------------//
						// Read the file / guess the number of rows and columns in the file.
						//--------------------------------------------------------------------------------------//

						// Search for the first line that is not a comment.
						// Nmat lines for ADDA are also ignored.
						while ((pNumStart[0] == '#' || pNumStart[0] == 'N' || pNumStart[0] == 'n') && pNumStart < pb) {
							const char* lineEnd = strchr(pNumStart + 1, '\n');
							pNumStart = lineEnd + 1;
						}
						if (pNumStart >= pb) throw(std::invalid_argument("Cannot find any points in a shapefile."));

						const char* firstLineEnd = strchr(pNumStart + 1, '\n'); // End of the first line containing numeric data.
																				// Attempt to guess the number of points based on the number of lines in the file.
						// The implementation using std::count is unfortunately slow
						//int guessNumPoints = (int) std::count(pNumStart, pb, '\n');
						// This is much faster, and allows for auto-vectorization
						int guessNumPoints = 2; // Just in case there is a missing newline at the end
						// This format does not pre-specify the number of points.
						for (const char* c = pNumStart; c != pb; ++c)
							if (c[0] == '\n') guessNumPoints++;

						float max_element = -1, junk_f = -1;
						std::array<float, 4> firstLineVals; //(numPoints*8);
														  //std::vector<float> &parser_vals = res.required.particle_scattering_element_coordinates;
						std::vector<float> parser_vals((guessNumPoints * 4), 0);

						size_t actualNumReads = strints_array_to_floats(pNumStart, pb - pNumStart, parser_vals.data(), parser_vals.size(), max_element);
						if (actualNumReads == 0) throw (std::invalid_argument("Bad read"));
						parser_vals.resize(actualNumReads);

						// Also parse just the first line to get the number of columns
						size_t numCols = strints_array_to_floats(pNumStart, firstLineEnd - pNumStart, firstLineVals.data(), firstLineVals.size(), junk_f);

						bool good = false;
						if (numCols == 3) good = true; // Three columns, x, y and z
						if (numCols == 4) good = true; // Four columns, x, y, z and material
						if (!good) throw (std::invalid_argument("Bad read"));

						size_t actualNumPoints = actualNumReads / numCols;

						std::vector<int32_t> parsed_as_ints(parser_vals.size());
						for (size_t i = 0; i < parser_vals.size(); ++i)
							parsed_as_ints[i] = (int32_t)parser_vals[i];

						//--------------------------------------------------------------------------------------//
						// Pack the data in the shpdata structure.
						//--------------------------------------------------------------------------------------//

						//shpdata.particle_id = opts->filename(); // Placeholder particle id. Should be set by importer program.
						shpdata.author = "TESTING---MUST-SET-IN-PRODUCTION"; // Should be set by the importer program.
						shpdata.contact = "TESTING---MUST-SET-IN-PRODUCTION"; // Should be set by the importer program.
						shpdata.scattering_method = "ADDA";
						//shpdata.version = { 0,0,0 }; // Should be set by the importer program.
						//shpdata.scattering_element_coordinates_scaling_factor; // Should be set by the importer program.
						//shpdata.scattering_element_coordinates_units; // Should be set by the importer program.
						shpdata.dataset_id = "TESTING---MUST-SET-IN-PRODUCTION"; // Should be set by the importer program.

						shpdata.particle_scattering_element_number.resize(actualNumPoints);
						for (size_t i = 0; i < actualNumPoints; ++i)
							shpdata.particle_scattering_element_number[i] = (int32_t)(i + 1);

						if (numCols == 3) {
							shpdata.particle_constituents = { std::pair<uint16_t, std::string>(1, "ice") };
							shpdata.particle_scattering_element_composition_whole = std::vector<uint16_t>(actualNumPoints, 1);

							shpdata.particle_scattering_element_coordinates_as_ints = parsed_as_ints;
						}
						else if (numCols == 4) {
							// Count the number of distinct materials
							uint16_t max_constituent = 1;
							for (size_t i = 0; i < actualNumPoints; ++i) {
								auto constit = parser_vals[(4 * i) + 3];
								Expects(static_cast<uint16_t>(constit) < UINT16_MAX);
								if (static_cast<uint16_t>(constit) > max_constituent)
								{
									max_constituent = static_cast<uint16_t>(constit);
									shpdata.particle_constituents.push_back(std::pair<uint16_t, std::string>((uint16_t)constit, "unknown"));
								}
							}
							if (max_constituent == 1) // If only one constituent, then this is ice.
								shpdata.particle_constituents = { std::pair<uint16_t, std::string>(1, "ice") };

							shpdata.particle_scattering_element_coordinates_as_ints.resize(3 * actualNumPoints);
							shpdata.particle_scattering_element_composition_whole.resize(actualNumPoints);
							for (size_t i = 0; i < actualNumPoints; ++i) {
								size_t crdindex = (3 * i);
								size_t parserindex = (4 * i);
								shpdata.particle_scattering_element_coordinates_as_ints[crdindex + 0] = (int32_t)parser_vals[parserindex + 0];
								shpdata.particle_scattering_element_coordinates_as_ints[crdindex + 1] = (int32_t)parser_vals[parserindex + 1];
								shpdata.particle_scattering_element_coordinates_as_ints[crdindex + 2] = (int32_t)parser_vals[parserindex + 2];
								shpdata.particle_scattering_element_composition_whole[i] = static_cast<uint16_t>(parser_vals[parserindex + 3]);
							}
						}
						else ICEDB_throw(icedb::error::error_types::xBadInput)
							.add<string>("Reason", "Bad number of columns in this text file.");



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
				std::shared_ptr<icedb::plugins::ddscat::ddscat_handle> h = registry::construct_handle
					<registry::IOhandler, icedb::plugins::ddscat::ddscat_handle>(
						sh, PLUGINID, [&]() {return std::shared_ptr<icedb::plugins::ddscat::ddscat_handle>(
							new icedb::plugins::ddscat::ddscat_handle(filename.c_str(), iotype)); });

				// Actually read the shape
				*s = readDDSCATfile(h, opts);

				// Return the opened "handle".
				return h;
			}
	}
}
