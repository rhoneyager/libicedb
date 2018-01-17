#include "ddscat_versions.hpp"
//#pragma warning( disable : 4996 ) // -D_SCL_SECURE_NO_WARNINGS

#include <mutex>
#include <thread>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/qi_omit.hpp>
#include <boost/spirit/include/qi_repeat.hpp>
#include <iostream>
#include <sstream>
#include <vector>
#include <exception>


/// Internal namespace for the reader parsers
namespace {
	namespace qi = boost::spirit::qi;
	namespace ascii = boost::spirit::ascii;
	namespace phoenix = boost::phoenix;

	/// Parses a version number string
	template <typename Iterator>
	bool parse_ddversion(Iterator first, Iterator last, std::vector<unsigned int> &v)
	{
		using qi::double_;
		using qi::uint_;
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
				//omit[string("DDSCAT")] >> 
				omit[repeat(0, 6)[*char_("a-zA-Z,-.='")]] >>
				uint_[push_back(phoenix::ref(v), _1)] >>
				*('.' >> uint_[push_back(phoenix::ref(v), _1)])
				// >> omit[repeat[*char_]]
				)
			,
			//  End grammar

			space);

		if (first != last) // fail if we did not get a full match
			return false;
		return r;
	}
}

namespace
{
	bool verMapInited = false;
	std::mutex verMapMutex;
	typedef std::map< size_t, std::string > verMapType;
	verMapType avgMap;
	void initVerMap()
	{
		std::lock_guard<std::mutex> guard(verMapMutex);
		if (verMapInited) return;
		verMapInited = true;

		avgMap.insert(verMapType::value_type(70, "DDSCAT 7.0 [08.09.03]"));
		avgMap.insert(verMapType::value_type(707, "DDSCAT 7.0.7 [09.12.11]"));
		avgMap.insert(verMapType::value_type(708, "DDSCAT 7.0.8 [10.02.04]"));
		avgMap.insert(verMapType::value_type(71, "DDSCAT 7.1 [10.02.07]"));
		avgMap.insert(verMapType::value_type(710, "DDSCAT 7.1.0 [10.03.03]"));
		avgMap.insert(verMapType::value_type(720, "DDSCAT 7.2.0 [12.02.16]"));
		avgMap.insert(verMapType::value_type(721, "DDSCAT 7.2.1 [12.05.14]"));
		avgMap.insert(verMapType::value_type(722, "DDSCAT 7.2.2 [12.06.05]"));
		avgMap.insert(verMapType::value_type(73, "DDSCAT 7.3 [13.04.04]-BETA"));
		avgMap.insert(verMapType::value_type(730, "DDSCAT 7.3.0 [13.05.03]"));
		avgMap.insert(verMapType::value_type(731, "DDSCAT 7.3.1 [16.08.25]"));
		avgMap.insert(verMapType::value_type(732, "DDSCAT 7.3.2 [16.06.08]"));
	}
}

namespace icedb
{
	namespace DDSCAT
	{
		namespace versioning
		{
			size_t getVerId(const std::string& s)
			{
				//initVerMap();
				// Use boost::regex to find the 0.0.0 matching string,
				// then expand it.
				std::string::const_iterator start, end;
				start = s.begin();
				end = s.end();

				std::vector<unsigned int> v;
				parse_ddversion(start, end, v);

				size_t ret = 0;
				if (!v.size()) throw std::invalid_argument("Bad version id");
				for (auto it = v.begin(); it != v.end(); ++it)
				{
					size_t i = static_cast<size_t>(*it);
					ret *= 10;
					ret += i;
				}

				return ret;
			}

			std::string getVerString(size_t id)
			{
				std::string s;
				std::ostringstream out;
				std::vector<size_t> v;
				// Just progressively chop off the end figures
				do
				{
					size_t i = id % 10;
					v.push_back(i);
					id = id / 10;
				} while (id);

				for (auto it = v.rbegin(); it != v.rend(); ++it)
				{
					if (it != v.rbegin()) out << ".";
					out << *it;
				}

				s = out.str();
				return s;
			}

			std::string getVerAvgHeaderString(size_t id)
			{
				initVerMap();
				std::string s;
				if (avgMap.count(id)) s = avgMap.at(id);
				else throw std::invalid_argument("Unknown DDSCAT version");
				return s;
			}

			bool isVerWithin(size_t ver, size_t min, size_t max)
			{
				// Needed because of the obtuse ddscat versioning scheme
				// ex: 7.2.0 is the first release after 7.2

				// min is inclusive, max is exclusive. If zero is passed to max, then
				// there is no upper bound.

				if (ver < 100)
				{
					bool maxM = false; // Flag set when upper version > 100 (like 7.2.0)
					if (min > 100) min /= 10;
					if (max > 100) { max /= 10; maxM = true; }
					if (ver < min) return false;
					if (!max) return true;
					if (ver > max) return false;
					if (ver != max) return true;
					if (maxM) return true; // 7.2 < 7.2.0 case
					return false;
				}
				else { // ver has three digits
					if (min < 100) min *= 10;
					if (max < 100) max *= 10;

					if (ver < min) return false; // min 2-digit case handled above
					if (!max) return true;
					if (ver > max) return false;
					return true; // ver is in [0,max).
				}
			}

			bool isVerWithin(size_t ver, const std::string &range)
			{
				// range represents several intervals, and if ver is within 
				// the union of these intervals, the expression evaluates to true.
				// each range interval is comma-delimeted. the interval itself 
				// is expressed using : notation. 

				using namespace std;
				// Prepare tokenizer
				typedef boost::tokenizer<boost::char_separator<char> >
					tokenizer;
				boost::char_separator<char> sep(",");
				boost::char_separator<char> seprange(":");
				tokenizer tcom(range, sep);
				for (auto ot = tcom.begin(); ot != tcom.end(); ++ot)
				{
					tokenizer trange(*ot, seprange);
					vector<size_t> range;
					size_t i = 0;
					for (auto rt = trange.begin(); rt != trange.end(); rt++, i++)
					{
						try {
							string s = *rt;
							boost::algorithm::trim(s);
							if (i < 3)
							{
								range.push_back(boost::lexical_cast<size_t>(s));
							}
						}
						catch (...)
						{
							throw std::invalid_argument("Bad input string");
						}
					}

					if (!range.size()) continue;
					size_t min = *range.begin();
					size_t max = *range.rbegin();

					if (isVerWithin(ver, min, max)) return true;
				}

				return false;
			}

			size_t getDefaultVer()
			{
#pragma message("Warning: ddVersions needs a full getDefaultVer() implementation")
				return 730;
			}
		}
	}
}
