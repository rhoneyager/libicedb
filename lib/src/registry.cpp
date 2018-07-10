/**
* \brief Contains registry functions for extending functionality through DLLs.
*
* Contains both general and OS-specific functions.
**/

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
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
#include "../icedb/versioning/versioning.hpp"

namespace icedb
{
	namespace registry
	{
		
		handler_external::handler_external(const char* id) : id(id) {}
		options::options() {}
		options::~options() {}
		void options::enumVals(std::ostream &out) const
		{
			using namespace icedb::registry;
			out << "Options definition:\n\tName\tValue" << std::endl;
			for (const auto &v : _mapStr)
			{
				if (v.first != "password")
				{
					out << "\t" << v.first << ":\t" << v.second << std::endl;
				}
				else {
					out << "\t" << v.first << ":\t" << "********" << std::endl;
				}
			}
		}
		IO_options::IO_options() {}
		IO_options::~IO_options() {}

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

} }

