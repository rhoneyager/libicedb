#include <string>
#include <iostream>
#define BOOST_TEST_DYN_LINK
#include <HH/Handles.hpp>
//#include <HH/Attributes.hpp>
//#include <HH/Datasets.hpp>
//#include <HH/Files.hpp>
//#include <HH/Groups.hpp>
#include <HH/Handles_HDF.hpp>
#include <HH/Handles_Valid.hpp>
#include <HH/Tags.hpp>
#include <HH/Types.hpp>
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(HDFforHumans_File_IO);

using namespace std;
using namespace HH;

// Attempt to open a nonexistent file

// Test creation of a nonexistent file

// Check that an error occurs when trying to create an already-existing file, no truncation mode.

// Test open of a regular file, read-only mode

// Attempt to write to a file marked as read-only

// Create an in-memory only file

// Create a file image that will be written to disk later



BOOST_AUTO_TEST_SUITE_END();

