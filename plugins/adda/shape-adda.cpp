#include "defs.hpp"
#include "plugin-adda.hpp"
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
#include <BinaryIO/Files.hpp>
//#include "shapeIOtext.hpp"

namespace icedb {
	namespace plugins {
		namespace adda {
			namespace Shapes {

				size_t strints_array_to_floats(
					const char* in, const size_t inlen, float* out, const size_t outlen, float& max_element)
				{
					max_element = 0;
					size_t curout = 0;
					// Accepts numbers of the form: [0-9]*
					// Handles negatives, No exponents or decimals.

					float numerator = 0;
					assert(in);
					const char* end = in + inlen;
					bool readnums = false;
					bool negative = false;
					for (const char* cur = in; (cur <= end) && (curout < outlen); ++cur) {
						if (*cur == '-') {
							negative = true;
						}
						else if ((*cur <= '9') && (*cur >= '0')) {
							numerator *= 10;
							numerator += (*cur - '0');
							readnums = true;
						}
						else if (readnums) {
							if (negative) numerator = -numerator;
							negative = false;
							out[curout] = numerator;
							if (numerator > max_element) max_element = numerator;
							curout++;
							numerator = 0;
							readnums = false;
						}
					}
					return curout;
				}


				size_t array_to_floats(
					const char* in, const size_t inlen, float* out, const size_t outlen)
				{
					size_t curout = 0;
					// Accepts numbers of the form: (+- )[0-9]*.[0-9]*(eE)(+- )[0-9]*.[0-9]*
					bool isNegative = false;
					bool inExponent = false;
					bool expIsNeg = false;
					bool pastDecimal = false;

					uint64_t numerator, numeratorExp;
					uint64_t digits_denom, digits_denom_Exp;
					auto resetNum = [&]()
					{numerator = 0; digits_denom = 0;
					numeratorExp = 0; digits_denom_Exp = 0;
					isNegative = false; inExponent = false;
					expIsNeg = false; pastDecimal = false; };
					resetNum();

					const char* cur = in;
					const char* end = in + inlen;
					// Advance to the start of a number
					assert(cur);
					const char* numbers = "0123456789-+.eE";
					const char* whitespace = " \t\n";
					const char* numEnd = nullptr;
					auto isNumber = [](char c) -> bool {
						if (c >= '0' && c <= '9') return true;
						if (c == '-' || c == '+' || c == '.' || c == 'e' || c == 'E') return true;
						return false;
					};
					auto isControl = [](char c) -> bool {
						if (c == '-' || c == '+' || c == '.' || c == 'e' || c == 'E') return true;
						return false;
					};
					auto advanceToNumber = [](const char* in, const char* end) -> const char* {
						while (in < end) {
							if (*in >= '0' && *in <= '9') return in;
							else if (*in == '-' || *in == '+' || *in == '.' || *in == 'e' || *in == 'E') return in;
							else ++in;
						}
						return in;
					};
					while ((cur < end) && (curout < outlen)) {
						//cur = strpbrk(cur, numbers);
						cur = advanceToNumber(cur, end);
						while (isNumber(*cur)) {
							if (!isControl(*cur)) {
								if (!inExponent) {
									numerator *= 10;
									numerator += (*cur - '0');
									if (pastDecimal) digits_denom++;
								}
								else {
									numeratorExp *= 10;
									numeratorExp += (*cur - '0');
									if (pastDecimal) digits_denom_Exp++;
								}
							}
							else {
								if (*cur == '.') pastDecimal = true;
								if (*cur == '-' && !inExponent) isNegative = true;
								if (*cur == '-' && inExponent) expIsNeg = true;
								if (*cur == 'e' || *cur == 'E') {
									inExponent = true; pastDecimal = false; isNegative = false;
								}
							}
							++cur;
						}
						if (!numerator) continue;

						// Number is loaded. Assign and advance.
						float exponent = 0;
						if (numeratorExp) {
							exponent = static_cast<float>(numeratorExp);
							if (digits_denom_Exp) exponent /= powf(10.f, static_cast<float>(digits_denom_Exp));
							if (expIsNeg) exponent *= -1;
						}

						float num = 0;
						num = static_cast<float>(numerator);
						if (digits_denom) num /= powf(10.f, static_cast<float>(digits_denom));
						if (isNegative) num *= -1;

						float fnum = num;
						if (numeratorExp) fnum *= powf(10.f, exponent);

						out[curout] = fnum;
						resetNum();
						curout++;

						++cur;
					}
					return curout;
				}

				template <class T>
				T m_atof(const char* x, size_t len)
				{
					T res = 0;
					unsigned int remainder = 0;
					unsigned int rembase = 1;
					unsigned int digit = 0;
					// Sign false indicates positive. True is negative
					bool sign = false;
					bool expsign = false;
					unsigned int expi = 0;
					const char* p = x; // Set pointer to beginning of character stream
					bool exponent = false;
					bool decimal = false;
					size_t i = 0;
					while (*p != '\0' && ((len) ? i < len : true))
					{
						// Do digit checks here (no calls to isdigit)
						// Ignore whitespace
						if (*p == 'e' || *p == 'E')
						{
							exponent = true;
						}
						else if (*p == '.') {
							decimal = true;
						}
						else if (*p == '-') {
							if (!exponent)
							{
								sign = true;
							}
							else {
								expsign = true;
							}
						}
						else if (*p == '+') {
							if (!exponent)
							{
								sign = false;
							}
							else {
								expsign = false;
							}
						}
						else if (*p == ' ' || *p == '\t') {
							// Ignore whitespace (but disallow endlines)
						}
						else {
							// It's a digit!
							switch (*p)
							{
							case '0':
								digit = 0;
								break;
							case '1':
								digit = 1;
								break;
							case '2':
								digit = 2;
								break;
							case '3':
								digit = 3;
								break;
							case '4':
								digit = 4;
								break;
							case '5':
								digit = 5;
								break;
							case '6':
								digit = 6;
								break;
							case '7':
								digit = 7;
								break;
							case '8':
								digit = 8;
								break;
							case '9':
								digit = 9;
								break;
							default:
								// Invalid input
								p++;
								continue;
								break;
							}
							// Digit is set. Next, see what to do with it
							if (!decimal && !exponent)
							{
								res *= 10;
								res += digit;
							}
							else if (decimal && !exponent) {
								remainder *= 10;
								rembase *= 10;
								remainder += digit;
							}
							else if (exponent) {
								expi *= 10;
								expi += digit;
							}
						}

						p++;
						i++;
					}
					// Iterated through the string
					// Now, to combine the elements into my double
					res += (T)remainder / (T)rembase;
					if (sign) res *= -1;
					if (exponent)
					{
						if (!expsign)
						{
							res *= (T)std::pow(10, (T)expi);
						}
						else {
							res *= (T)std::pow(10, -1.0 * (T)expi);
						}
					}
					return res;
				}

				template <class T>
				T m_atoi(const char *x, size_t len)
				{
					T res = 0;
					int digit = 0;
					bool sign = false; // false is pos, true is neg
					bool done = false;
					size_t i = 0;
					const char* p = x; // Set pointer to beginning of character stream
					while (*p != '\0' && done == false && ((len) ? i < len : true))
					{
						if (*p == '-') {
							sign = true;
						}
						else if (*p == '+') {
							sign = false;
						}
						else if (*p == ' ' || *p == '\t') {
							// Ignore whitespace (but disallow endlines)
						}
						else {
							// It's a digit!
							switch (*p)
							{
							case '0':
								digit = 0;
								break;
							case '1':
								digit = 1;
								break;
							case '2':
								digit = 2;
								break;
							case '3':
								digit = 3;
								break;
							case '4':
								digit = 4;
								break;
							case '5':
								digit = 5;
								break;
							case '6':
								digit = 6;
								break;
							case '7':
								digit = 7;
								break;
							case '8':
								digit = 8;
								break;
							case '9':
								digit = 9;
								break;
							default:
								// Invalid input
								done = true;
								break;
							}
							// Digit is set. Next, see what to do with it
							if (done) break;
							res *= 10;
							res += digit;
						}
						p++;
						i++;
					}

					// Return the value
					if (sign) res *= -1;
					return res;
				}

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
						error_info->add<std::string>("Reason", "This file does not have the proper structure for a Penn State geometry file.");
						err.push(error_info);
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
