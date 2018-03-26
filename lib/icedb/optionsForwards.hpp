#pragma once
#include "defs.h"
#include <memory>

namespace icedb {
	namespace registry {
		class options_inner;
		class options;
		typedef std::shared_ptr<options> options_ptr;
		typedef std::shared_ptr<const options> const_options_ptr;
	}
}
