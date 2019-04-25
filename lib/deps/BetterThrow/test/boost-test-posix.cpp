#include <string>
#include <iostream>
#include <memory>
#include <exception>
#define BOOST_TEST_MODULE BT_WINAPI
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "Testing.hpp"
#include "BetterThrow/Info.hpp"
#include "BetterThrow/Error.hpp"
#include "../src/os-posix.hpp"

BOOST_TEST_GLOBAL_FIXTURE(BT_GlobalTestingFixture);

#if (BOOST_VERSION / 100 % 1000) < 59
#define BOOST_TEST BOOST_CHECK
#define BOOST_TEST_REQUIRE BOOST_REQUIRE
#endif

using namespace BT::posix;
using BT::convertStr;

BOOST_AUTO_TEST_CASE(posix_getCWD) {
	BT::native_path_string_t s;
	auto res = getCWD(s);
	BOOST_TEST_REQUIRE(res.first == 0);
	BOOST_TEST_MESSAGE("CWD is " << convertStr(s));
	BOOST_TEST_REQUIRE(s.size() != 0);
}

BOOST_AUTO_TEST_CASE(posix_getEnvironment) {
	std::map<::BT::native_path_string_t, ::BT::native_path_string_t> envs;
	BOOST_TEST_MESSAGE("Environment:");
	auto res = getEnvironment(envs);
	BOOST_TEST_REQUIRE(res.first == 0);
	for (const auto& e : envs)
		BOOST_TEST_MESSAGE("\t\t" << convertStr(e.first) << "\t" << convertStr(e.second));
}

BOOST_AUTO_TEST_CASE(posix_getCmdLine) {
	std::vector<::BT::native_path_string_t> cmdline;
	BOOST_TEST_MESSAGE("Command line:");
	BT::Error_Res_t err = getCmdLine(cmdline);
	BOOST_TEST_REQUIRE(err.first == 0);
	BOOST_TEST_MESSAGE("Num params: " << cmdline.size());
	for (const auto& c : cmdline)
		BOOST_TEST_MESSAGE("\t" << convertStr(c));
}

BOOST_AUTO_TEST_CASE(posix_getElevated) {
	bool res = false;
	auto err = getElevated(res);
	BOOST_TEST_REQUIRE(err.first == 0);
	BOOST_TEST_MESSAGE("Elevated: " << res);
}

void test_getModulePath() {}

BOOST_AUTO_TEST_CASE(posix_getModulePath) {
	::BT::native_path_string_t modPath;
	BT::Error_Res_t err = getModulePath((void*)test_getModulePath, modPath);
	BOOST_TEST_REQUIRE(err.first == 0);
	BOOST_TEST_MESSAGE("modPath: " << convertStr(modPath));
}

BOOST_AUTO_TEST_CASE(posix_getLoadedModules) {
	BT::ModuleInfo_t<> mi;
	BT::Error_Res_t err = getLoadedModules(mi);
	BOOST_TEST_REQUIRE(err.first == 0);

	BOOST_TEST_MESSAGE("Modules: ");
	for (const auto& c : mi)
		BOOST_TEST_MESSAGE("\t" << convertStr(c));
}

BOOST_AUTO_TEST_CASE(posix_getUserName) {
	::BT::native_path_string_t res;
	BT::Error_Res_t err = getUserName(res);
	BOOST_TEST_REQUIRE(err.first == 0);
	BOOST_TEST_MESSAGE("User name: " << convertStr(res));
}

BOOST_AUTO_TEST_CASE(posix_getComputerName) {
	::BT::native_path_string_t res;
	BT::Error_Res_t err = getComputerName(res);
	BOOST_TEST_REQUIRE(err.first == 0);
	BOOST_TEST_MESSAGE("Computer name: " << convertStr(res));
}

BOOST_AUTO_TEST_CASE(posix_getAppConfigDir) {
	::BT::native_path_string_t res;
	BT::Error_Res_t err = getAppConfigDir(res);
	BOOST_TEST_REQUIRE(err.first == 0);
	BOOST_TEST_MESSAGE("App config dir: " << convertStr(res));
}

BOOST_AUTO_TEST_CASE(posix_getHomeDir) {
	::BT::native_path_string_t res;
	BT::Error_Res_t err = getHomeDir(res);
	BOOST_TEST_REQUIRE(err.first == 0);
	BOOST_TEST_MESSAGE("Home dir: " << convertStr(res));
}

BOOST_AUTO_TEST_CASE(posix_getStartTime) {
	::BT::native_path_string_t res;
	BT::Error_Res_t err = getStartTime(BT::getPID(), res);
	BOOST_TEST_REQUIRE(err.first == 0);
	BOOST_TEST_MESSAGE("Start time: " << convertStr(res));
}
