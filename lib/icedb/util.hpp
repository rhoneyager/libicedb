#pragma once
#include "defs.h"
#include <memory>
#include <type_traits>
#include <new>
#include "HH/Groups.hpp"
#include <map>
#include <sstream>
#include <string>
//#include <gsl/gsl>

namespace icedb {
	/** Allocate memory in bytes. Generally this is just malloced, but a custom allocator may be substituted. **/
	ICEDB_DL void* _malloc(size_t numBytes);

	template <class T>
	T* malloc(size_t numBytes) {
		T* res = static_cast<T*>(_malloc(numBytes));
		return res;
	}

	/** Free memory region. Should not be double-freed. **/
	ICEDB_DL void _free(void* obj);
	template <class T>
	void free(T* obj) {
		_free(static_cast<void*>(obj));
	}

	namespace Validate {

		typedef std::string ObjectName_t;
		typedef HH::HH_hid_t ObjectStorageType_t;
		typedef std::pair< ObjectName_t, ObjectStorageType_t> ObjectNameType_t;
		typedef std::pair<bool, std::string> TestResultAndReason_t;

		inline TestResultAndReason_t TestAttExistenceAndType(HH::Group& g, const ObjectNameType_t& p)
		{
			Expects(g.isGroup());
			using std::make_pair;
			using std::string;
			const auto& name = p.first;
			const auto& type = p.second;
			if (!g.atts.exists(name)) return make_pair(false, string("Attribute does not exist: ").append(name));
			if (H5Tequal(g.atts[name].getType()(), type()) <= 0) return make_pair(false, string("Attribute does not have the expected type: ").append(name));
			return make_pair(true, string("Attribute is valid: ").append(name));
		}

		template <typename DataType>
		TestResultAndReason_t TestAttExistenceAndType(HH::Group & g, const std::string & attname) {
			return TestAttExistenceAndType(g,
				std::make_pair(attname, HH::Types::GetHDF5Type<DataType>()));
		}

		inline TestResultAndReason_t TestDsetExistenceAndType(HH::Group & g, const ObjectNameType_t & p)
		{
			Expects(g.isGroup());
			using std::make_pair;
			using std::string;
			const auto& name = p.first;
			const auto& type = p.second;
			if (!g.dsets.exists(name)) return make_pair(false, string("Dataset does not exist: ").append(name));
			if (H5Tequal(g.dsets[name].getType()(), type()) <= 0) return make_pair(false, string("Dataset does not have the expected type: ").append(name));
			return make_pair(true, string("Dataset is valid: ").append(name));
		}

		template <typename DataType>
		TestResultAndReason_t TestDsetExistenceAndType(HH::Group & g, const std::string & dsetname) {
			return TestDsetExistenceAndType(g,
				std::make_pair(dsetname, HH::Types::GetHDF5Type<DataType>()));
		}

		template <typename Object_Key_t>
		TestResultAndReason_t TestGroupIsValid(HH::Group g,
			const std::map<Object_Key_t, ObjectNameType_t> & attsToTest,
			const std::map<Object_Key_t, ObjectNameType_t> & dsetsToTest)
		{
			Expects(g.isGroup());
			using std::make_pair;
			bool bad = false;
			std::ostringstream diagnosticInfo;

			// Test essential attribute and variable existence, and test that they are the appropriate type.
			TestResultAndReason_t res;
			for (auto& c : attsToTest) {
				res = TestAttExistenceAndType(g, c.second);
				if (!res.first) {
					bad = true;
					diagnosticInfo << "Failed attribute test for "
						<< c.second.first << ": " << res.second << std::endl;
				}
			}

			for (auto& d : dsetsToTest) {
				res = TestDsetExistenceAndType(g, d.second);
				if (!res.first) {
					bad = true;
					diagnosticInfo << "Failed attribute test for "
						<< d.second.first << ": " << res.second << std::endl;
				}
			}

			return make_pair(!bad, diagnosticInfo.str());
		};
	}
}
