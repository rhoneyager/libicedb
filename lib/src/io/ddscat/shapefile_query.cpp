#include "Stdafx-ddscat.h"
#pragma warning( disable : 4996 ) // -D_SCL_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <tuple>
#include <boost/math/constants/constants.hpp>
#include <boost/lexical_cast.hpp>

#include <Ryan_Debug/debug.h>
#include <Ryan_Debug/macros.h>
#include <Ryan_Debug/hash.h>
#include <Ryan_Debug/splitSet.h>
#include <Ryan_Debug/registry.h>
#include <Ryan_Debug/error.h>
#include "../rtmath/ddscat/shapefile.h"
#include "../rtmath/error/debug.h"

namespace Ryan_Debug {
	namespace registry {
		template struct IO_class_registry_writer
			< ::rtmath::ddscat::shapefile::shapefile > ;

		template struct IO_class_registry_reader
			< ::rtmath::ddscat::shapefile::shapefile > ;

		template class usesDLLregistry <
			::rtmath::ddscat::shapefile::shapefile_IO_output_registry,
			IO_class_registry_writer<::rtmath::ddscat::shapefile::shapefile> > ;

		template class usesDLLregistry <
			::rtmath::ddscat::shapefile::shapefile_IO_input_registry,
			IO_class_registry_reader<::rtmath::ddscat::shapefile::shapefile> > ;

	}
}
namespace rtmath {
	namespace ddscat {
		namespace shapefile {

			bool shapefile_db_registry::shapefile_db_comp::operator()(const std::shared_ptr<const shapefile>& lhs, const std::shared_ptr<const shapefile>& rhs) const
			{
				return *lhs < *rhs;
			}

			bool shapefile_db_registry::shapefile_db_comp::operator()(const boost::shared_ptr<const shapefile>& lhs, const boost::shared_ptr<const shapefile>& rhs) const
			{
				return *lhs < *rhs;
			}


			bool shapefile::operator!=(const shapefile &rhs) const { return !operator==(rhs); }
			bool shapefile::operator==(const shapefile &rhs) const { return hash().lower == rhs.hash().lower; }
			bool shapefile::operator<(const shapefile &rhs) const { return numPoints < rhs.numPoints; }

			shapefile_db_registry::shapefile_db_registry() : name(nullptr), fQuery(nullptr) {}
			shapefile_db_registry::~shapefile_db_registry() {}

			shapefile_db_registry::shapefile_index::shapefile_index() {}
			shapefile_db_registry::shapefile_index::~shapefile_index() {}
			std::shared_ptr<shapefile_db_registry::shapefile_index> 
				shapefile_db_registry::shapefile_index::generate()
			{
				std::shared_ptr<shapefile_db_registry::shapefile_index> res
					(new shapefile_index());
				return res;
			}


			std::pair<shapefile_db_registry::shapefile_index::collection, std::shared_ptr<Ryan_Debug::registry::DBhandler> >
				shapefile_db_registry::shapefile_index::doQuery(std::shared_ptr<Ryan_Debug::registry::DBhandler> p, std::shared_ptr<Ryan_Debug::registry::DB_options> o) const
			{
				collection c(new std::set<boost::shared_ptr<const shapefile>, shapefile_db_comp >());
				std::shared_ptr<Ryan_Debug::registry::DBhandler> fp;

				auto hooks = ::Ryan_Debug::registry::usesDLLregistry<shapefile_query_registry, shapefile_db_registry >::getHooks();
				for (const auto &h : *(hooks.get()))
				{
					if (!h.fQuery) continue;
					if (!h.fMatches) continue;
					if (!h.fMatches(p, o)) continue;
					fp = h.fQuery(*this, c, p, o);

					return std::pair < shapefile_db_registry::shapefile_index::collection,
						std::shared_ptr<Ryan_Debug::registry::DBhandler> >(c, fp);
				}

				return std::pair<shapefile_db_registry::shapefile_index::collection, 
					std::shared_ptr<Ryan_Debug::registry::DBhandler> >
					(c, nullptr);
			}


			bool shapefile_db_registry::shapefile_index::filter(const shapefile *s) const
			{
				// Tag filtering - OR filtering
				if (tags.size())
				{
					bool matches = false;
					for (const auto &t : tags)
					{
						if (s->tags.count(t.first))
						{
							if (s->tags.at(t.first) == t.second)
								matches = true;
							break;
						}
					}
					if (!matches) return false;
				}

				// hash filtering
				if (hashLowers.size() && !hashLowers.count(s->hash().string()))
					return false;
				if (hashUppers.size() && !hashUppers.count(boost::lexical_cast<std::string>(s->hash().upper)))
					return false;

				// flake types
				if (flakeTypes.size())
				{
					if (s->tags.count("flake_classification"))
					{
						if (!flakeTypes.count(s->tags.at("flake_classification")))
							return false;
					}
				}

				// refHashLowers
				if (s->tags.count("flake_reference"))
				{
					if (refHashLowers.size() &&
						std::find(refHashLowers.begin(), refHashLowers.end(),
						boost::lexical_cast<std::string>(s->hash().lower)) == refHashLowers.end())
						return false;
				}

				// Dipole spacings
				if (dipoleSpacings.ranges.size())
					if (!dipoleSpacings.inRange(s->standardD)) return false;

				// Number of dipoles
				if (dipoleNumbers.ranges.size())
					if (!dipoleNumbers.inRange(s->numPoints)) return false;

				return true;
			}

			std::pair<shapefile_db_registry::shapefile_index::collection, 
				std::shared_ptr<Ryan_Debug::registry::DBhandler> >
				shapefile_db_registry::shapefile_index::doQuery(
				collection srcs, bool doUnion, bool doDb,
				std::shared_ptr<Ryan_Debug::registry::DBhandler> p,
				std::shared_ptr<Ryan_Debug::registry::DB_options> o) const
			{
				collection toMergeC(new std::set<boost::shared_ptr<const shapefile>, shapefile_db_comp >());
				//collection toMergeS(new std::set<boost::shared_ptr<shapefile>, shapefile_db_comp >());
				collection res(new std::set<boost::shared_ptr<const shapefile>, shapefile_db_comp >());
				std::shared_ptr<Ryan_Debug::registry::DBhandler> fp;

				std::map<Ryan_Debug::hash::HASH_t, boost::shared_ptr<const shapefile> > db_hashes; // database results, as a map

				auto hooks = ::Ryan_Debug::registry::usesDLLregistry<shapefile_query_registry, shapefile_db_registry >::getHooks();
				if (doDb)
				{
					for (const auto &h : *(hooks.get()))
					{
						if (!h.fQuery) continue;
						if (!h.fMatches) continue;
						if (!h.fMatches(p, o)) continue;
						fp = h.fQuery(*this, toMergeC, p, o);

						//return std::pair < shapefile_db_registry::shapefile_index::collection,
						//	std::shared_ptr<rtmath::registry::DBhandler> >(c, fp);
					}

					for (const auto &r : *(toMergeC))
						db_hashes[r->hash()] = r;
				}



				// Also perform the same filtering on srcs, putting results in res
				for (auto &s : *(srcs))
				{
					if (!filter(s.get())) continue;

					res->insert(s); // Passed filtering
					// Merge the results of the provided object with any query results
					if (db_hashes.count(s->hash()))
					{
						auto d = db_hashes.at(s->hash());

						/// \note This is not good.
						boost::shared_ptr<shapefile> ucs = boost::const_pointer_cast<shapefile>(s);

						if (d->standardD) ucs->standardD = d->standardD;
						if (d->numPoints) ucs->numPoints = d->numPoints;
						if (d->desc.size()) ucs->desc = d->desc;
						for (const auto &tag : d->tags)
							ucs->tags[tag.first] = tag.second;

						db_hashes.erase(s->hash()); // Remove from consideration (already matched)
					}
				}

				// If doUnion, select the remainder of doHashes and insert
				if (doUnion)
					for (const auto &d : db_hashes)
						res->insert(d.second);


				return std::pair<shapefile_db_registry::shapefile_index::collection,
					std::shared_ptr<Ryan_Debug::registry::DBhandler> >
					(res, fp);
			}
		}
	}
}

