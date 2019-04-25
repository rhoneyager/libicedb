#include <iostream>

#include "../include/BetterThrow/Error.hpp"
#include "../include/BetterThrow/Info.hpp"
#include "../src/os-posix.hpp"

using namespace BT::posix;

int test_getCWD() {
	BT::native_path_string_t s;
	getCWD(s);
	BT::native_cout << BT_NATIVE("\tCWD is ") << s << std::endl;
	return 0;
}

int test_getEnvironment() {
	std::map<::BT::native_path_string_t, ::BT::native_path_string_t> envs;
	BT::native_cout << BT_NATIVE("\tEnvironment: ") << std::endl;
	getEnvironment(envs);
	for (const auto& e : envs)
		BT::native_cout << BT_NATIVE("\t\t")
		<< e.first << BT_NATIVE("\t") << e.second << std::endl;
	return 0;
}

int test_getCmdLine() {
	std::vector<::BT::native_path_string_t> cmdline;
	BT::native_cout << BT_NATIVE("\tCommand line: ") << std::endl;
	BT::Error_Res_t err;
	err = getCmdLine(cmdline);
	if (err.first) throw;
	BT::native_cout << BT_NATIVE("\t\tNum params: ") << cmdline.size() << std::endl;
	for (const auto& c : cmdline)
		BT::native_cout << BT_NATIVE("\t\t\t") << c << std::endl;
	return 0;
}

int test_getElevated() {
	bool res = false;
	getElevated(res);
	BT::native_cout << BT_NATIVE("\tElevated: ") << res << std::endl;
	return 0;
}

int test_getModulePath() {
	::BT::native_path_string_t modPath;
	BT::Error_Res_t err;
	err = getModulePath((void*)test_getModulePath, modPath);
	//std::cerr << err.first << "\t" << err.second << std::endl;
	BT::native_cout << BT_NATIVE("\tmodPath: ") << modPath << std::endl;
	return 0;
}

int test_getLoadedModules() {
	BT::ModuleInfo_t mi;
	getLoadedModules(mi);

	BT::native_cout << BT_NATIVE("\tModules: ") << std::endl;
	for (const auto& c : mi)
		BT::native_cout << BT_NATIVE("\t\t")
		<< c << std::endl;

	return 0;
}

int test_getUserName() {
	::BT::native_path_string_t res;
	getUserName(res);
	BT::native_cout << BT_NATIVE("\tUser name: ") << res << std::endl;
	return 0;
}

int test_getComputerName() {
	::BT::native_path_string_t res;
	getComputerName(res);
	BT::native_cout << BT_NATIVE("\tComputer name: ") << res << std::endl;
	return 0;
}

int test_getAppConfigDir() {
	::BT::native_path_string_t res;
	getAppConfigDir(res);
	BT::native_cout << BT_NATIVE("\tApp config dir: ") << res << std::endl;
	return 0;
}

int test_getHomeDir() {
	::BT::native_path_string_t res;
	getHomeDir(res);
	BT::native_cout << BT_NATIVE("\tHome dir: ") << res << std::endl;
	return 0;
}

int test_getStartTime() {
	::BT::native_path_string_t res;
	getStartTime(BT::getPID(), res);
	BT::native_cout << BT_NATIVE("\tStart time: ") << res << std::endl;
	return 0;
}


int doTry(int &errvar, int &numTests, int (*f)())
{
	numTests++;
	std::cout << "Test " << numTests << std::endl;
	int res = 0;
	try {
		res = f();
		errvar += res;
	}
	catch (std::exception & f) {
		BT::print_exception(f);
		errvar++;
		res = 1;
	}
	return res;
}

int main(int, char**) {
	int numErrors = 0;
	int numTests = 0;
	doTry(numErrors, numTests, test_getCWD);
	doTry(numErrors, numTests, test_getEnvironment);
	doTry(numErrors, numTests, test_getCmdLine);
	doTry(numErrors, numTests, test_getElevated);
	doTry(numErrors, numTests, test_getModulePath);
	doTry(numErrors, numTests, test_getLoadedModules);
	doTry(numErrors, numTests, test_getUserName);
	doTry(numErrors, numTests, test_getComputerName);
	doTry(numErrors, numTests, test_getAppConfigDir);
	doTry(numErrors, numTests, test_getHomeDir);
	doTry(numErrors, numTests, test_getStartTime);

	std::cout << "There were " << numTests
		<< ((numTests == 1) ? " test" : " tests") << ", with "
		<< numErrors << ((numErrors == 1) ? " error" : " errors") << "." << std::endl;
	return numErrors;
}
