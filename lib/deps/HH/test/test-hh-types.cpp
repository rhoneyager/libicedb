#include <string>
#include <iostream>
#include <map>
#include <memory>
#include <exception>
#include <tuple>
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "HH/Types.hpp"
#include "HH/Types/Complex.hpp"
#include "IntegratedTesting.hpp"

BOOST_AUTO_TEST_SUITE(HH_Types);

BOOST_AUTO_TEST_CASE(GetTypes)
{
	BOOST_TEST(HH::Types::GetHDF5Type<bool>().isValid() == true);
	BOOST_TEST(HH::Types::GetHDF5Type<char>().isValid() == true);
	BOOST_TEST(HH::Types::GetHDF5Type<int8_t>().isValid() == true);
	BOOST_TEST(HH::Types::GetHDF5Type<uint8_t>().isValid() == true);
	BOOST_TEST(HH::Types::GetHDF5Type<int16_t>().isValid() == true);
	BOOST_TEST(HH::Types::GetHDF5Type<uint16_t>().isValid() == true);
	BOOST_TEST(HH::Types::GetHDF5Type<int16_t const>().isValid() == true);
	BOOST_TEST(HH::Types::GetHDF5Type<uint32_t>().isValid() == true);
	BOOST_TEST(HH::Types::GetHDF5Type<int32_t>().isValid() == true);
	BOOST_TEST(HH::Types::GetHDF5Type<int32_t const>().isValid() == true);
	BOOST_TEST(HH::Types::GetHDF5Type<uint64_t>().isValid() == true);
	BOOST_TEST(HH::Types::GetHDF5Type<int64_t>().isValid() == true);
	BOOST_TEST(HH::Types::GetHDF5Type<float>().isValid() == true);
	BOOST_TEST(HH::Types::GetHDF5Type<double>().isValid() == true);

	BOOST_TEST(HH::Types::GetHDF5TypeFixedString(16).isValid() == true);
	BOOST_TEST(HH::Types::GetHDF5Type<std::string>().isValid() == true);
}

BOOST_AUTO_TEST_SUITE_END();
