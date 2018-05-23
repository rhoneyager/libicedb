#include <string>
#include <iostream>
#define BOOST_TEST_DYN_LINK
#include <HH/Handles.hpp>
#include <HH/Attributes.hpp>
//#include <HH/Datasets.hpp>
#include <HH/Files.hpp>
//#include <HH/Groups.hpp>
#include <HH/Handles_HDF.hpp>
#include <HH/Handles_Valid.hpp>
#include <HH/Tags.hpp>
#include <HH/Types.hpp>
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(HDFforHumans_Objects);

using namespace std;
using namespace HH;

// Open a basic temporary file, and set some attributes directly.
BOOST_AUTO_TEST_CASE(direct_attr_set) {
	auto f = HH::Files::create_file_image("direct_attr_set");
	Has_Attributes atts(f.getWeakHandle());
	vector<int16_t> data = { 1, 2, 3, 4, 5 };
	gsl::span<int16_t> d(data);
	atts.add<int16_t>("test_i16", d);
	atts.add<int16_t>("test_i162", { 2, 3, 4, 1 });
	atts.add<int16_t>("test_i163", 5);
	atts.add("test_i164", d);
	atts.add("j", { 1.0f }, { 1 });

	auto attj = atts.open("j");
	float vf = attj.read<float>();

	//attj.read()
	//atts.add()
	//	<int8_t>("a", 1)
	//	<uint32_t>("b", { 2,3,4 })
	//	("c","test string")
	//	;
}



BOOST_AUTO_TEST_SUITE_END();

