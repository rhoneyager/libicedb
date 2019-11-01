#include "icedb/defs.h"
#if defined(_MSC_FULL_VER)
#pragma warning( disable : 4244 ) // ICEDB_LOG_WARNINGs C4244 and C4267: size_t to int and int <-> _int64
#pragma warning( disable : 4267 )
#endif

#include <cmath>
#include <complex>
#include <fstream>
#include <valarray>
#include <mutex>
#include <boost/shared_ptr.hpp>
#include <boost/tokenizer.hpp>
#include "icedb/refract/refract.hpp"
#include "icedb/refract/refractBase.hpp"
#include "icedb/misc/zeros.hpp"
#include "icedb/units/units.hpp"
//#include "../private/linterp.h"
#include "icedb/Errors/error.hpp"
#include "icedb/Utils/logging.hpp"

namespace scatdb {
	namespace refract {
		const char* teststr = "test";
	}
}
