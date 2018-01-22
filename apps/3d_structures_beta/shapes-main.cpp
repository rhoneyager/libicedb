/** 3d_structures program - A program to read and write shape files
**/
// Boost bug with C++17 requires this define. See https://stackoverflow.com/questions/41972522/c2143-c2518-when-trying-to-compile-project-using-boost-multiprecision
#define _HAS_AUTO_PTR_ETC 1
#include <boost/program_options.hpp>
#include <atomic>
#include <chrono>       // std::chrono::seconds
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
#include "../../private/hdf5_supplemental.hpp"
#include "shape.hpp"
#include "shapeIOtext.hpp"

/// A list of valid shapefile output formats
const std::map<std::string, std::set<sfs::path> > file_formats = {
	{"text", {".dat", ".shp", ".txt", ".shape"} },
	{"icedb", {".hdf5", ".nc", ".h5", ".cdf", ".hdf"} }
};

//std::mutex mHDF5; ///< Allows only one thread to write to the HDF5 file at a time.
std::mutex mStack, mOutStack; ///< Allows synchronized access to the job stack
std::deque<std::pair<sfs::path, std::string> > myreadstack;
std::deque<std::tuple<icedb::Examples::Shapes::ShapeDataBasic, sfs::path, std::string> > mywritestack;
std::vector<std::unique_ptr<std::thread> > pool;
std::atomic<int> countCurrentReading;
// These get set in main(int,char**).
float resolution_um = 0;
bool nccompat = true;
icedb::Groups::Group::Group_ptr basegrp;

void readtask() {
	// Fetch from the pool
	while (true) {
		decltype(myreadstack)::value_type cur;
		{
			std::lock_guard<std::mutex> lStack(mStack);
			if (myreadstack.empty()) return;
			cur = myreadstack.front();
			countCurrentReading++;
			myreadstack.pop_front();
		}
		auto data = icedb::Examples::Shapes::readTextFile(cur.first.string());
		data.required.particle_id = cur.first.filename().string();
		data.required.NC4_compat = nccompat;
		if (resolution_um)
			data.optional.particle_scattering_element_spacing = resolution_um / 1.e6f;

		{
			std::lock_guard<std::mutex> lOutStack(mOutStack);
			//std::cout << "." << std::endl;
			mywritestack.push_back(
				std::tuple<icedb::Examples::Shapes::ShapeDataBasic, sfs::path, std::string>
				(std::move(data), cur.first, cur.second));
			countCurrentReading--;
		}
	}
}

/// Build a pool of threads for parsing shapes and writing to the hdf5 file
bool construct_thread_pool(int numThreads) {
	if (numThreads > 1) numThreads-= 1;
	for (int i = 0; i < numThreads; ++i)
		pool.push_back(std::make_unique<std::thread>(readtask));
	//pool.resize(numThreads, std::thread(task));
	return true;
}

int main(int argc, char** argv) {
	try {
		using namespace std;
		mStack.lock();
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
		const int Num_Threads = thread::hardware_concurrency();
		


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

		// Create the output database if it does not exist
		auto iof = fs::IOopenFlags::READ_WRITE;
		if (vm.count("create")) iof = fs::IOopenFlags::CREATE;
		if (vm.count("truncate")) iof = fs::IOopenFlags::TRUNCATE;
		if (!sfs::exists(pToRaw)) iof = fs::IOopenFlags::CREATE;
		Databases::Database::Database_ptr db = Databases::Database::openDatabase(pToRaw.string(), iof);
		basegrp = db->createGroupStructure(dbfolder);
		auto files = icedb::fs::impl::collectDatasetFiles(pFromRaw, file_formats.at("text"));
		//std::future<bool> make_threads = std::async(
		//	construct_thread_pool, Num_Threads);
		//bool threadPoolCreateRes = make_threads.get();
		//Expects(threadPoolCreateRes == true);
		
		for (const auto &f : files) myreadstack.push_back(f);
		construct_thread_pool(Num_Threads);
		//std::cout << "Starting the i/o run." << std::endl;
		mStack.unlock();

		while (true) {
			decltype(mywritestack)::value_type cur;
			{
				// Awkward object encapsulation. I want RAII, but I want to
				// explicitly free the lock before sleeping this thread.
				std::unique_ptr<std::lock_guard<std::mutex> > lOutStack = make_unique<std::lock_guard<std::mutex>>(mOutStack);
				// There is a bit of a bug here where some shaped can be reading, but not yet written...
				if (myreadstack.empty() && mywritestack.empty() && !countCurrentReading.load()) {
					//std::cout << "Writer finished and closing." << std::endl;
					break;
				}
				if (mywritestack.empty()) {
					lOutStack.reset();
					std::this_thread::sleep_for(std::chrono::milliseconds(100));
					//std::cout << "Writer sleeping." << std::endl;
					continue;
				}
				cur = mywritestack.front();
				mywritestack.pop_front();
			}
			{
				//std::cout << "Writing " << std::get<2>(cur) << std::endl;

				// Trying to figure out where to put he read shape in the HDF5 file.
				// For this example, single file reads may occasionally occur,
				// and the groupName would just be "/", which is not correct.
				std::string groupName = std::get<2>(cur);
				if (groupName == "/") groupName = "";
				const std::string particleId = std::get<0>(cur).required.particle_id;
				if (!groupName.size() && particleId.size()) groupName = particleId;
				if (!groupName.size()) groupName = "shape";
				auto sgrp = basegrp->createGroup(groupName);
				auto shp = std::get<0>(cur).toShape(
					std::get<1>(cur).filename().string(), sgrp->getHDF5Group());
			}
		}

		for (auto &t : pool) t->join(); // These are all completed by the time this line is reached.
	}
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
