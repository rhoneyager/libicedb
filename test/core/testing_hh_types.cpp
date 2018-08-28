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
	BOOST_WARN_EQUAL(is_string<const std::string>::value, true); // TODO: Fix this case.
	BOOST_CHECK_EQUAL(is_string<const char*>::value, true);
	BOOST_CHECK_EQUAL(is_string<char*>::value, true);
	BOOST_CHECK_EQUAL(is_string<const char[]>::value, true);
	BOOST_CHECK_EQUAL(is_string<char[]>::value, true);
}

// Just make sure that the types generate, and are not invalid
BOOST_AUTO_TEST_CASE(types_fundamental_are_valid) {
	using namespace HH::Types;
	BOOST_CHECK_EQUAL(GetHDF5Type<char>().isInvalid(), false);
	BOOST_CHECK_EQUAL(GetHDF5Type<int8_t>().isInvalid(), false);
	BOOST_CHECK_EQUAL(GetHDF5Type<uint8_t>().isInvalid(), false);
	BOOST_CHECK_EQUAL(GetHDF5Type<int16_t>().isInvalid(), false);
	BOOST_CHECK_EQUAL(GetHDF5Type<uint16_t>().isInvalid(), false);
	BOOST_CHECK_EQUAL(GetHDF5Type<int32_t>().isInvalid(), false);
	BOOST_CHECK_EQUAL(GetHDF5Type<uint32_t>().isInvalid(), false);
	BOOST_CHECK_EQUAL(GetHDF5Type<int64_t>().isInvalid(), false);
	BOOST_CHECK_EQUAL(GetHDF5Type<uint64_t>().isInvalid(), false);
	BOOST_CHECK_EQUAL(GetHDF5Type<float>().isInvalid(), false);
	BOOST_CHECK_EQUAL(GetHDF5Type<double>().isInvalid(), false);
}

BOOST_AUTO_TEST_CASE(types_strings_are_valid) {
	using namespace HH::Types;
	BOOST_CHECK_EQUAL(GetHDF5Type<std::string>().isInvalid(), false);
	BOOST_CHECK_EQUAL(GetHDF5Type<const char*>().isInvalid(), false);
	// For some reason, an extra set of parentheses are needed here. Macro fun.
	BOOST_CHECK_EQUAL((GetHDF5Type<std::string,15>().isInvalid()), false);
	BOOST_CHECK_EQUAL((GetHDF5Type<const char*, 15>().isInvalid()), false);

	BOOST_CHECK_EQUAL(GetHDF5Type<std::string>(16).isInvalid(), false);
	BOOST_CHECK_EQUAL(GetHDF5Type<const char*>(16).isInvalid(), false);
}


// Construct various types, and do equality checks

// Construct a custom struct as a type

// Construct a complex number as a type

// Construct array types




BOOST_AUTO_TEST_SUITE_END();

