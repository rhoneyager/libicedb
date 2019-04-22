#pragma warning( disable : 4244 ) // ICEDB_LOG_WARNINGs C4244 and C4267: size_t to int and int <-> _int64
#pragma warning( disable : 4267 )
#include <cmath>
#include <complex>
#include <fstream>
#include <valarray>
#include <mutex>
#include <boost/shared_ptr.hpp>
#include <boost/tokenizer.hpp>
#include "../icedb/refract/refract.hpp"
#include "../icedb/refract/refractBase.hpp"
#include "../icedb/zeros.hpp"
#include "../icedb/units/units.hpp"
//#include "../private/linterp.h"
#include "../icedb/error.hpp"
#include "../icedb/logging.hpp"

namespace scatdb {
	namespace refract {
		const char* teststr = "test";
	}
}
