#include <string>
#include <iostream>
#include <map>
#include <memory>
#include <exception>
#include <tuple>
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "HH/Errors.hpp"
#include "IntegratedTesting.hpp"

BOOST_AUTO_TEST_SUITE(HH_Errors);

/*
BOOST_AUTO_TEST_CASE(GenerateError)
{
	try {
		throw HH_throw.add("Reason", "This is a test.");
	}
	catch (const HH::Error&) {
		BOOST_TEST(true);
	}
	catch (...) {
		BOOST_FAIL("Could not catch the HH::xError.");
	}
}
*/

BOOST_AUTO_TEST_SUITE_END();
