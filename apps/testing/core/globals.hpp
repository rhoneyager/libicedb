#pragma once

/** This defines a global test fixture that locates the test directory paths. 
* Many of the tests require I/O, but the location when testing is 
* not necessarily the same as the install location.
**/
#include <memory>

struct globals
{
public:
	globals();
	~globals();
	static globals*& instance();
	//void findDirs();
	//void loadConfig();
};

