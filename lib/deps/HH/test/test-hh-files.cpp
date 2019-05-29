#include <string>
#include <iostream>
#include <map>
#include <memory>
#include <exception>
#include <tuple>
#include <vector>
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "IntegratedTesting.hpp"
#include "HH/Files.hpp"

BOOST_AUTO_TEST_SUITE(HH_Files);

BOOST_AUTO_TEST_CASE(create_basic)
{
	using namespace std;
	HH::File f = Testing::createTestFile();
	BOOST_TEST_REQUIRE(f.isGroup());
	auto g = f.create("g");
	BOOST_TEST_REQUIRE(g.isGroup());
	g.atts.add<int16_t>("i16t_1", { 1 });
	g.atts.add<int16_t>("i16t_22", { 1, 2, 3, 4 }, {2, 2});
	BOOST_TEST(g.atts.exists("i16t_1"));
	BOOST_TEST(g.atts.exists("i16t_22"));
	g.atts.add<std::string>("s1", { "Test 1" });
	BOOST_TEST(g.atts.exists("s1"));
	g.atts.add<std::string>("s2", { "Test 2a", "Test2b" });
	BOOST_TEST(g.atts.exists("s2"));
	g.atts.add<std::string>("s3", { "Test 3a", "Test3b", "Test3c", "Test3d" }, { 2, 2 });
	BOOST_TEST(g.atts.exists("s3"));

	auto ga = g.atts["i16t_1"];
	int16_t ia = ga.read<int16_t>();
	BOOST_TEST(ia == 1);
	auto gb = g.atts["i16t_22"];
	vector<int16_t> gb_d(4);
	gb.read<int16_t>(gb_d);
	BOOST_TEST(gb_d[3] == 4);

	auto as1 = g.atts["s1"];
	string s1;
	s1 = as1.read<std::string>();
	BOOST_TEST(s1 == "Test 1");

	vector<string> vs3(4);
	auto as3 = g.atts["s3"];
	as3.read(vs3);
	BOOST_TEST(vs3[0] == "Test 3a");
	BOOST_TEST(vs3[1] == "Test3b");
	BOOST_TEST(vs3[2] == "Test3c");
	BOOST_TEST(vs3[3] == "Test3d");

	// This won't work yet.
	/// \todo Add fixed string read support!!!
	//g.atts.addFixedLengthString("f1", "fixed-1");
	//string sf;
	//sf = g.atts["f1"].read<std::string>();
	//BOOST_TEST(sf == "fixed-1");

	// Attempt to read and write datasets
	auto di1 = g.dsets.create<int>("di1", { 1 }).write<int>({ 3 });
	auto ds1 = g.dsets.create<string>("ds1", { 1 }).write<string>({ "Test" });
	auto ds2 = g.dsets.create<string>("ds2", { 2 }).write<string>({ "Test", "T2" });
	auto ds32 = g.dsets.create<string>("ds32", { 3, 2 }).write<string>({ "Test", "T2", "T3", "B1", "B2", "B3" });

	BOOST_TEST_CHECKPOINT("Datasets written");

	vector<int> vi1(1);
	di1.read<int>(vi1);
	BOOST_TEST(vi1[0] == 3);
	BOOST_TEST_CHECKPOINT("Int read");

	vector<string> dvs1(1), dvs2(2), dvs32(6);
	ds1.read<string>(dvs1);
	BOOST_TEST_REQUIRE(dvs1.size() > 0);
	string sdvs1 = dvs1.at(0);
	BOOST_TEST(sdvs1 == "Test");
	BOOST_TEST_CHECKPOINT("Vector<string> of size 1 read");
	ds2.read<string>(dvs2);
	BOOST_TEST_REQUIRE(dvs2.size() == 2);
	BOOST_TEST(dvs2.at(0) == "Test");
	BOOST_TEST(dvs2.at(1) == "T2");
	ds32.read<string>(dvs32);
	BOOST_TEST_REQUIRE(dvs32.size() == 6);
	BOOST_TEST(dvs32.at(0) == "Test");
	BOOST_TEST(dvs32.at(1) == "T2");
	BOOST_TEST(dvs32.at(2) == "T3");
	BOOST_TEST(dvs32.at(3) == "B1");
	BOOST_TEST(dvs32.at(4) == "B2");
	BOOST_TEST(dvs32.at(5) == "B3");
}

/*
BOOST_AUTO_TEST_CASE(create_file_structure)
{
	using namespace std;
	HH::File f = Testing::createTestFile();
	BOOST_TEST_REQUIRE(f.isGroup());
	auto g = f.create("g");
	BOOST_TEST_REQUIRE(g.isGroup());
	auto ga = g.atts.add<int16_t>("i16t_1", { 1 });
	auto ga2 = g.atts.add<int16_t>("i16t_2_2", { 1,2,3,4 }, { 2, 2 });
	BOOST_TEST(g.atts.exists("i16t_1"));
	BOOST_TEST(g.atts.exists("i16t_2_2"));

	auto d = g.dsets.create<int>("d", { 2,3 });
	d.write<int>({ 1,2,3,4,5,6 });

	vector<int> data(6);
	d.read<int>(gsl::make_span(data));
	BOOST_TEST(data[4] = 4);

	auto da = d.atts.add<float>("f", { 1.2f });
}
*/

/*
BOOST_AUTO_TEST_CASE(variable_char_string_attributes)
{
	using namespace std;
	HH::File f = Testing::createTestFile();
	//f.atts.add<std::string>("variable_char_string", "This is a test");
	BOOST_TEST(f.atts.exists("variable_char_string"));
}
*/

/*
BOOST_AUTO_TEST_CASE(variable_string_attributes)
{
	using namespace std;
	HH::File f = Testing::createTestFile();
	f.atts.add<std::string>("variable_string", std::string("This is a test"));
	BOOST_TEST(f.atts.exists("variable_string"));
	auto att = f.atts.open("variable_string");
	BOOST_TEST_CHECKPOINT("Var opened");
	//std::string s = att.read<std::string>();
	//BOOST_TEST_CHECKPOINT("Att read as a string");
}
*/
/*
BOOST_AUTO_TEST_CASE(fixed_string_attributes)
{
	using namespace std;
	HH::File f = Testing::createTestFile();
	f.atts.addFixedLengthString<std::string>("fixed_string", "This is a test");
	BOOST_TEST(f.atts.exists("fixed_string"));
	auto att = f.atts.open("fixed_string");
	auto dims = att.getDimensions();
	BOOST_TEST_MESSAGE("Att has dimensionality " << dims.dimensionality
		<< " and length of " << dims.numElements);
	//auto t = att.getType();
	BOOST_TEST_CHECKPOINT("Pre-read");
	vector<char> char_buf(50);
	//att.read<char>(gsl::make_span(char_buf));
	BOOST_TEST_CHECKPOINT("Att read as chars");
	std::string s = att.read<std::string>();
	BOOST_TEST_CHECKPOINT("Att read as a string");
}
*/
BOOST_AUTO_TEST_SUITE_END();
