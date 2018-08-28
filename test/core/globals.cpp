#include <boost/filesystem.hpp>
#define BOOST_TEST_NO_MAIN
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <exception>
#include <string>
#include <iostream>
#include <sstream>
#include <set>

#include "globals.hpp"
#include <icedb/defs.h>

globals::globals()
//: cRoot(nullptr)
{
	BOOST_TEST_MESSAGE("Loading globals");
	instance() = this; // Really only meant to be invoked once
					   //rtmath::debug::instances::registerInstance( "test globals", reinterpret_cast<void*>(this));

	try {
		//findDirs();
		//loadConfig();
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		BOOST_FAIL("Test threw an error.");
	}
}

globals::~globals()
{
	BOOST_TEST_MESSAGE("Unloading globals");
}

/*
void globals::findDirs()
{
	using namespace std;
	using namespace boost::filesystem;

	// Find test data
	{
		set<string> cand;
		cand.insert("./data");
		cand.insert("../rtmath/rtmath/tests/data");

		bool done = false;
		for (auto it = cand.begin(); it != cand.end() && done == false; it++)
		{
			pTestData = path(*it);
			if (exists(pTestData)) done = true;
		}
		if (!done)
			BOOST_FAIL("Cannot find test data directory.");
	}

	// Find full data
	{
		set<string> cand;
		cand.insert("../data");
		cand.insert("../rtmath/rtmath/data");
		bool done = false;
		for (auto it = cand.begin(); it != cand.end() && done == false; it++)
		{
			pFullData = path(*it);
			if (exists(pFullData)) done = true;
		}
		if (!done)
			BOOST_FAIL("Cannot find full data directory.");
	}

	pProfiles = pFullData / "profiles/";
	pRtconf = pTestData / "rtmath.conf";

}

void globals::loadConfig()
{
	using namespace std;
	using namespace boost::filesystem;
	try {
		if (!exists(pRtconf))
			throw rtmath::debug::xMissingFile(pRtconf.string().c_str());
		cRoot = rtmath::config::loadRtconfRoot(pRtconf.string());
	}
	catch (std::exception &e)
	{
		cerr << e.what() << endl;
		BOOST_FAIL("Cannot load testing rtmath.conf file! Many tests cannot proceed.");
	}
}
*/


globals*& globals::instance()
{
	static globals* s_inst = nullptr;
	return s_inst;
}
