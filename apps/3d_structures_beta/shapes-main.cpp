/** \brief 3d_structures program - An example program to read and write shape files
 *
 * This program reads shape files (in ADDA or DDSCAT formats) and writes
 * an HDF5/netCDF file as an output. The program demonstrates how to
 * create shapes using the library's C++ interface.
 *
 * This example is multi-threaded. This makes it somewhat complicated.
 * Read operations on ASCII text are slow,
 * as are filesystem calls. This program can read in files at near disk
 * speed. Sadly, HDF5 can only write objects using one thread at a time,
 * and its internal compression filters are also single-threaded.
 *
 * Here's how the program works:
 * 1. Read the user options.
 * 2. Collect a list of all valid shape files for input. Save this list to "myreadstack".
 * 3. Spawn a pool of threads. Each thread in the pool will be dedicated to 
 *		reading text-based shape files.
 * 3a. Each reading thread gets the name of another shape file from "myreadstack", 
 *		reads it, and stores the shape data on "mywritestack".
 * 3b. The main thread (not in the pool) handles shape writing. It runs in a loop, 
 *		waiting for shapes to populate "mywritestack". It checks every 100 milliseconds.
 *		It writes shapes sequentially to the output file.
 * 4. Once there are no shapes to be read or written, execution terminates.
**/

#include <icedb/defs.h>
#include <boost/program_options.hpp>
#include <atomic>
#include <chrono>	// std::chrono::milliseconds
#include <deque>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <set>
#include <stack>
#include <string>
#include <vector>
#include <icedb/shape.hpp>
#include <icedb/Database.hpp>
#include <icedb/fs_backend.hpp>
// TODO: Remove this and either make the convenience function public or
// improve the interface.
// Currently, it is used only to set the compression level of the output data.
#include "../../lib/private/hdf5_supplemental.hpp"
#include "shape.hpp"
#include "shapeIOtext.hpp"



// This program uses a few global objects to keep the threads in sync.

// This is the thread pool
std::vector<std::unique_ptr<std::thread> > pool;
// This keeps track of the number of threads in the reading pool that are working on reading shapes.
std::atomic<int> countCurrentReading;

// The std::mutex objects allow for synchronized access to a few protected variables.
// This helps prevent data races.
std::mutex mStack, mOutStack;

// The mutexes protect the reading and writing "stacks".

// myreadstack is a collection of {filesystem path, shape name} pairs.
std::deque<std::pair<sfs::path, std::string> > myreadstack;
// mywritestack is a collection of {shapes, filesystem path, shape name} tuples.
std::deque<std::tuple<icedb::Examples::Shapes::ShapeDataBasic, sfs::path, std::string> > mywritestack;

// A list of valid shapefile output formats
const std::map<std::string, std::set<sfs::path> > file_formats = {
	{"text", {".dat", ".shp", ".txt", ".shape"} },
	{"icedb", {".hdf5", ".nc", ".h5", ".cdf", ".hdf"} }
};

// These get set in main(int,char**).
float resolution_um = 0; ///< The resolution of each shape lattice, in micrometers.
bool nccompat = true; ///< Are we forcing NetCDF-4 compatability.

icedb::Groups::Group::Group_ptr basegrp; ///< Shapes get written to this location in the output database.

/** \brief This is the primary function in each thread devoted to reading shapes.
* Usually, about three of these threads are active.
*
* This is a simple loop.
* 1. Read from "myreadstack" to get the filename of an input shape.
* 2. Read the shape.
* 3. Put the read shape on "mywritestack".
* 4. Read another shape. If the reading stack (myreadstack) is empty, then stop this thread.
**/
void readtask() {
	// Loop constantly, pulling new shape locations from the stack.
	// If we run out of shapes to read, then the thread terminates.
	// We protect against premature termination using mutex locks - 
	// the main program locks the reading stack until it is populated.
	while (true) {
		decltype(myreadstack)::value_type cur; // The current object being read by this thread.
		{
			// Each thread in the reading stack locks mStack to have exclusive access to "myreadstack".
			// It pops off an element from the reading stack and processes it.
			std::lock_guard<std::mutex> lStack(mStack);
			if (myreadstack.empty()) return;
			cur = myreadstack.front();
			countCurrentReading++; // Keep track of the number of active reads, to prevent premature program termination.
			myreadstack.pop_front();
			// Once we are done with manipulating "myreadstack", then the mutex lock is released and
			// another thread can use it.
		}

		// Read the text file.
		auto data = icedb::Examples::Shapes::readTextFile(cur.first.string());
		// Set a basic particle id. This id is used when writing the shape to the output file.
		// In this example, objects in the output file are named according to their ids.
		data.required.particle_id = cur.first.filename().string();
		data.required.NC4_compat = nccompat;
		if (resolution_um)
			data.optional.particle_scattering_element_spacing = resolution_um / 1.e6f;

		{
			// We enter a new scope to lock the output stack (mOutStack / mywritestack).
			// We lock the output stack because we are appending to it.
			std::lock_guard<std::mutex> lOutStack(mOutStack);
			//std::cout << "." << std::endl;
			mywritestack.push_back(
				std::tuple<icedb::Examples::Shapes::ShapeDataBasic, sfs::path, std::string>
				(std::move(data), cur.first, cur.second));
			countCurrentReading--;
		}
	}
}

/// Convenience function to build a pool of threads for parsing shapes and writing to the hdf5 file
bool construct_thread_pool(int numThreads) {
	if (numThreads > 1) numThreads-= 1;
	for (int i = 0; i < numThreads; ++i)
		pool.push_back(std::make_unique<std::thread>(readtask));
	//pool.resize(numThreads, std::thread(task));
	return true;
}

/** \brief The main body of the program.
*
* The main program:
* 1. Reads the user-provided input options.
* 2. Creates or opens the output database.
* 3. Gathers the list of all valid input files.
* 4. Spawns a pool of threads to read the input files.
* 5. Writes the output shape objects to the output database.
**/
int main(int argc, char** argv) {
	try {
		using namespace std;
		// Read program options
		
		namespace po = boost::program_options;
		po::options_description desc("Allowed options");
		desc.add_options()
			("help,h", "produce help message")
			("from", po::value<string>(), "The path where shapes are read from")
			("to", po::value<string>(), "The path where shapes are written to")
			("db-folder", po::value<string>()->default_value("shapes"), "The path within the database to write to")
			("create", "Create the output database if it does not exist")
			("resolution", po::value<float>(), "Lattice spacing for the shape, in um")
			("compression-level", po::value<int>()->default_value(6), "Compression level (0-9). 0 is no compression, 9 is max compression.")
			("nc4-compat", po::value<bool>()->default_value(true), "Generate a NetCDF4-compatible file")
			("truncate", "Instead of opening existing output files in read-write mode, truncate them.")
			;
		po::variables_map vm;
		po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
		po::notify(vm);

		auto doHelp = [&](const string& s)->void
		{
			cout << s << endl;
			cout << desc << endl;
			exit(1);
		};
		if (vm.count("help")) doHelp("");
		if (!vm.count("from") || !vm.count("to")) doHelp("Need to specify to/from locations.");

		
		using namespace icedb;
		int clev = vm["compression-level"].as<int>();
		nccompat = vm["nc4-compat"].as<bool>();
		Expects(clev >= 0);
		Expects(clev < 10);
		icedb::fs::hdf5::useZLIB(clev);

		// namespace sfs defined for compatability. See <icedb/fs_backend.hpp>
		string sFromRaw = vm["from"].as<string>();
		string sToRaw = vm["to"].as<string>();
		sfs::path pFromRaw(sFromRaw);
		sfs::path pToRaw(sToRaw);
		string dbfolder = vm["db-folder"].as<string>();
		if (vm.count("resolution")) resolution_um = vm["resolution"].as<float>();

		// By default, run as many threads as we can on the platform.
		// TODO: Make this adjustable.
		const int Num_Threads = thread::hardware_concurrency();
		
		// Create the output database if it does not exist
		auto iof = fs::IOopenFlags::READ_WRITE;
		if (vm.count("create")) iof = fs::IOopenFlags::CREATE;
		if (vm.count("truncate")) iof = fs::IOopenFlags::TRUNCATE;
		if (!sfs::exists(pToRaw)) iof = fs::IOopenFlags::CREATE;
		Databases::Database::Database_ptr db = Databases::Database::openDatabase(pToRaw.string(), iof);
		basegrp = db->createGroupStructure(dbfolder);

		// Gather a list of shape files to read.
		auto files = icedb::fs::impl::collectDatasetFiles(pFromRaw, file_formats.at("text"));
		for (const auto &f : files) myreadstack.push_back(f);

		// Create the reading thread pool. Start processing the input shapes.
		construct_thread_pool(Num_Threads);

		// The loop used for writing shapes in the output database.
		while (true) {
			decltype(mywritestack)::value_type cur;
			{
				// Awkward object encapsulation. I want RAII, but I want to
				// explicitly free the lock before sleeping this thread.
				std::unique_ptr<std::lock_guard<std::mutex> > lOutStack
					= make_unique<std::lock_guard<std::mutex>>(mOutStack);
				if (myreadstack.empty() && mywritestack.empty() && !countCurrentReading.load()) {
					// If there is nothing left to read or write, then we can exit the writer loop and
					// end the program.
					break;
				}
				if (mywritestack.empty()) {
					// There is nothing to write (waiting on readers to parse an input shape).
					// Sleep for 100 milliseconds.
					lOutStack.reset();
					std::this_thread::sleep_for(std::chrono::milliseconds(100));
					continue;
				}
				// There is at least one object that is ready to be written to the output file.
				// Get this shape.
				cur = mywritestack.front();
				mywritestack.pop_front();
			}
			{
				// Writing an output shape.

				// Trying to figure out where to put the shape in the database.
				// For this example, single file reads may occasionally occur,
				// and the groupName would just be "/", which is not correct.
				// TODO: Make this clearer.
				std::string groupName = std::get<2>(cur);
				if (groupName == "/") groupName = "";
				const std::string particleId = std::get<0>(cur).required.particle_id;
				if (!groupName.size() && particleId.size()) groupName = particleId;
				if (!groupName.size()) groupName = "shape";

				// Write the shape
				auto sgrp = basegrp->createGroup(groupName);
				auto shp = std::get<0>(cur).toShape(
					std::get<1>(cur).filename().string(), sgrp->getHDF5Group());
			}
		}

		// Ensure that all threads are completed, and re-join them with the main process.
		for (auto &t : pool) t->join(); // These are all completed by the time this line is reached.
	}
	// Ensure that unhandled errors are displayed before the application terminates.
	catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}
	catch (...) {
		std::cerr << "Unknown exception caught." << std::endl;
		return 1;
	}
	return 0;
}
