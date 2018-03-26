#pragma once
#include "defs.h"

#include <string>
#include <map>
#include <set>
#include <vector>

namespace icedb {
	namespace splitSet {
		/// Function that expands sets of _numbers_ with 
		/// separators of commas, dashes and colons.
		template <class T>
		void splitSet(
			const std::string &instr, 
			std::set<T> &expanded,
			const std::map<std::string, std::string> *aliases = nullptr);

		/// Specialization for splitting strings. These 
		/// objects have no ranges to be compared against.
		template <> void splitSet<std::string>(
			const std::string &instr, 
			std::set<std::string> &expanded,
			const std::map<std::string, std::string> *aliases);

		/// Shortcut that already passes parsed ICEDB_LOG_INFOrmation
		template <class T>
		void splitSet(
			const T &start, const T &end, const T &interval,
			const std::string &specializer,
			std::set<T> &expanded);

		/// Extracts ICEDB_LOG_INFOrmation from interval notation
		template <class T>
		void extractInterval(
			const std::string &instr,
			T &start, T &end, T &interval, size_t &num,
			std::string &specializer);

		/** \brief Convenience function to split a null-separated string list into a vector of strings.
		*
		* Commonly-used to split up the results of a Ryan_Debug::ProcessICEDB_LOG_INFO command-line structure.
		**/
		void splitVector(
			const std::string &instr, std::vector<std::string> &out, char delim = '\0');
		inline void splitNullVector(
			const std::string &instr, std::vector<std::string> &out) { splitVector(instr, out); }

		/** \brief Convenience function to split a null-separated string list into a map of strings.
		*
		* Commonly-used to split up the results of a Ryan_Debug::ProcessICEDB_LOG_INFO environment structure.
		**/
		void splitNullMap(
			const std::string &instr, std::map<std::string, std::string> &out);


		/** \brief Class to define and search on intervals.
		*
		* This is a simple class used for searching based on user input. It does not 
		* provide interval unions, intersections, etc. It can, however, aid in setting 
		* these up, such as for a database query.
		* 
		* Accepts standard paramSet notation, but also adds the '-' range operator, 
		* implying that values may be found in a certain range.
		**/
		template <class T>
		class intervals
		{
		public:
			std::vector<std::pair<T, T> > ranges;
			intervals(const std::string &s = "");
			intervals(const std::vector<std::string> &s);
			~intervals();
			void append(const std::string &instr,
				const std::map<std::string, std::string> *aliases = nullptr);
			void append(const std::vector<std::string> &s,
				const std::map<std::string, std::string> *aliases = nullptr);
			void append(const intervals<T>& src);
			bool inRange(const T& val) const;
			bool isNear(const T& val, const T& linSep, const T& factorSep) const;
		};
	}
}

//#if defined _MSC_FULL_VER
//#pragma warning(pop)
//#endif
