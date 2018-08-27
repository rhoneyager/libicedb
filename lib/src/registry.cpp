/**
* \brief Contains registry functions for extending functionality through DLLs.
*
* Contains both general and OS-specific functions.
**/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <mutex>
#include <boost/filesystem.hpp>
#include <boost/version.hpp>
#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/tokenizer.hpp>
//#include "../Ryan_Debug/logging.h"
//#include "../Ryan_Debug/debug.h"
#include "../icedb/error.hpp"
//#include "../Ryan_Debug/fs.h"
//#include "../Ryan_Debug/config.h"
#include "../icedb/splitSet.hpp"
#include "../icedb/registry.hpp"
#include "../icedb/io.hpp"
#include "../icedb/versioning/versioning.hpp"
#include <boost/iostreams/filter/newline.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/copy.hpp>

namespace icedb
{
	namespace registry
	{
		
		handler_external::handler_external(const char* id) : id(id) {}
		//IO_options::IO_options() {}
		//IO_options::~IO_options() {}

		IOhandler::IOhandler(const char* id) : handler_external(id) {}

		bool match_file_type(const char* filename, const char* type, 
			const char* ext, const char *op, const char *opref)
		{
			using namespace boost::filesystem;
			using std::string;
			using std::ofstream;

			// Checking to see if a given export operation is supported.
			// For regular file saving, this falls through.
			string sop(op), sopref(opref);
			if (sop.compare(sopref) != 0) return false;

			// Actually comparing the type and file extension.
			string sext(ext);
			string sext2(".");
			sext2.append(sext);
			string stype(type);
			path pPrefix(filename);
			if (stype.compare(sext) == 0 || stype.compare(sext2) == 0) return true;

			else if (pPrefix.extension().string().compare(sext2) == 0) return true;
			return false;
		}

		bool match_file_type_multi(std::shared_ptr<icedb::registry::IOhandler> h,
			const char* pluginid,
			std::shared_ptr<IO_options> opts,
			std::shared_ptr<IO_options> opts2)
		//bool match_file_type_multi(const char* filename, const char* type, 
		//	std::shared_ptr<Ryan_Debug::registry::IOhandler> h, const char* pluginid, const char* ext)
		{
			std::string spluginid(pluginid);
			if (h)
			{
				if (h->getId() != spluginid) return false;
				return true;
			} else {
				std::string filename = opts->filename();
				std::string ext = opts2->extension();
				std::string type = opts->filetype();
				return match_file_type(filename.c_str(), type.c_str(), ext.c_str(), 
					opts->exportType().c_str(), opts2->exportType().c_str());
			}
		}

std::istream& operator>>(std::istream& in, ::icedb::registry::IOhandler::IOtype& val)
{
	using namespace icedb::registry;
	char data[50];
	in >> data;
	//in.getline(data,48);
	std::string v(data);
	if ("READONLY" == v) val = IOhandler::IOtype::READONLY;
	else if ("READWRITE" == v) val = IOhandler::IOtype::READWRITE;
	else if ("EXCLUSIVE" == v) val = IOhandler::IOtype::EXCLUSIVE;
	else if ("TRUNCATE" == v) val = IOhandler::IOtype::TRUNCATE;
	else if ("DEBUG" == v) val = IOhandler::IOtype::DEBUG;
	else if ("CREATE" == v) val = IOhandler::IOtype::CREATE;
	else ICEDB_throw(::icedb::error::error_types::xBadInput)
		.add<std::string>("Reason","Unlisted IOtype value")
		.add<std::string>("IO_type",v);
	return in;
}

std::ostream& operator<<(std::ostream &out, const ::icedb::registry::IOhandler::IOtype& val)
{
	using namespace icedb::registry;
	std::string v;
	if (val == IOhandler::IOtype::READONLY) v = "READONLY";
	else if (val == IOhandler::IOtype::READWRITE) v = "READWRITE";
	else if (val == IOhandler::IOtype::EXCLUSIVE) v = "EXCLUSIVE";
	else if (val == IOhandler::IOtype::TRUNCATE) v = "TRUNCATE";
	else if (val == IOhandler::IOtype::DEBUG) v = "DEBUG";
	else if (val == IOhandler::IOtype::CREATE) v = "CREATE";
	else ICEDB_throw(::icedb::error::error_types::xBadInput)
		.add<std::string>("Reason", "Unlisted IOtype value")
		.add<std::string>("IO_type", v);
	out << v;
	return out;
}

std::ostream& operator<<(std::ostream &out, const ::icedb::registry::options& val)
{
	val.enumVals(out);
	return out;
}

} 
	namespace io {
		std::mutex mlock_implementsIO;
		std::mutex& getLock()
		{
			return mlock_implementsIO;
		}
		std::set<std::string> mtypes;
		static std::mutex mlock, mlock_serialization_handle;
		const char* hid = "io_implementsSerialization";

		namespace TextFiles
		{
			class hSerialization
			{
				friend struct serialization_handle;
			public:
				~hSerialization() {}
			private:
				hSerialization() {}
				/// For loading file
				std::unique_ptr<std::ifstream> fi;
				/// For writing file
				std::unique_ptr<std::ofstream> fo;
				/// Boost stream to load file, applying compression filters
				std::shared_ptr<boost::iostreams::filtering_istream> bfi;
				/// Boost stream to write file, applying compression filters
				std::shared_ptr<boost::iostreams::filtering_ostream> bfo;
			};


			bool serialization_handle::compressionEnabled()
			{
				return false;
			}

			const char* serialization_handle::getSHid()
			{
				return hid; // constant object at namespace scope
			}

			serialization_handle::serialization_handle(const char* filename,
				::icedb::registry::IOhandler::IOtype t) :
				IOhandler(hid), h(new hSerialization)
				// hid is a constant object at namespace scope
			{
				open(filename, t);
			}

			serialization_handle::~serialization_handle()
			{
			}

			void serialization_handle::open(const char* filename, ::icedb::registry::IOhandler::IOtype t)
			{
				using namespace boost::filesystem;
				switch (t)
				{
				case IOtype::READONLY:
				{
					if (!exists(path(filename))) ICEDB_throw(icedb::error::error_types::xMissingFile)
						.add("file_name",filename);
					load(filename);
				}
				break;
				case IOtype::CREATE:
					if (exists(path(filename))) ICEDB_throw(icedb::error::error_types::xFileExists)
						.add("file_name", filename);
				case IOtype::TRUNCATE:
					create(filename);
					break;
				case IOtype::EXCLUSIVE:
				case IOtype::DEBUG:
				case IOtype::READWRITE:
					ICEDB_throw(icedb::error::error_types::xUnsupportedIOaction)
						.add("Reason", "IO mode READWRITE "
							"is currently unsupported in serialization code.");
					break;
				}
			}

			void serialization_handle::load(const char* fname)
			{
				// Check file existence
				using namespace std;
				using namespace boost::filesystem;
				//using namespace serialization;
				std::string cmeth, target, uncompressed, filename(fname);
				// Combination of detection of compressed file, file type and existence.
				//if (!detect_compressed(filename, cmeth, target))
				//	RDthrow(Ryan_Debug::error::xMissingFile())
				//	<< Ryan_Debug::error::file_name(filename);
				//uncompressed_name(target, uncompressed, cmeth);

				boost::filesystem::path p = filename;// (uncompressed);
				boost::filesystem::path pext = p.extension(); // Uncompressed extension

				h->fi = std::unique_ptr<std::ifstream>
					(new std::ifstream(fname, std::ios_base::binary | std::ios_base::in));
				// Construct an filtering_iostream that matches the type of compression used.
				using namespace boost::iostreams;

				h->bfi = (std::shared_ptr<filtering_istream>
					(new filtering_istream));
				//if (cmeth.size())
				//	prep_decompression(cmeth, *(h->bfi.get()));
				h->bfi->push(boost::iostreams::newline_filter(boost::iostreams::newline::posix));
				h->bfi->push(*(h->fi.get()));

				reader = h->bfi;
			}

			void serialization_handle::create(const char* fname)
			{
				//using namespace serialization;
				std::string cmeth, uncompressed, filename(fname);
				//uncompressed_name(filename, uncompressed, cmeth);
				boost::filesystem::path p(filename);// (uncompressed);
				boost::filesystem::path pext = p.extension(); // Uncompressed extension


				h->fo = (std::unique_ptr<std::ofstream>
					(new std::ofstream(fname, std::ios_base::trunc | std::ios_base::binary | std::ios_base::out)));
				// Consutuct an filtering_iostream that matches the type of compression used.
				using namespace boost::iostreams;

				h->bfo = (std::shared_ptr<filtering_ostream>
					(new filtering_ostream));
				//if (cmeth.size())
				//	prep_compression(cmeth, *(h->bfo.get()));
				h->bfo->push(boost::iostreams::newline_filter(boost::iostreams::newline::posix));
				h->bfo->push(*(h->fo.get()));

				writer = h->bfo;
			}

			const std::set<std::string>& serialization_handle::known_formats()
			{
				// Moved to hidden file scope to avoid race condition
				//static std::set<std::string> mtypes;
				//static std::mutex mlock;
				// Prevent threading clashes
				{
					std::lock_guard<std::mutex> lck(mlock_serialization_handle);
					if (!mtypes.size())
					{
						std::string formats;
						//serialization::known_formats(formats, compressionEnabled());
						icedb::splitSet::splitSet(formats, mtypes);
					}
				}
				return mtypes;
			}

			bool serialization_handle::match_file_type(const char* filename,
				const char* type,
				const std::set<std::string> &mtypes,
				const char *op)
			{
				using namespace boost::filesystem;
				using std::string;
				using std::ofstream;
				
				//const std::set<string> &mtypes = known_formats();

				string sop(op);
				if (sop.size()) return false;

				// Actually comparing the type and file extension.
				string stype(type);
				string sfilename(filename);

				for (const auto &m : mtypes)
				{
					auto match = [](const std::string &a, const std::string &b) -> bool
					{
						auto res = a.find(b);
						if (res == string::npos) return false;
						// Verify that the extension is at the end of the filename
						string sa = a.substr(a.size() - b.size());
						if (sa == b) return true;
						return false;
					};
					auto res = sfilename.find(m);
					auto resb = stype.find(m);

					if (match(sfilename, m)) return true;
					if (match(stype, m)) return true;
				}

				return false;
			}

			bool serialization_handle::match_file_type_multi(
				std::shared_ptr<icedb::registry::IOhandler> h,
				const char* pluginid,
				std::shared_ptr<icedb::registry::IO_options> opts,
				const std::set<std::string> &mtypes)
			{
				std::string spluginid(pluginid);
				if (h)
				{
					if (h->getId() != spluginid) return false;
					return true;
				}
				else {
					std::string filename = opts->filename();
					std::string type = opts->filetype();
					return match_file_type(filename.c_str(), type.c_str(), mtypes,
						opts->exportType().c_str());
				}
			}
		}
}
}

