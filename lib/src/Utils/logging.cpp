#include "icedb/Utils/logging.hpp"
#include <iostream>
#include <fstream>
#include <memory>
#include <sstream>
#include "icedb/misc/os_functions.h"
namespace {
	int logConsoleThreshold = icedb::logging::PRIORITIES::ICEDB_LOG_DEBUG_WARNING;
	int logDebugThreshold = 0;
	std::string logFile;
	std::shared_ptr<std::ofstream> lOut;
	icedb::logging::log_handler_ft logHandler = nullptr;
}
namespace icedb {
	namespace logging {
		void emit_log(
			const std::string &channel,
			const std::string &message,
			PRIORITIES p)
		{
			emit_log(channel.c_str(), message.c_str(), p);
		}
		void register_log_handler(log_handler_ft p) { logHandler = p; }
		void emit_log(
			const char* channel,
			const char* message,
			PRIORITIES p)
		{
			if (logHandler) {
				logHandler(channel, message, p);
			}
			else {
				std::string m;
				std::ostringstream out;
				out << channel << " - " << message << std::endl;
				m = out.str();
				if (p >= logConsoleThreshold)
					std::cerr << m;
				if (p >= logDebugThreshold) {
					ICEDB_writeDebugString(m.c_str());
				}
				if (lOut) {
					*(lOut.get()) << m;
				}
			}
		}
		void setupLogging(
			int argc,
			char** argv,
			const log_properties* lps) {
			if (lps) {
				logConsoleThreshold = lps->consoleLogThreshold;
				logDebugThreshold = lps->debuggerLogThreshold;
				logFile = lps->logFile;
				if (logFile.size()) {
					lOut = std::shared_ptr<std::ofstream>(new std::ofstream(logFile.c_str()));
				}
			}
		}

	}
}

