#include <string>
#include <iostream>
#include <map>
#include <memory>
#include <exception>
#include <tuple>
#define BOOST_TEST_MODULE HH
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "HH/Files.hpp"
#include "HH/Groups.hpp"
#include "HH/Funcs.hpp"
#include "IntegratedTesting.hpp"

BOOST_TEST_GLOBAL_FIXTURE(HH_GlobalTestingFixture);

BOOST_AUTO_TEST_CASE(explode_path)
{
	using namespace std;
	const map<string, vector<string>> Source_And_Expected{
		{"HH", {"HH"}},
		{"HH/Level1", {"HH", "Level1"}},
		{"HH/Level1/Level2", {"HH","Level1","Level2"}},
		{"HH/Level1/Level2/Level3", {"HH","Level1","Level2","Level3"}},
		{"/", {"/"}},
		{"//", {"/"}},
		{"/HH", {"/","HH"}},
		{"/HH/", {"/","HH"}},
		{"HH/", {"HH"}},
		{"/HH/Level1", {"/","HH","Level1"}},
		{"HH//Level1", {"HH","Level1"}},
		{"/HH//Level1", {"/","HH","Level1"}},
		{"//HH////Level1", {"/","HH","Level1"}},
		{"HH/Level1/", {"HH","Level1"}}
	};
	auto expandVector = [](const vector<string> & s) -> string {
		std::string res;
		for (const auto& ss : s) res.append("      ").append(ss);
		return res;
	};

	// Preliminary tests:
	// If these fail, then something is wrong in your C++ standard library.
	string p{ "/" };
	size_t i = p.find('/', 0);
	size_t j = p.find('/', 1);
	size_t k = p.find('/', 2);
	BOOST_TEST_REQUIRE(i == 0);
	BOOST_TEST_REQUIRE(j == string::npos);
	BOOST_TEST_REQUIRE(k == string::npos);

	// Main tests:
	for (const auto& m : Source_And_Expected) {
		BOOST_TEST_MESSAGE("Splitting " << m.first << " into " << expandVector(m.second));
		auto res = HH::splitPaths(m.first);
		BOOST_TEST_MESSAGE("Result: " << expandVector(res));

		BOOST_TEST(res.size() == m.second.size());
		if (res.size() != m.second.size()) {
			BOOST_TEST_MESSAGE("Skipping remaining comparisons for this case, as the sizes do not align.");
			continue;
		}
		for (size_t i = 0; i < res.size(); ++i)
			BOOST_TEST(res[i] == m.second[i]);
	}
}

BOOST_AUTO_TEST_CASE(condense_path)
{
	using namespace std;
	const vector<tuple<string, vector<string>, size_t, size_t>> Source_And_Expected{
		{"HH", {"HH"}, 0, 1},
		{"HH/Level1", {"HH", "Level1"}, 0, 2},
		{"HH/Level1/Level2", {"HH","Level1","Level2"}, 0, 3},
		{"HH/Level1/Level2/Level3", {"HH","Level1","Level2","Level3"}, 0, 4},
		{"/", {"/"}, 0, 1},
		{"/HH", {"/","HH"}, 0, 2},
		{"/HH/Level1", {"/","HH","Level1"}, 0, 3},
		{"HH", {"HH","Level1","Level2","Level3"}, 0, 1},
		{"HH/Level1", {"HH","Level1","Level2","Level3"}, 0, 2},
		{"HH/Level1/Level2", {"HH","Level1","Level2","Level3"}, 0, 3},
		{"Level1/Level2/Level3", {"HH","Level1","Level2","Level3"}, 1, 4},
		{"Level1/Level2", {"HH","Level1","Level2","Level3"}, 1, 3},
		{"Level2/Level3", {"HH","Level1","Level2","Level3"}, 2, 4},
		{"Level1/Level2/Level3", {"/","HH","Level1","Level2","Level3"}, 2, 5},
		{"/HH/Level1/Level2/Level3", {"/","HH","Level1","Level2","Level3"}, 0, 5},
		{"/HH/Level1", {"/","HH","Level1","Level2","Level3"}, 0, 3},
		{"Level1/Level2/Level3", {"/","HH","Level1","Level2","Level3"}, 2, 5}
	};
	auto expandVector = [](const vector<string> & s) -> string {
		std::string res;
		for (const auto& ss : s) res.append("      ").append(ss);
		return res;
	};

	// Main tests:
	for (const auto& m : Source_And_Expected) {
		BOOST_TEST_MESSAGE("Combining " << get<0>(m)
			<< " from " << expandVector(get<1>(m)) 
			<< " on " << get<2>(m) << " to " << get<3>(m));

		auto res = HH::condensePaths(get<1>(m), get<2>(m), get<3>(m));
		BOOST_TEST_MESSAGE("Result: " << res);

		BOOST_TEST(res == get<0>(m));
	}
}
