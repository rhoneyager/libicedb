#pragma once
#include "defs.h"
#include <iostream>
#include <sstream>
#include <string>


namespace icedb {
	namespace logging {
		enum PRIORITIES {
			ICEDB_LOG_DEBUG_2,
			ICEDB_LOG_DEBUG_1,
			ICEDB_LOG_INFO,
			ICEDB_LOG_NORMAL,
			ICEDB_LOG_NOTIFICATION,
			ICEDB_LOG_DEBUG_WARNING,
			ICEDB_LOG_ERROR,
			ICEDB_LOG_CRITICAL
		};
		struct log_properties {
			//log_properties();
			bool debugChannel;
			::std::string logFile;
			int consoleLogThreshold;
			int debuggerLogThreshold;
		};
		typedef void(*log_handler_ft)(const char*, const char*, PRIORITIES);
		void register_log_handler(log_handler_ft);
		void emit_log(
			const char* channel,
			const char* message,
			PRIORITIES p
		);
		void emit_log(
			const std::string &channel,
			const std::string &message,
			PRIORITIES p = ICEDB_LOG_NORMAL);
		void setupLogging(
			int argc = 0,
			char** argv = nullptr,
			const log_properties* lps = nullptr);
	}
}

#define ICEDB_log(c,p,x) { ::std::ostringstream l; l << x; \
	::std::string s = l.str(); \
	::icedb::logging::emit_log(c, s, p); }
