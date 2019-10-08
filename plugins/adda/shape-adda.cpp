#include "defs.hpp"
#include "plugin-adda.hpp"
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
#include "BinaryIO/Files.hpp"

#include <sstream>
#include <boost/spirit/include/karma.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>

// These occasionally get set by system headers in Windows, which then may get picked up by Boost.
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

/// Internal namespace for the reader parsers
namespace {
	namespace qi = boost::spirit::qi;
	namespace ascii = boost::spirit::ascii;
	namespace phoenix = boost::phoenix;

	/** \brief Parses space-separated shapefile entries.
	**/
	template <typename Iterator>
	bool parse_shapefile_entries(Iterator first, Iterator last, std::vector<float>& v)
	{
		using qi::float_;
		using qi::phrase_parse;
		using qi::_1;
		using ascii::space;
		using phoenix::push_back;

		bool r = phrase_parse(first, last,

			//  Begin grammar
			(
				// *long_[push_back(phoenix::ref(v), _1)]
				*float_
				)
			,
			//  End grammar

			space, v);

		if (first != last) // fail if we did not get a full match
			return false;
		return r;
	}
}

namespace icedb {
	namespace plugins {
		namespace adda {
			namespace Shapes {
				icedb::Shapes::NewShapeProperties readADDAfile(std::shared_ptr<icedb::plugins::adda::adda_handle> h,
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
						if (bIO::readFileToBuffer(h->filename, buffer))
							ICEDB_throw(icedb::error::error_types::xBadInput)
							.add<std::string>("Reason", "Cannot read file to buffer.");

						const char* pa = buffer.data(); // Start of the file
						const char* pb = pa + buffer.size(); // End of the file
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
						if (pNumStart >= pb)
							ICEDB_throw(icedb::error::error_types::xBadInput)
								.add("Reason", "Cannot find any points in a shapefile.")
								;

						const char* firstLineEnd = strchr(pNumStart + 1, '\n'); // End of the first line containing numeric data.
																				// Attempt to guess the number of points based on the number of lines in the file.
						// The implementation using std::count is unfortunately slow
						//int guessNumPoints = (int) std::count(pNumStart, pb, '\n');
						// This is much faster, and allows for auto-vectorization
						int guessNumPoints = 2; // Just in case there is a missing newline at the end
						// This format does not pre-specify the number of points.
						for (const char* c = pNumStart; c != pb; ++c)
							if (c[0] == '\n') guessNumPoints++;

						//float max_element = -1, junk_f = -1;
						std::vector<float> firstLineVals;
						std::vector<float> parser_vals;
						parser_vals.reserve(guessNumPoints * 4);

						parse_shapefile_entries(pNumStart, pb, parser_vals);
						size_t actualNumReads = parser_vals.size();
						//size_t actualNumReads = strints_array_to_floats(pNumStart, pb - pNumStart, parser_vals.data(), parser_vals.size(), max_element);
						if (parser_vals.size() == 0)
							ICEDB_throw(icedb::error::error_types::xBadInput)
                                                                .add("Reason", "Bad read. parser_vals is empty.");
						//parser_vals.resize(actualNumReads);

						// Also parse just the first line to get the number of columns
						parse_shapefile_entries(pNumStart, firstLineEnd, firstLineVals);
						size_t numCols = firstLineVals.size(); // strints_array_to_floats(pNumStart, firstLineEnd - pNumStart, firstLineVals.data(), firstLineVals.size(), junk_f);

						bool good = false;
						if (numCols == 3) good = true; // Three columns, x, y and z
						if (numCols == 4) good = true; // Four columns, x, y, z and material
						if (!good)
							ICEDB_throw(icedb::error::error_types::xBadInput)
                                                                .add("Reason", "Bad read. numCols is wrong. Should be 3 or 4.")
								.add("numCols", numCols);

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
							shpdata.particle_scattering_element_number[i] = (int32_t) (i + 1);

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
						error_info->add<std::string>("Reason", "This file does not have the proper structure for an ADDA shape file.");
						err.push(error_info);
                        err ICEDB_RSpushErrorvars;
						throw err;
					}
				}
			}
		}
	}

	using std::shared_ptr;
	using namespace icedb::plugins::adda::Shapes;

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
				std::shared_ptr<icedb::plugins::adda::adda_handle> h = registry::construct_handle
					<registry::IOhandler, icedb::plugins::adda::adda_handle>(
						sh, PLUGINID, [&]() {return std::shared_ptr<icedb::plugins::adda::adda_handle>(
							new icedb::plugins::adda::adda_handle(filename.c_str(), iotype)); });

				// Actually read the shape
				*s = readADDAfile(h, opts);

				// Return the opened "handle".
				return h;
			}
	}
}
