// This is REALLY rough. Mostly to serve as an example / get the test shape ingest program running.
#pragma warning( disable : 4996 ) // -D_SCL_SECURE_NO_WARNINGS
#pragma warning( disable : 4244 ) // 'argument': conversion from 'std::streamsize' to 'int', possible loss of data - boost::copy
#define _HAS_AUTO_PTR_ETC 1
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/spirit/include/karma.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <fstream>
#include "shape.hpp"
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
		using qi::double_;
		using qi::long_;
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

	/// Used in quickly printing shapefile
	template <typename OutputIterator, typename Container>
	bool print_shapefile_entries(OutputIterator& sink, Container const& v)
	{
		using boost::spirit::karma::long_;
		using boost::spirit::karma::float_;
		using boost::spirit::karma::repeat;
		using boost::spirit::karma::generate;
		//using boost::spirit::karma::generate_delimited;
		using boost::spirit::ascii::space;

		bool r = generate(
			sink,                           // destination: output iterator
			*(
				//repeat(7)()
				'\t' << long_ << '\t' << // point id
				float_ << '\t' << float_ << '\t' << float_ << '\t' << // point coordinates
				long_ << '\t' << long_ << '\t' << long_ << '\n' // dielectric
				),
			//space,                          // the delimiter-generator
			v                               // the data to output 
		);
		return r;
	}

	template <typename OutputIterator, typename Container>
	bool print_shapefile_pts(OutputIterator& sink, Container const& v)
	{
		using boost::spirit::karma::long_;
		using boost::spirit::karma::float_;
		using boost::spirit::karma::repeat;
		using boost::spirit::karma::generate;
		//using boost::spirit::karma::generate_delimited;
		using boost::spirit::ascii::space;

		bool r = generate(
			sink,                           // destination: output iterator
			*(
				//repeat(7)()
				float_ << '\t' << float_ << '\t' << float_ << '\n'
				),
			//space,                          // the delimiter-generator
			v                               // the data to output 
		);
		return r;
	}
}

namespace icedb {
	namespace Examples {
		namespace Shapes {
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
			/// Read ddscat text contents
			void readTextContents(const char *iin, size_t headerEnd, ShapeDataBasic& p)
			{
				using namespace std;

				//Eigen::Vector3f crdsm, crdsi; // point location and diel entries
				const char* pa = &iin[headerEnd];
				const char* pb = strchr(pa + 1, '\0');

				std::vector<float> parser_vals; //(numPoints*8);
				parse_shapefile_entries(pa, pb, parser_vals);
				assert(parser_vals.size() % 7 == 0);
				size_t numPoints = parser_vals.size() / 7;
				p.required.particle_scattering_element_number.resize(numPoints);
				p.required.particle_scattering_element_coordinates.resize(numPoints * 3);
				std::set<uint64_t> constituents;

				for (size_t i = 0; i < numPoints; ++i)
				{
					size_t pIndex = 7 * i;
					p.required.particle_scattering_element_number[i] = static_cast<uint64_t>(parser_vals[pIndex]);
					p.required.particle_scattering_element_coordinates[3*i] = parser_vals[pIndex+1];
					p.required.particle_scattering_element_coordinates[(3 * i)+1] = parser_vals[pIndex + 2];
					p.required.particle_scattering_element_coordinates[(3 * i)+2] = parser_vals[pIndex + 3];
					
					constituents.emplace(static_cast<uint64_t>(parser_vals[pIndex + 4]));
					//p.required.particle_scattering_element_composition[i] = parser_vals[pIndex + 4]; //! Redo pass later and map
					
				}
				p.required.particle_constituent_number = IntData_t(constituents.begin(), constituents.end());
				p.required.particle_scattering_element_composition.resize(constituents.size()*numPoints);
				
				for (size_t i = 0; i < numPoints; ++i)
				{
					size_t pIndex = 7 * i;
					uint64_t substance_id = static_cast<uint64_t>(parser_vals[pIndex + 4]);
					size_t offset = 0;
					for (auto it = constituents.cbegin(); it != constituents.cend(); ++it, ++offset) {
						if ((*it) == substance_id) break;
					}
					p.required.particle_scattering_element_composition[(numPoints*constituents.size()) + offset] = 1;
				}
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
				readTextContents(str.c_str(), headerEnd, res);
				return res;
			}
			


			void writeDDSCAT(const std::string &filename, const ShapeDataBasic& p)
			{
				using namespace std;
				std::ofstream out(filename.c_str());

				out << p.required.particle_id << endl;
				out << p.required.particle_scattering_element_number.size() << "\t= Number of lattice points" << endl;
				
				out << "1.0\t1.0\t1.0\t= target vector a1 (in TF)" << endl;
				out << "0.0\t1.0\t0.0\t= target vector a2 (in TF)" << endl;
				out << "1.0\t1.0\t1.0\t= d_x/d  d_y/d  d_x/d  (normally 1 1 1)" << endl;

				//out << (*hdr)(X0, 0) << "\t" << (*hdr)(X0, 1) << "\t" << (*hdr)(X0, 2);
				out << "0.0\t0.0\t0.0\t= X0(1-3) = location in lattice of target origin" << endl;
				out << "\tNo.\tix\tiy\tiz\tICOMP(x, y, z)" << endl;
				//size_t i = 1;

				const size_t numPoints = p.required.particle_scattering_element_number.size();
				std::vector<float> oi(numPoints * 7);

				for (size_t j = 0; j < numPoints; j++)
				{
					const float &x = p.required.particle_scattering_element_coordinates[j * 3+0];
					const float &y = p.required.particle_scattering_element_coordinates[j * 3+1];
					const float &z = p.required.particle_scattering_element_coordinates[j * 3+2];
					oi[j * 7 + 0] = p.required.particle_scattering_element_number[j];
					oi[j * 7 + 1] = x;
					oi[j * 7 + 2] = y;
					oi[j * 7 + 3] = z;
					oi[j * 7 + 4] = 1;
					oi[j * 7 + 5] = 1;
					oi[j * 7 + 6] = 1;
				}

				std::string generated;
				std::back_insert_iterator<std::string> sink(generated);
				if (!print_shapefile_entries(sink, oi))
				{
					throw(std::invalid_argument("Somehow unable to print the shape points properly."));
				}
				out << generated;
			}
			
			
			
			/// Simple file assuming one substance, with 3-column rows, each representing a single point.
			ShapeDataBasic readTextRaw(const char *iin)
			{
				using namespace std;
				ShapeDataBasic res;
				//Eigen::Vector3f crdsm, crdsi; // point location and diel entries
				const char* pa = iin;
				const char* pb = strchr(pa + 1, '\0');
				const char* firstLineEnd = strchr(pa + 1, '\n');
				// Attempt to guess the number of points based on the number of lines in the file.
				int guessNumPoints = std::count(pa, pb, '\n');
				std::vector<float> parser_vals, firstLineVals; //(numPoints*8);
				parser_vals.reserve(guessNumPoints * 8);
				parse_shapefile_entries(pa, pb, parser_vals);
				parse_shapefile_entries(pa, firstLineEnd, firstLineVals);

				size_t numCols = firstLineVals.size();
				bool good = false;
				if (numCols == 3) good = true;
				if (!good) throw (std::invalid_argument("Bad read"));
				if (parser_vals.size() == 0) throw (std::invalid_argument("Bad read"));

				size_t actualNumPoints = parser_vals.size() / numCols;
				assert(actualNumPoints == guessNumPoints);

				// Just copy parser_vals into the point array
				res.required.particle_scattering_element_coordinates.resize(parser_vals.size());
				for (size_t i = 0; i < parser_vals.size(); ++i)
					res.required.particle_scattering_element_coordinates[i] = static_cast<float>(parser_vals[i]);
				// Just one dielectric
				res.required.particle_scattering_element_number.resize(actualNumPoints);
				res.required.particle_scattering_element_composition.resize(actualNumPoints);
				for (size_t i = 1; i <= actualNumPoints; ++i) {
					res.required.particle_scattering_element_number[i] = static_cast<uint64_t>(i);
					res.required.particle_scattering_element_composition[i] = 1;
				}
				res.required.particle_constituent_number.resize(1);
				res.required.particle_constituent_number[0] = 1;

				res.required.particle_id = "";

				return res;
			}

			void writeTextRaw(const std::string &filename, const ShapeDataBasic& p)
			{
				using namespace std;
				std::ofstream out(filename.c_str());
				std::string generated;
				std::back_insert_iterator<std::string> sink(generated);
				if (!print_shapefile_pts(sink, p.required.particle_scattering_element_coordinates))
				{
					throw(std::invalid_argument("Somehow unable to print the shape points properly."));
				}
				out << generated;
			}

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
				if (std::string::npos == s.find_first_not_of("0123456789. \t\n", 0, end)) {
					return readDDSCAT(s.c_str());
				}
				else {
					return readTextRaw(s.c_str());
				}
			}

		}
	}
}
