#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <cmath>

#include <icedb/fs_backend.hpp>
#include <boost/lexical_cast.hpp>
//#include <boost/iostreams/copy.hpp>
//#include <boost/iostreams/filtering_stream.hpp>
#include "shape.hpp"
namespace icedb {
	namespace Examples {
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
				    } else if ((*cur <= '9') && (*cur >= '0')) {
						numerator *= 10;
						numerator += (*cur - '0');
						readnums = true;
					} else if(readnums){
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
							} else {
								numeratorExp *= 10;
								numeratorExp += (*cur - '0');
								if (pastDecimal) digits_denom_Exp++;
							}
						} else {
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
				while (*p != '\0' && ((len) ? i<len : true))
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
				while (*p != '\0' && done == false && ((len) ? i<len : true))
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
				uintmax_t sz = sfs::file_size(sfs::path(filename));
				std::string s(sz, ' ');
				in.read(&s[0], sz);

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
					case 6: // In case of DDSCAT6 this is the first valid dipole
					        // This method is not super robust: what if line 7 contains just numbers but not a dipole coordinate
					{
					    bool isNotHeaderLine = (lin.find_first_not_of(" \t-+0123456789") == std::string::npos);
					    if (isNotHeaderLine) pend = pstart; // I assume not header line implies a dipole thus I put back the buffer to the line start
					}
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

			/// Read ddscat text contents - the stuff after the header
			void readDDSCATtextContents(const char *iin, size_t numExpectedPoints, size_t headerEnd, ShapeDataBasic& p)
			{
				using namespace std;

				//Eigen::Vector3f crdsm, crdsi; // point location and diel entries
				const char* pa = &iin[headerEnd];
				const char* pb = strchr(pa + 1, '\0');
				
				std::vector<float> parser_vals; //(numPoints*8);
				parser_vals.resize(7 * numExpectedPoints);
				float max_element = 0;
				size_t numRead = strints_array_to_floats(pa, pb - pa, parser_vals.data(), parser_vals.size(), max_element);

				assert(numRead % 7 == 0);
				auto &numPoints = p.required.number_of_particle_scattering_elements;
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
				p.optional.hint_max_scattering_element_dimension = max_element;
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
				// The implementation using std::count is unfortunately slow
				//int guessNumPoints = (int) std::count(pNumStart, pb, '\n');
				// This is much faster, and allows for auto-vectorization
				int guessNumPoints = 1; // Just in case there is a missing newline at the end
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

				//parse_shapefile_entries(pNumStart, pb, parser_vals);
				//const void* floatloc = memchr(pNumStart, '.', pb - pNumStart);
				//res.required.particle_scattering_element_coordinates_are_integral = (floatloc) ? 0 : 1;
				res.required.particle_scattering_element_coordinates_are_integral = 1;

				// Also parse just the first line to get the number of columns
				size_t numCols = strints_array_to_floats(pNumStart, firstLineEnd - pNumStart, firstLineVals.data(), firstLineVals.size(), junk_f);
				
				bool good = false;
				if (numCols == 3) good = true; // Three columns, x, y and z
				if (numCols == 4) good = true; // Four columns, x, y, z and material
				if (!good) throw (std::invalid_argument("Bad read"));

				size_t actualNumPoints = actualNumReads / numCols;
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
				res.optional.hint_max_scattering_element_dimension = max_element;

				return res;
			}


		}
	}
}
