#include <string>
#include <iostream>
#define BOOST_TEST_DYN_LINK
//#include <HH/Handles.hpp>
//#include <HH/Handles_HDF.hpp>
//#include <HH/Handles_Valid.hpp>
#include <HH/Types.hpp>
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(HDFforHumans_Types);

using namespace std;
using namespace HH;

// Type tests

BOOST_AUTO_TEST_CASE(types_are_they_string_types) {
	using namespace HH::Types;
	BOOST_CHECK_EQUAL(is_string<int>::value, false);
	BOOST_CHECK_EQUAL(is_string<float>::value, false);
	BOOST_CHECK_EQUAL(is_string<char>::value, false);
	BOOST_CHECK_EQUAL(is_string<unsigned char>::value, false);
	BOOST_CHECK_EQUAL(is_string<unsigned long>::value, false);
	BOOST_CHECK_EQUAL(is_string<double>::value, false);
	BOOST_CHECK_EQUAL(is_string<std::string>::value, true);
	BOOST_CHECK_EQUAL(is_string<const std::string>::value, true);
	BOOST_CHECK_EQUAL(is_string<const char*>::value, true);
	BOOST_CHECK_EQUAL(is_string<char*>::value, true);
	BOOST_CHECK_EQUAL(is_string<const char[]>::value, true);
	BOOST_CHECK_EQUAL(is_string<char[]>::value, true);
}

// Construct various types, and do equality checks

// Construct a custom struct as a type

// Construct a complex number as a type

// Construct array types




BOOST_AUTO_TEST_SUITE_END();

