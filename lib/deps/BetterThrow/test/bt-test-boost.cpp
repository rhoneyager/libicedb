#include <string>
#include <iostream>
#include <memory>
#include <exception>
#define BOOST_TEST_MODULE BT
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "Testing.hpp"
#include "BetterThrow/Info.hpp"

BOOST_TEST_GLOBAL_FIXTURE(BT_GlobalTestingFixture);

#if (BOOST_VERSION / 100 % 1000) < 59
#define BOOST_TEST BOOST_CHECK
#define BOOST_TEST_REQUIRE BOOST_REQUIRE
#endif

BOOST_AUTO_TEST_CASE(getOSfamilyName)
{
	using namespace std;
	const char* os = BT::getOSfamilyName();
	BOOST_TEST_MESSAGE("Returned OS name is " << os << ".");
	std::string sos(os);

	BOOST_TEST_REQUIRE(sos != "UNKNOWN");
}

BOOST_AUTO_TEST_CASE(convertStr)
{
	using namespace std;
	std::string res = BT::convertStr(L"test");
	BOOST_TEST_REQUIRE(res == "test");
	std::string resb = BT::convertStr("test");
	BOOST_TEST_REQUIRE(resb == "test");
}

BOOST_AUTO_TEST_CASE(getOSerror)
{
	using namespace std;
	auto err = BT::getOSerror();
	BOOST_TEST_MESSAGE("Returned OS error " << err.first << ".");
	BOOST_TEST_MESSAGE("Returned OS error " << err.second << ".");
	BOOST_TEST_REQUIRE(err.first == 0);
}

BOOST_AUTO_TEST_CASE(getOSerrno)
{
	using namespace std;
	auto err = BT::getOSerrno(-1);
	BOOST_TEST_MESSAGE("Returned OS errorno " << err.first << ".");
	BOOST_TEST_MESSAGE("Returned OS errorno " << err.second << ".");
	BOOST_TEST_REQUIRE(err.first == 0);
}

BOOST_AUTO_TEST_CASE(getPID)
{
	using namespace std;
	int pid = BT::getPID();
	BOOST_TEST_MESSAGE("Returned PID is " << pid << ".");

	BOOST_TEST_REQUIRE(pid >= 0);
}

BOOST_AUTO_TEST_CASE(getPPID)
{
	using namespace std;
	int ppid = BT::getPPID();
	BOOST_TEST_MESSAGE("Returned PPID is " << ppid << ".");

	BOOST_TEST_REQUIRE(ppid >= 0);
}

BOOST_AUTO_TEST_CASE(pidExists)
{
	using namespace std;
	auto res = BT::pidExists(BT::getPID());
	BOOST_TEST_REQUIRE(res.success == true);
	BOOST_TEST_REQUIRE(res.value == true);
}

BOOST_AUTO_TEST_CASE(getModule)
{
	using namespace std;
	auto res = BT::getModule<std::string>();
	BOOST_TEST_REQUIRE(res.size() > 0);
	BOOST_TEST_MESSAGE("Loaded module " << res << ".");
}

BOOST_AUTO_TEST_CASE(getLoadedModules)
{
	using namespace std;
	auto res = BT::getLoadedModules<std::string>();
	BOOST_TEST_REQUIRE(res.size() > 0);
	for (const auto &r : res)
		BOOST_TEST_MESSAGE("Loaded module " << r << ".");
}

BOOST_AUTO_TEST_CASE(getProcessInfo_cur)
{
	using namespace std;
	auto res = BT::ProcessInfo<std::string>::get<std::string>();
	BOOST_TEST_MESSAGE("Name: " << res.name << ".");
	BOOST_TEST_MESSAGE("Path: " << res.path << ".");
	BOOST_TEST_MESSAGE("CWD: " << res.cwd << ".");
	BOOST_TEST_MESSAGE("Start time: " << res.startTime << ".");
	BOOST_TEST_MESSAGE("PID: " << res.pid << ".");
	BOOST_TEST_MESSAGE("PPID: " << res.ppid << ".");
	BOOST_TEST_MESSAGE("Is elevated: " << res.isElevated << ".");

	BOOST_TEST_MESSAGE("Command line:");
	for (const auto &c : res.cmdline)
		BOOST_TEST_MESSAGE("\t" << c);

	BOOST_TEST_MESSAGE("Environment:");
	for (const auto& c : res.environment)
		BOOST_TEST_MESSAGE("\t" << c.first << "\t\t" << c.second);

	BOOST_TEST_REQUIRE(res.name.size() > 0);
	BOOST_TEST_REQUIRE(res.path.size() > 0);
	BOOST_TEST_REQUIRE(res.cwd.size() > 0);
	BOOST_TEST_REQUIRE(res.startTime.size() > 0);
	BOOST_TEST_REQUIRE(res.pid >= 0);
	BOOST_TEST_REQUIRE(res.ppid >= 0);

	BOOST_TEST_REQUIRE(res.cmdline.size() > 0);
	BOOST_TEST_REQUIRE(res.environment.size() > 0);
}

BOOST_AUTO_TEST_CASE(getProcessInfo_parent)
{
	using namespace std;
	auto res = BT::ProcessInfo<std::string>::get<std::string>(
		BT::getPPID(BT::getPID()));
	BOOST_TEST_MESSAGE("Name: " << res.name << ".");
	BOOST_TEST_MESSAGE("Path: " << res.path << ".");
	BOOST_TEST_MESSAGE("CWD: " << res.cwd << ".");
	BOOST_TEST_MESSAGE("Start time: " << res.startTime << ".");
	BOOST_TEST_MESSAGE("PID: " << res.pid << ".");
	BOOST_TEST_MESSAGE("PPID: " << res.ppid << ".");
	BOOST_TEST_MESSAGE("Is elevated: " << res.isElevated << ".");

	BOOST_TEST_MESSAGE("Command line:");
	for (const auto& c : res.cmdline)
		BOOST_TEST_MESSAGE("\t" << c);

	BOOST_TEST_MESSAGE("Environment:");
	for (const auto& c : res.environment)
		BOOST_TEST_MESSAGE("\t" << c.first << "\t\t" << c.second);

	BOOST_TEST_REQUIRE(res.name.size() > 0);
	BOOST_TEST_REQUIRE(res.path.size() > 0);
	BOOST_TEST_REQUIRE(res.pid >= 0);
	BOOST_TEST_REQUIRE(res.ppid >= 0);

	// The OS probably does not support these.
	//BOOST_TEST_REQUIRE(res.cwd.size() > 0);
	//BOOST_TEST_REQUIRE(res.startTime.size() > 0);
	//BOOST_TEST_REQUIRE(res.cmdline.size() > 0);
	//BOOST_TEST_REQUIRE(res.environment.size() > 0);
}

BOOST_AUTO_TEST_CASE(getProcessInfo_streams)
{
	using namespace std;
	std::ostringstream sOut;
#if defined(BT_OS_WINDOWS)
	std::wostringstream sOsOut;
#elif defined(BT_OS_LINUX) || defined(BT_OS_UNIX) || defined(BT_OS_MACOS)
	std::ostringstream sOsOut;
#endif
	auto s = BT::ProcessInfo<std::string>::get<std::string>();
	auto os = BT::ProcessInfo<BT::native_path_string_t>::get<BT::native_path_string_t>();

	sOut << s;
	sOsOut << os;


	BOOST_TEST_MESSAGE("std::string stream output\n" << sOut.str());
	std::string ssOs = BT::convertStr(sOsOut.str());
	BOOST_TEST_MESSAGE("std::wstring stream output\n" << ssOs);
	BOOST_TEST_REQUIRE(ssOs.size() > 0);
}

BOOST_AUTO_TEST_CASE(getRuntimeInfo)
{
	using namespace std;
	auto res = BT::RuntimeInfo<std::string>::get<std::string>();
	BOOST_TEST_MESSAGE("username: " << res.username << ".");
	BOOST_TEST_MESSAGE("computername: " << res.computername << ".");
	BOOST_TEST_MESSAGE("homedir: " << res.homedir << ".");
	BOOST_TEST_MESSAGE("appconfigdir: " << res.appconfigdir << ".");
	BOOST_TEST_REQUIRE(res.username.size() > 0);
	BOOST_TEST_REQUIRE(res.computername.size() > 0);
	BOOST_TEST_REQUIRE(res.homedir.size() > 0);
	BOOST_TEST_REQUIRE(res.appconfigdir.size() > 0);
}

BOOST_AUTO_TEST_CASE(getRuntimeInfo_streams)
{
	using namespace std;
	std::ostringstream sOut;
#if defined(BT_OS_WINDOWS)
	std::wostringstream sOsOut;
#elif defined(BT_OS_LINUX) || defined(BT_OS_UNIX) || defined(BT_OS_MACOS)
	std::ostringstream sOsOut;
#endif
	auto s = BT::RuntimeInfo<std::string>::get<std::string>();
	auto os = BT::RuntimeInfo<BT::native_path_string_t>::get<BT::native_path_string_t>();

	sOut << s;
	sOsOut << os;


	BOOST_TEST_MESSAGE("std::string stream output\n" << sOut.str());
	std::string ssOs = BT::convertStr(sOsOut.str());
	BOOST_TEST_MESSAGE("std::wstring stream output\n" << ssOs);
	BOOST_TEST_REQUIRE(ssOs.size() > 0);
}
