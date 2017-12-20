module gsl.assert;

import std.core;
namespace gsl
{
	export struct fail_fast : public std::logic_error
	{
		explicit fail_fast(char const* const message) : std::logic_error(message) {}
	};
}
