#pragma once
#ifdef __has_include
#  if __has_include(<variant>) && 0
#    include <variant>
#    define have_stdcpplib_variant 1
	using std::variant;
#  else
#    include "../../related_includes/variant/include/mpark/variant.hpp"
#    define have_stdcpplib_variant 0
	using mpark::variant;
#  endif
#endif

