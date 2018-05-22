#include <string>
#include <iostream>
#define BOOST_TEST_DYN_LINK
#include <HH/Handles.hpp>
//#include <HH/Attributes.hpp>
//#include <HH/Datasets.hpp>
#include <HH/Files.hpp>
//#include <HH/Groups.hpp>
#include <HH/Handles_HDF.hpp>
#include <HH/Handles_Valid.hpp>
#include <HH/Tags.hpp>
#include <HH/Types.hpp>
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(HDFforHumans_File_IO);

using namespace std;
using namespace HH;

// Check that property list classes are not valid HDF5 handles,
// while property lists that are created from them are indeed valid.
BOOST_AUTO_TEST_CASE(hdf5_plistids_valid) {
	hid_t t = H5P_FILE_ACCESS;
	BOOST_CHECK_EQUAL(HH::Handles::InvalidHDF5Handle::isValid(t), false);
	hid_t u = H5Pcreate(H5P_FILE_ACCESS);
	BOOST_CHECK_EQUAL(HH::Handles::InvalidHDF5Handle::isValid(u), true);
	H5Pclose(u);
}

// These test basic handle operations, including scoped moves and swaps.
// If these tests do not pass, then any handle-dependent operations will
// probably fail with odd errors.
BOOST_AUTO_TEST_CASE(hdf5_handle_operations_valid) {
	auto p = HH::Handles::H5P_ScopedHandle{ H5Pcreate(H5P_FILE_ACCESS) };
	bool v = p.valid();
	BOOST_CHECK_EQUAL(v, true);
	auto q = std::move(p);
	bool u = q.valid();
	BOOST_CHECK_EQUAL(u, true);
	HH::Handles::H5P_ScopedHandle r{ 0 };
	r.swap(q);
	bool ra = q.valid();
	BOOST_CHECK_EQUAL(ra, false);
	bool rb = r.valid();
	BOOST_CHECK_EQUAL(rb, true);
}

// Check that validity is preserved across operations involving weak and strong handles.
BOOST_AUTO_TEST_CASE(hdf5_handle_operations_valid_2) {
	auto p = HH::Handles::H5P_ScopedHandle{ H5Pcreate(H5P_FILE_ACCESS) };
	BOOST_CHECK_EQUAL(p.valid(), true);

	// By current design, this code block would cause a double close!
	// not_invalid would have to clone p. It cannot currently accept by a move constructor.
	// Currently, there is an odd bug preventing this.
	//HH::Handles::not_invalid<HH::Handles::HH_hid_t> q{ std::move(p) };
	//auto qval = q.get(); // On std::move, the destructor gets triggered.
	//BOOST_CHECK_EQUAL(qval.valid(), true);

	// Instead, provide a weak handle to not_invalid. We do not want to pass ownership.
	auto r = HH::Handles::H5P_ScopedHandle{ H5Pcreate(H5P_FILE_ACCESS) };
	HH::Handles::not_invalid<HH::Handles::HH_hid_t> s{ r.getWeakHandle() };
	auto sval = s.get(); // On std::move, the destructor gets triggered.
	BOOST_CHECK_EQUAL(sval.valid(), true);
}

void io_ni_move_2(HH::Handles::not_invalid<HH::Handles::HH_hid_t> ImageCreationPlist)
{
	hid_t h = ImageCreationPlist.get().h; // Separated from next line for easier debugging.
	const auto h5Result = H5Pset_fapl_core(h, 10000, false);
	Expects(h5Result >= 0 && "H5Pset_fapl_core failed");
}
// Test not_invalid upon move
BOOST_AUTO_TEST_CASE(io_ni_move) {
	auto p = HH::Handles::H5P_ScopedHandle{ H5Pcreate(H5P_FILE_ACCESS) };
	io_ni_move_2(p.h);
	HH::Handles::not_invalid<HH::Handles::HH_hid_t> q{ p.getWeakHandle() };
	io_ni_move_2(q.get().h);
	HH::Handles::not_invalid<HH::Handles::HH_hid_t> r{ q };
	io_ni_move_2(r.get().h);
	auto s = HH::Handles::H5P_ScopedHandle{ H5Pcreate(H5P_FILE_ACCESS) };
	io_ni_move_2(s.getWeakHandle());
	BOOST_CHECK_EQUAL(1, 1); // This test really 'succeeds' if the code executes without aborting the program.
}

// Create an in-memory-only file
BOOST_AUTO_TEST_CASE(io_inmemfile) {
	auto pl = HH::Handles::H5P_ScopedHandle{ H5Pcreate(H5P_FILE_ACCESS) };
	auto f = HH::Files::create_file_image(
		"testing_hh_file_io_inmemfile",
		10000000,
		false,
		pl.getWeakHandle());

	BOOST_CHECK_EQUAL(f.valid(), true);
}


// Attempt to open a nonexistent file

// Test creation of a nonexistent file

// Check that an error occurs when trying to create an already-existing file, no truncation mode.

// Test open of a regular file, read-only mode

// Attempt to write to a file marked as read-only

// Create an in-memory only file

// Create a file image that will be written to disk later



BOOST_AUTO_TEST_SUITE_END();

