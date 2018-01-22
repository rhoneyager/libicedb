#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>

#include <boost/lexical_cast.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include "shape.hpp"
namespace icedb {
	namespace Examples {
		namespace Shapes {
			ShapeDataBasic readTextFile(const std::string &filename);
			ShapeDataBasic readDDSCAT(const char* in);
			void readHeader(const char* in, std::string &desc, size_t &np, size_t &headerEnd);
			void readDDSCATtextContents(const char *iin, size_t numExpectedPoints, size_t headerEnd, ShapeDataBasic& p);
			ShapeDataBasic readRawText(const char *iin);

			ShapeDataBasic readTextFile(
				const std::string &filename) {
				// Open the file and copy to a string. Check the first few lines to see if any
				// alphanumeric characters are present. If there are, treat it as a DDSCAT file.
				// Otherwise, treat as a raw text file.
				std::ifstream in(filename.c_str());
				std::ostringstream so;
				boost::iostreams::copy(in, so);
				std::string s = so.str();

				auto end = s.find_first_of("\n\0");
				Expects(end != std::string::npos);
				std::string ssub = s.substr(0, end);
				auto spos = ssub.find_first_not_of("0123456789. \t\n");

				if (std::string::npos == spos) // This is a raw text file
					return readRawText(s.c_str());
				else return readDDSCAT(s.c_str()); // This is a DDSCAT file
												   //if ((std::string::npos != spos) && (spos < end)) {
												   //	return readDDSCAT(s.c_str());
												   //}
												   //else {
												   //	return readTextRaw(s.c_str());
												   //}
			}

			ShapeDataBasic readDDSCAT(const char* in)
			{
				using namespace std;
				ShapeDataBasic res;

				std::string str(in);
				std::string desc; // Currently unused
				size_t headerEnd = 0, numPoints = 0;
				readHeader(str.c_str(), desc, numPoints, headerEnd);
				//res.required.particle_id = desc;
				//data->resize((int)numPoints, ::scatdb::shape::backends::NUM_SHAPECOLS);
				readDDSCATtextContents(str.c_str(), numPoints, headerEnd, res);
				return res;
			}

			void readHeader(const char* in, std::string &desc, size_t &np,
				size_t &headerEnd)
			{
				using namespace std;

				// Do header processing using istreams.
				// The previous method used strings, but this didn't work with compressed reads.
				//size_t &pend = headerEnd;
				const char* pend = in;
				const char* pstart = in;

				// The header is seven lines long
				for (size_t i = 0; i < 7; i++)
				{
					pstart = pend;
					pend = strchr(pend, '\n');
					pend++; // Get rid of the newline
							//pend = in.find_first_of("\n", pend+1);
					string lin(pstart, pend - pstart - 1);
					if (*(lin.rbegin()) == '\r') lin.pop_back();
					//std::getline(in,lin);

					size_t posa = 0, posb = 0;
					//Eigen::Array3f *v = nullptr;
					switch (i)
					{
					case 0: // Title line
						desc = lin;
						break;
					case 1: // Number of dipoles
					{
						// Seek to first nonspace character
						posa = lin.find_first_not_of(" \t\n", posb);
						// Find first space after this position
						posb = lin.find_first_of(" \t\n", posa);
						size_t len = posb - posa;
						string s = lin.substr(posa, len);
						np = boost::lexical_cast<size_t>(s);
						//np = macros::m_atoi<size_t>(&(lin.data()[posa]), len);
					}
					break;
					case 6: // Junk line
					default:
						break;
					case 2: // a1
					case 3: // a2
					case 4: // d
					case 5: // x0
							// These all have the same structure. Read in three doubles, then assign.
							/*
							{
							Eigen::Array3f v;
							for (size_t j = 0; j < 3; j++)
							{
							// Seek to first nonspace character
							posa = lin.find_first_not_of(" \t\n,", posb);
							// Find first space after this position
							posb = lin.find_first_of(" \t\n,", posa);
							size_t len = posb - posa;
							string s = lin.substr(posa, len);
							v(j) = boost::lexical_cast<float>(s);
							//v(j) = macros::m_atof<float>(&(lin.data()[posa]), len);
							}
							hdr->block<3, 1>(0, i - 2) = v;

							}
							*/
						break;
					}
				}

				headerEnd = (pend - in) / sizeof(char);
			}

			/// Read a series of floating point numbers from a buffer. It is suggested to
			/// pre-size the output vector with reserve(...) and an expected size of the output.
			void readFloats(
				std::vector<float> &outNumbers,
				gsl::not_null<const char *> startPosition,
				const char* endPosition = nullptr)
			{
				if (!endPosition)
					endPosition = strchr(startPosition.get() + 1, '\0');

				const char *sB = startPosition; // Start of buffer
				const char *eB = endPosition;   // End of buffer
				const char *cN = sB; // Start of current number
				const char *eN; // End of current number

				const char* numbers = "0123456789.";
				const char* whitespace = " \t\n";
				float num = 0;
				unsigned int numExtracted = 0;
				// This loop extracts every number that it can from the input character buffer.
				while (cN < eB) {
					// Seek to the start of a number
					if (cN != sB)
						cN = strpbrk(eN+1, numbers);
					else 
						cN = strpbrk(eN, numbers); // Special case: array start

					if (!cN) break;
					// Find the end of the number (end of string, whitespace or end of line)
					eN = strpbrk(cN, whitespace);

					// Safety checks for end of range.
					if (cN >= eB) break;
					if (eN > eB) eN = eB;
					// Both statements should guarantee that you can't have a partial end read,
					// and lexical_cast should not encounter a whitespace-only string.

					// Extract the number.
					// Throws bad_lexical_cast if there is any text in-between.
#if BOOST_VERSION >= 105200
					num = boost::lexical_cast<float>(cN, eN - cN);
#else
					// This version of lexical_cast is simply too old.
					// A temporary string gets constructed.
					// The other option is to make the character array
					// mutable, and to pepper it with NULLS, but this
					// makes the code much less portable.
					std::string temp(cN,eN-cN);
					num = boost::lexical_cast<float>(temp);
#endif
					// Append the number to the output vector
					outNumbers.push_back(num);
					numExtracted++;
				}
			}

			/// Read ddscat text contents - the stuff after the header
			void readDDSCATtextContents(const char *iin, size_t numExpectedPoints, size_t headerEnd, ShapeDataBasic& p)
			{
				using namespace std;

				//Eigen::Vector3f crdsm, crdsi; // point location and diel entries
				const char* pa = &iin[headerEnd];
				const char* pb = strchr(pa + 1, '\0');
				
				std::vector<float> parser_vals; //(numPoints*8);
				parser_vals.reserve(7 * numExpectedPoints);

				readFloats(parser_vals, pa, pb);

				assert(parser_vals.size() % 7 == 0);
				size_t &numPoints = p.required.number_of_particle_scattering_elements;
				numPoints = parser_vals.size() / 7;
				assert(numPoints == numExpectedPoints);
				p.optional.particle_scattering_element_number.resize(numPoints);
				p.required.particle_scattering_element_coordinates.resize(numPoints * 3);
				std::set<uint8_t> constituents;

				for (size_t i = 0; i < numPoints; ++i)
				{
					size_t pIndex = 7 * i;
					p.optional.particle_scattering_element_number[i] = static_cast<uint64_t>(parser_vals[pIndex]);
					p.required.particle_scattering_element_coordinates[3 * i] = parser_vals[pIndex + 1];
					p.required.particle_scattering_element_coordinates[(3 * i) + 1] = parser_vals[pIndex + 2];
					p.required.particle_scattering_element_coordinates[(3 * i) + 2] = parser_vals[pIndex + 3];

					constituents.emplace(static_cast<uint8_t>(parser_vals[pIndex + 4]));
					//p.required.particle_scattering_element_composition[i] = parser_vals[pIndex + 4]; //! Redo pass later and map

				}
				p.optional.particle_constituent_number = Int8Data_t(constituents.begin(), constituents.end());
				p.optional.particle_scattering_element_composition_whole.resize(numPoints);
				if (constituents.size() >= UINT8_MAX) throw (std::invalid_argument("Shape has too many constituents."));
				p.required.number_of_particle_constituents = static_cast<uint8_t>(constituents.size());

				for (size_t i = 0; i < numPoints; ++i)
				{
					size_t pIndex = 7 * i;
					uint64_t substance_id = static_cast<uint64_t>(parser_vals[pIndex + 4]);
					size_t offset = 0;
					for (auto it = constituents.cbegin(); it != constituents.cend(); ++it, ++offset) {
						if ((*it) == substance_id) break;
					}
					size_t idx = (i*constituents.size()) + offset;
					p.optional.particle_scattering_element_composition_whole[idx] = 1;
				}
				p.required.particle_scattering_element_coordinates_are_integral = 1;
			}




			/// Simple file assuming one substance, with 3-column rows, each representing a single point.
			/// ADDA allows comment lines at the beginning of the file. Each line starts with a '#'.
			ShapeDataBasic readRawText(const char *iin)
			{
				using namespace std;
				ShapeDataBasic res;
				//Eigen::Vector3f crdsm, crdsi; // point location and diel entries
				const char* pa = iin; // Start of the file
				const char* pb = strchr(pa + 1, '\0'); // End of the file
				const char* pNumStart = pa;
				// Search for the first line that is not a comment.
				// Nmat lines for ADDA are also ignored.
				while ((pNumStart[0] == '#' || pNumStart[0] == 'N' || pNumStart[0] == 'n') && pNumStart < pb) {
					const char* lineEnd = strchr(pNumStart + 1, 'n');
					pNumStart = lineEnd + 1;
				}
				if (pNumStart >= pb) throw(std::invalid_argument("Cannot find any points in a shapefile."));

				const char* firstLineEnd = strchr(pNumStart + 1, '\n'); // End of the first line containing numeric data.
																		// Attempt to guess the number of points based on the number of lines in the file.
				int guessNumPoints = (int) std::count(pNumStart, pb, '\n');
				std::vector<float> firstLineVals; //(numPoints*8);
												  //std::vector<float> &parser_vals = res.required.particle_scattering_element_coordinates;
				std::vector<float> parser_vals;
				parser_vals.reserve(guessNumPoints * 4);
				// Read all of the numbers
				readFloats(parser_vals, pNumStart, pb);
				//parse_shapefile_entries(pNumStart, pb, parser_vals);
				const void* floatloc = memchr(pNumStart, '.', pb - pNumStart);
				res.required.particle_scattering_element_coordinates_are_integral = (floatloc) ? 0 : 1;

				// Also parse just the first line to get the number of columns
				readFloats(firstLineVals, pNumStart, firstLineEnd);
				//parse_shapefile_entries(pNumStart, firstLineEnd, firstLineVals);

				size_t numCols = firstLineVals.size();
				bool good = false;
				if (numCols == 3) good = true; // Three columns, x, y and z
				if (numCols == 4) good = true; // Four columns, x, y, z and material
				if (!good) throw (std::invalid_argument("Bad read"));
				if (parser_vals.size() == 0) throw (std::invalid_argument("Bad read"));

				size_t actualNumPoints = parser_vals.size() / numCols;
				assert(actualNumPoints == guessNumPoints);

				res.required.number_of_particle_scattering_elements = actualNumPoints;
				if (numCols == 3) {
					res.required.number_of_particle_constituents = 1;
					res.required.particle_scattering_element_coordinates = parser_vals;
				}
				else if (numCols == 4) {
					// Count the number of distinct materials
					uint8_t max_constituent = 1;
					for (size_t i = 0; i < actualNumPoints; ++i) {
						auto constit = parser_vals[(4 * i) + 3];
						assert(static_cast<uint8_t>(constit) < UINT8_MAX);
						if (static_cast<uint8_t>(constit) > max_constituent)
							max_constituent = static_cast<uint8_t>(constit);
					}
					res.required.number_of_particle_constituents = max_constituent;
					res.optional.particle_constituent_number.resize(max_constituent);
					for (size_t i = 0; i < max_constituent; ++i)
						res.optional.particle_constituent_number[i] = static_cast<uint8_t>(i + 1); // assert-checked before

					res.required.particle_scattering_element_coordinates.resize(actualNumPoints * 3);
					res.optional.particle_scattering_element_composition_whole.resize(actualNumPoints);
					for (size_t i = 0; i < actualNumPoints; ++i) {
						size_t crdindex = (3 * i);
						size_t parserindex = (4 * i);
						res.required.particle_scattering_element_coordinates[crdindex + 0] = parser_vals[parserindex + 0];
						res.required.particle_scattering_element_coordinates[crdindex + 1] = parser_vals[parserindex + 1];
						res.required.particle_scattering_element_coordinates[crdindex + 2] = parser_vals[parserindex + 2];
						res.optional.particle_scattering_element_composition_whole[i] = static_cast<uint8_t>(parser_vals[parserindex + 3]);
					}
				}

				res.required.particle_id = "";

				return res;
			}


		}
	}
}
