#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/iostreams/filter/newline.hpp>
#include <boost/math/constants/constants.hpp>
#include <boost/pointer_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/qi_omit.hpp>
#include <boost/spirit/include/qi_repeat.hpp>

#include "ddOriDataParsers.h"
#include "ddOriData.h"
#include "ddVersions.h"
#include <icedb/error.hpp>
#include <icedb/units/units.hpp>

//#include "../rtmath/refract.h"

namespace icedb
{
	namespace io {
		namespace ddscat {
			namespace ddOriDataParsers {
				namespace qi = boost::spirit::qi;
				namespace ascii = boost::spirit::ascii;
				namespace phoenix = boost::phoenix;

				/// Parses a set number of doubles mixed with text in a line
				template <typename Iterator>
				bool parse_string_ddNval(Iterator first, Iterator last, size_t pos, double &val)
				{
					using qi::double_;
					using qi::ulong_;
					using qi::char_;
					using qi::phrase_parse;
					using qi::_1;
					using ascii::space;
					using phoenix::push_back;
					using qi::repeat;
					using qi::omit;

					bool r = phrase_parse(first, last,

						//  Begin grammar
						(
							omit[repeat(pos)[*char_("a-zA-Z.*=,/-()<>^_:")]] >> double_
							)
						,
						//  End grammar

						space, val);

					if (first != last) // fail if we did not get a full match
						return false;
					return r;
				}

				template <typename Iterator>
				bool parse_string_ddNvals(Iterator first, Iterator last, size_t pos, std::vector<double> &val)
				{
					using qi::double_;
					using qi::ulong_;
					using qi::char_;
					using qi::phrase_parse;
					using qi::_1;
					using ascii::space;
					using phoenix::push_back;
					using qi::repeat;
					using qi::omit;

					bool r = phrase_parse(first, last,

						//  Begin grammar
						(
							omit[repeat(pos)[*char_("a-zA-Z.*=,/-()<>^_:")]] >> double_[push_back(phoenix::ref(val), _1)]
							)
						,
						//  End grammar

						space);

					//if (first != last) // fail if we did not get a full match
					//	return false;
					return r;
				}

				/** \brief Parses space-separated numbers.
				*
				* \see rtmath::ddscat::ddOutputSingle::readF
				* \see rtmath::ddscat::ddOutputSingle::readMueller
				**/
				template <typename Iterator>
				bool parse_numbers_space(Iterator first, Iterator last, std::vector<double>& v)
				{
					using qi::double_;
					using qi::phrase_parse;
					using qi::_1;
					using ascii::space;
					using phoenix::push_back;

					bool r = phrase_parse(first, last,

						//  Begin grammar
						(
							*double_
							)
						,
						//  End grammar

						space, v);

					if (first != last) // fail if we did not get a full match
						return false;
					return r;
				}

				void version::write(std::ostream &out, size_t v) {
					out << " DDSCAT --- ";
					out << icedb::io::ddscat::ddVersions::getVerAvgHeaderString(v);
					out << std::endl;
				}

				size_t version::read(std::istream &in, size_t)
				{
					std::string lin;
					std::getline(in, lin);
					return icedb::io::ddscat::ddVersions::getVerId(lin);
				}

				void simpleString::write(std::ostream &out, size_t, const std::string &s, const std::string &p)
				{
					out << " " << p << " --- ";
					out << s << std::endl;
				}

				void simpleString::read(std::istream &in, std::string &s)
				{
					std::string lin;
					std::getline(in, lin);
					size_t p = lin.find("---");
					s = lin.substr(p + 3);
					// Remove any leading and lagging spaces
					// Not all Liu avg files are correct in this respect
					boost::algorithm::trim(s);
				}

				void simpleStringRev::write(std::ostream &out, size_t, const std::string &s, const std::string &p)
				{
					out << " " << s << " --- ";
					out << p << std::endl;
				}

				void simpleStringRev::read(std::istream &in, std::string &s)
				{
					std::string lin;
					std::getline(in, lin);
					size_t p = lin.find("---");
					s = lin.substr(0, p - 1);
					// Remove any leading and lagging spaces
					// Not all Liu avg files are correct in this respect
					boost::algorithm::trim(s);
				}

				template struct simpleNumRev < double >;
				template struct simpleNumRev < size_t >;

				template struct simpleNumCompound < double >;
				template struct simpleNumCompound < size_t >;

				void ddNumOris::read(std::istream &in, size_t &num) {
					std::string lin, s;
					std::getline(in, lin);
					size_t p = lin.find("over ");
					size_t q = lin.find(" target");
					s = lin.substr(p + 5, q - (p + 5));
					//boost::algorithm::trim(s);
					num = (size_t)boost::lexical_cast<size_t>(s);
				}

				void ddNumOris::write(std::ostream &out, size_t, size_t num) {
					out << " Results averaged over " << num << " target orientations\n";
				}

				void refractive::write(std::ostream &out, size_t, size_t inum, const std::complex<double> &m, double k, double d)
				{
					std::complex<double> eps = m * m;
					double mkd = abs(m) * k * d;
					const size_t wd = 8;
					out << "n= ( "
						<< std::setw(wd) << m.real() << std::setw(0) << " ,  "
						<< std::setw(wd) << m.imag() << std::setw(0) << "),  eps.= (  "
						<< std::setw(wd) << eps.real() << std::setw(0) << " ,  "
						<< std::setw(wd) << eps.imag() << std::setw(0) << ")  |m|kd="
						<< std::setw(wd) << mkd << std::setw(0) << " for subs. " << inum << std::endl;
				}

				void refractive::read(std::istream &in, size_t &subst, std::complex<double> &m)
				{
					std::string str;
					std::getline(in, str);
					read(str, subst, m);
				}

				void refractive::read(const std::string &str, size_t &subst, std::complex<double> &m)
				{
					// todo: fix listed ranges (stars are correct, ends are not, but w is)
					// all ranges are INCLUSIVE
					// mreal in cols 4-11
					// mimag in cols 13-20
					// eps real in 32-40
					// eps imag in 42-49
					// mkd in 59-66
					// substance number in 78+
					double mre, mim, ere, eim, mkd;
					std::complex<double> eps;

					// Different ddscat versions write their numbers differently. There are
					// no set ranges. I'll use a slow approach to sieve them out from the
					// surrounding other symbols. Valid numeric digits are numbers and '.'
					std::vector<std::string> snums;
					char lchar = 0; // last character
					char cchar = 0; // current character
					std::string csnum; // current numeric string

					auto nDone = [&]()
					{
						if (csnum.size()) snums.push_back(csnum);
						csnum.clear();
					};

					for (const auto &c : str)
					{
						cchar = c;
						if (std::isdigit(c)) csnum.push_back(c);
						else if (c == '.' && std::isdigit(lchar)) csnum.push_back(c);
						else if (csnum.size()) nDone();

						lchar = c;
					}
					nDone();

					if (snums.size() < 6) ICEDB_throw(icedb::error::error_types::xBadInput)
						.add("Reason", "Cannot parse refractive index")
						.add("Line_Text",str);

					for (size_t i = 0; i < 6; ++i)
						boost::algorithm::trim(snums[i]);
					mre = boost::lexical_cast<double>(snums[0]);
					mim = boost::lexical_cast<double>(snums[1]);
					ere = boost::lexical_cast<double>(snums[2]);
					eim = boost::lexical_cast<double>(snums[3]);
					mkd = boost::lexical_cast<float>(snums[4]);
					subst = boost::lexical_cast<size_t>(snums[5]);
					m = std::complex<double>(mre, mim);
					eps = std::complex<double>(ere, eim);
				}

				void ddRot1d::write(std::ostream &out, size_t, const std::string &fieldname,
					double min, double max, size_t n, const std::string &fieldnamecaps)
				{
					const size_t w = 7;
					// Using formatted io operations
					using std::setw;
					using std::setprecision;
					out << setprecision(5) << setw(w) << min << " "
						<< setprecision(5) << setw(w) << max << " = "
						<< fieldname << "_min, " << fieldname << "_max ; " << fieldnamecaps
						<< "=" << n << "\n";
				}

				void ddRot1d::read(std::istream &in, std::string &fieldname, double &min, double &max, size_t &n)
				{
					std::string str;
					std::getline(in, str);
					std::string fieldnamecaps;

					// Retrieving three numbers and two strings
					// Thankfully, the fields are nicely aligned
					// min is from position 0 to 8 (not inclusive)
					// max is from 9 to 16
					// fieldname is from 19 to an underscore
					// fieldnamecaps is from 41 to 47
					// n is from 48 to the end of the line
					min = boost::lexical_cast<double>(std::string(str.data(), 8));
					max = boost::lexical_cast<double>(std::string(str.data()+9, 8));
					fieldname = str.substr(19, str.find_first_of('_', 19) - 19);
					fieldnamecaps = str.substr(41, 6);
					n = (size_t)boost::lexical_cast<size_t>(std::string(str.data() + 48));
				}

				void ddPolVec::write(std::ostream &out, size_t, const std::vector<std::complex<double> > &pols,
					size_t vecnum, frameType frame)
				{
					// Using formatted io operations
					using std::setw;
					using std::setprecision;
					const size_t p = 5, w = 8;
					out << " ( " << setprecision(p) << setw(w) << pols[0].real() << ","
						<< setw(w) << pols[0].imag() << ")(" << setw(w) << pols[1].real()
						<< "," << setw(w) << pols[1].imag() << ")(" << setw(w)
						<< pols[2].real() << "," << setw(w) << pols[2].imag()
						<< ")=inc.pol.vec. " << vecnum << " in ";
					(frame == frameType::LF) ? out << "LF\n" : out << "TF\n";
				}
				void ddPolVec::read(std::istream &in, std::vector<std::complex<double> > &pols, size_t &vecnum, frameType &frame)
				{
					std::string str;
					std::getline(in, str);

					// The first six numbers are doubles. The seventh is a size_t.
					// The last two characters describe the frame.
					std::vector<std::string> snums;
					snums.reserve(30);
					char lchar = 0; // last character
					char cchar = 0; // current character
					std::string csnum; // current numeric string
					csnum.reserve(200);

					auto nDone = [&]()
					{
						if (csnum.size()) snums.push_back(csnum);
						csnum.clear();
					};

					for (const auto &c : str)
					{
						cchar = c;
						if (std::isdigit(c)) csnum.push_back(c);
						else if (c == '.' && std::isdigit(lchar)) csnum.push_back(c);
						else if (c == '-') csnum.push_back(c);
						else if (csnum.size()) nDone();

						lchar = c;
					}
					nDone();

					if (snums.size() < 7) ICEDB_throw(icedb::error::error_types::xBadInput)
						.add("Line_text",str)
						.add("Reason", "Cannot parse inc.pol.vec. numbers");

					using boost::algorithm::trim_copy;
					using std::complex;
					pols.resize(3);
					pols[0] = complex<double>(boost::lexical_cast<double>(snums[0]), boost::lexical_cast<double>(snums[1]));
					pols[1] = complex<double>(boost::lexical_cast<double>(snums[2]), boost::lexical_cast<double>(snums[3]));
					pols[2] = complex<double>(boost::lexical_cast<double>(snums[4]), boost::lexical_cast<double>(snums[5]));
					vecnum = boost::lexical_cast<size_t>(snums[6]);
					auto it = str.find_last_of('F');
					if (it == std::string::npos) ICEDB_throw(icedb::error::error_types::xBadInput)
						.add("Line_text",str)
						.add("Reason",
							"Cannot parse inc.pol.vec. numbers");
					it--;
					if (str.at(it) == 'L') frame = frameType::LF;
					else if (str.at(it) == 'T') frame = frameType::TF;
					else ICEDB_throw(icedb::error::error_types::xBadInput)
						.add("Line_text",str)
						.add("Reason",
							"Cannot parse inc.pol.vec. numbers");
				}

				void ddAxisVec::write(std::ostream &out, size_t, const std::vector<double > &v,
					size_t axisnum, frameType frame)
				{
					// Using formatted io operations
					using std::setw;
					using std::setprecision;
					const size_t p = 5, w = 8;
					out << "( " << setprecision(p) << setw(w) << v[0] << "  "
						<< setprecision(p) << setw(w) << v[1] << "  "
						<< setprecision(p) << setw(w) << v[2] << "  "
						<< " ) = ";
					if (axisnum)
					{
						out << "target axis A" << axisnum << " in ";
					}
					else {
						out << "k vector (latt. units) in ";
					}
					(frame == frameType::LF) ? out << "Lab Frame\n" : out << "Target Frame\n";
				}
				void ddAxisVec::read(std::istream &in, std::vector<double> &v, size_t &axisnum, frameType &frame)
				{
					std::string str;
					std::getline(in, str);

					// The first three numbers are doubles.
					std::vector<std::string> snums;
					snums.reserve(30);
					char lchar = 0; // last character
					char cchar = 0; // current character
					std::string csnum; // current numeric string
					csnum.reserve(200);

					auto nDone = [&]()
					{
						if (csnum.size()) snums.push_back(csnum);
						csnum.clear();
					};

					for (const auto &c : str)
					{
						cchar = c;
						if (std::isdigit(c)) csnum.push_back(c);
						else if (c == '.' && std::isdigit(lchar)) csnum.push_back(c);
						else if (c == '-') csnum.push_back(c);
						else if (csnum.size()) nDone();

						lchar = c;
					}
					nDone();

					using boost::algorithm::trim_copy;
					using std::complex;
					v.resize(3);
					v[0] = boost::lexical_cast<double>(snums[0]);
					v[1] = boost::lexical_cast<double>(snums[1]);
					v[2] = boost::lexical_cast<double>(snums[2]);

					axisnum = 0;
					if (str.at(45) == '1') axisnum = 1;
					if (str.at(45) == '2') axisnum = 2;
					if (axisnum)
					{
						if (str.at(50) == 'T') frame = frameType::TF;
						else frame = frameType::LF;
					}
					else {
						if (str.at(58) == 'T') frame = frameType::TF;
						else frame = frameType::LF;
					}
				}

				void ddPhysExtent::write(std::ostream &out, size_t, const double a, const double b,
					char axisname)
				{
					// Using formatted io operations
					using std::setw;
					using std::setprecision;
					const size_t p = 6, w = 12;
					out << "  " << setprecision(p) << setw(w) << std::right << a << "  "
						<< setprecision(p) << setw(w) << std::right << b << " = " << axisname << "min,"
						<< axisname << "max (physical units)\n";
				}
				void ddPhysExtent::read(std::istream &in, double &a, double &b, char &axisname)
				{
					std::string str;
					std::getline(in, str);

					// The first two numbers are doubles.
					std::vector<std::string> snums;
					snums.reserve(30);
					char lchar = 0; // last character
					char cchar = 0; // current character
					std::string csnum; // current numeric string
					csnum.reserve(200);

					auto nDone = [&]()
					{
						if (csnum.size()) snums.push_back(csnum);
						csnum.clear();
					};

					for (const auto &c : str)
					{
						cchar = c;
						if (std::isdigit(c)) csnum.push_back(c);
						else if (c == '.' && std::isdigit(lchar)) csnum.push_back(c);
						else if (c == '-') csnum.push_back(c);
						else if (csnum.size()) nDone();

						lchar = c;
					}
					nDone();

					using boost::algorithm::trim_copy;
					a = boost::lexical_cast<double>(snums[0]);
					b = boost::lexical_cast<double>(snums[1]);
					axisname = str.at(31);
				}
			}

			/// This is now only used for reading the avg data Mueller matrices.
			/// SCA files' data can be provided by corresponding FML entries.
			void ddOriData::readMuellerDDSCAT(std::istream &in)
			{
				using namespace std;
				using namespace ddOriDataParsers;
				auto od = _parent.avgdata.avg.block<1, ddOutput::stat_entries::NUM_STAT_ENTRIES_DOUBLES>(0, 0);
				//auto od = _parent.oridata_d.block<1, ddOutput::stat_entries::NUM_STAT_ENTRIES_DOUBLES>(_row, 0);

				// The frequency is needed when reading this matrix
				const double f = freq();

				string lin;
				mMuellerIndices mIndices;// = _muellerMap;
				mIndices.clear();
				vector<double> vals;
				vals.reserve(10);

				while (in.good())
				{
					std::getline(in, lin);
					// Parse the string to get rid of spaces. This is used to determine
					// if we are still in the S matrix header or in the actual data
					boost::trim(lin);
					if (!lin.size()) continue;
					//std::cerr << lin << std::endl;
					// TODO: parse the header line to get the list of matrix entries known

					// Expecting the first line to begin with theta phi Pol. ...
					if (std::isalpha(lin.at(0)))
					{
						typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
						boost::char_separator<char> sep("\t ");
						tokenizer t(lin, sep);
						size_t i = 0; // Column number
						for (auto it = t.begin(); it != t.end(); ++it, ++i)
						{
							//std::cerr << "\t i: " << i << " it: " << *it << std::endl;
							// Mueller entry columns have a '_'
							size_t loc = it->find("_");
							if (loc == string::npos) continue;
							//std::cerr << it->substr(loc+1) << std::endl;
							size_t id = boost::lexical_cast<size_t>(it->substr(loc + 1));
							size_t row = (id / 10) - 1; // Annoying start at 1...
							size_t col = (id % 10) - 1;
							//std::cerr << "mIndices loc: " << loc << " id: " << id << " i: " << i << " row: " << row << " col: " << col << std::endl;
							mIndices[i] = std::pair<size_t, size_t>(row, col);
						}

					}
					else {
						// Parse the Mueller entries
						//std::cerr << "Parsing " << lin << std::endl;
						// TODO: check this
						// The ordering is theta, phi, polarization, and then the
						// relevant matrix entries
						// theta phi Pol. S_11 S_12 S_21 S_22 S_31 S_41
						vals.clear();
						if (!parse_numbers_space(lin.begin(), lin.end(), vals))
							ICEDB_throw(icedb::error::error_types::xBadInput)
							.add("Reason","Cannot parse Mueller entry");

						//for (auto it = t.begin(); it != t.end(); ++it)
						//	vals.push_back(rtmath::macros::m_atof(it->data(), it->size())); // Speedup using my own atof
						//vals.push_back(boost::lexical_cast<double>(*it));
						// ddScattMatrixF constructor takes frequency (GHz) and phi
					/*
						ddScattMatrixP mat(new ddScattMatrixP(f, vals[0], vals[1]));
						ddScattMatrix::PnnType P;

						size_t j = 0;
						for (auto ot = mIndices.begin(); ot != mIndices.end(); ++ot, ++j)
						{
							P(ot->first, ot->second) = vals[j]; // See Mueller header read
						}
						mat.setP(P);
						mat.polLin(vals[2]);

						_avgMatricesRaw.push_back(mat);

						//std::shared_ptr<const ddScattMatrix> matC =
						//	boost::dynamic_pointer_cast<const ddScattMatrix>(mat);

						/// \note Actual read of mueller matrix entries disabled
						//ICEDB_throw(icedb::error::error_types::xUnimplementedFunction())
						//	.add("Reason","Actual read of mueller matrix entries disabled");
						//_scattMatricesRaw.push_back(matC);
						//std::cerr << _scattMatricesRaw.size() << " elements\n";
					*/
					}
				}
				/*
							_parent.avgdata.pdata = std::shared_ptr<Eigen::Matrix<float, Eigen::Dynamic,
								avgScaColDefs::NUM_AVGSCACOLDEFS> >(new Eigen::Matrix<float, Eigen::Dynamic,
								avgScaColDefs::NUM_AVGSCACOLDEFS>);
							_parent.avgdata.pdata->resize((int) _avgMatricesRaw.size());
							for (size_t i=0; i < _avgMatricesRaw.size(); ++i) {
								auto o = _parent.avgdata.pdata->block<1, avgScaColDefs::NUM_AVGSCACOLDEFS>(i, 0);
								o(0)
							}
				*/
				//_statTable_Size_ts.at(stat_entries_size_ts::NUMP) = _scattMatricesRaw.size();
			}

			void ddOriData::readF_DDSCAT(std::istream &in,
				std::shared_ptr<const ddScattMatrixConnector> eProvider)
			{
				using namespace std;
				using namespace ddOriDataParsers;

				auto od = _parent.oridata_d.block<1, ddOutput::stat_entries::NUM_STAT_ENTRIES_DOUBLES>(_row, 0);
				//auto &os = _parent.oridata_s.at(_row);
				//auto &oi = _parent.oridata_i.block<1, ddOutput::stat_entries::NUM_STAT_ENTRIES_INTS>(_row, 0);

				// The frequency is needed when reading this matrix
				const double f = freq();

				string lin;

				std::vector<double> vals;
				vals.reserve(10);

				while (in.good())
				{
					std::getline(in, lin);
					if (lin == "") return;
					// Parse the string to get rid of spaces. This is used to determine
					// if we are still in the S matrix header or in the actual data
					boost::trim(lin);
					if (std::isalpha(lin.at(0))) continue;

					vals.clear();
					if (!parse_numbers_space(lin.begin(), lin.end(), vals))
						ICEDB_throw(icedb::error::error_types::xBadInput)
						.add("Reason","Cannot parse F entry");

					// ddScattMatrixF constructor takes frequency (GHz) and phi
					//std::shared_ptr<ddScattMatrixF> mat(new ddScattMatrixF
					//	(freq, vals[0], vals[1], 0, 0, eProvider));
					ddScattMatrixF mat(f, vals[0], vals[1], 0, 0, eProvider);
					ddScattMatrix::FType fs;
					fs(0, 0) = complex<double>(vals[2], vals[3]);
					fs(1, 0) = complex<double>(vals[4], vals[5]);
					fs(0, 1) = complex<double>(vals[6], vals[7]);
					fs(1, 1) = complex<double>(vals[8], vals[9]);
					mat.setF(fs);

					//std::shared_ptr<const ddScattMatrix> matC =
					//	boost::dynamic_pointer_cast<const ddScattMatrix>(mat);

					//_scattMatricesRaw.push_back(matC);
					_scattMatricesRaw.push_back(mat);
				}

				//_statTable_Size_ts.at(stat_entries_size_ts::NUMF) = _scattMatricesRaw.size();
			}

			void ddOriData::readS_ADDA(std::istream &in)
			{
				ICEDB_throw(icedb::error::error_types::xUnimplementedFunction);
				/*
				using namespace std;
				using namespace rtmath::ddscat::ddOriDataParsers;

				auto od = _parent.oridata_d.block<1, ddOutput::stat_entries::NUM_STAT_ENTRIES_DOUBLES>(_row, 0);
				//auto &os = _parent.oridata_s.at(_row);
				//auto &oi = _parent.oridata_i.block<1, ddOutput::stat_entries::NUM_STAT_ENTRIES_INTS>(_row, 0);

				// The frequency is needed when reading this matrix
				const double f = freq();

				string lin;

				std::vector<double> vals;
				vals.reserve(10);

				while (in.good())
				{
					std::getline(in, lin);
					if (lin == "") return;
					// Parse the string to get rid of spaces. This is used to determine
					// if we are still in the S matrix header or in the actual data
					boost::trim(lin);
					if (std::isalpha(lin.at(0))) continue;

					vals.clear();
					if (!parse_numbers_space(lin.begin(), lin.end(), vals))
						throw debug::xBadInput("Cannot parse S entry");

					// TODO: Add check to see if phi is a column or not!!!!!

					// ddScattMatrixF constructor takes frequency (GHz) and phi
					//std::shared_ptr<ddScattMatrixF> mat(new ddScattMatrixF
					//	(freq, vals[0], vals[1], 0, 0, eProvider));
					ddScattMatrixS mat(f, vals[0], vals[1], 0, 0, eProvider);
					ddScattMatrix::FType fs;
					fs(0, 0) = complex<double>(vals[2], vals[3]);
					fs(1, 0) = complex<double>(vals[4], vals[5]);
					fs(0, 1) = complex<double>(vals[6], vals[7]);
					fs(1, 1) = complex<double>(vals[8], vals[9]);
					mat.setF(fs);

					//std::shared_ptr<const ddScattMatrix> matC =
					//	boost::dynamic_pointer_cast<const ddScattMatrix>(mat);

					//_scattMatricesRaw.push_back(matC);
					_scattMatricesRaw.push_back(mat);
				}

				//_statTable_Size_ts.at(stat_entries_size_ts::NUMF) = _scattMatricesRaw.size();
				*/
			}

		}
	}
}
