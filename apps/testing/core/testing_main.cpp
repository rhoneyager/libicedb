// This is the master file that defines the test framework
#include <string>
#include <iostream>
#include <exception>

#define BOOST_TEST_MODULE icedb
#define BOOST_TEST_NO_MAIN
//#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK

#include "globals.h"

#include <boost/test/unit_test.hpp>
//#include <boost/test/included/unit_test.hpp>

BOOST_GLOBAL_FIXTURE(globals);

int BOOST_TEST_CALL_DECL
main(int argc, char* argv[])
{
	try {
		return ::boost::unit_test::unit_test_main(&init_unit_test, argc, argv);
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}
	catch (...)
	{
		std::cerr << "An unhandled exception has occurred." << std::endl;
		return 2;
	}
}

