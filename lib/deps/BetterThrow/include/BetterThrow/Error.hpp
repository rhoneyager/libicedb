#pragma once
#include "defs.hpp"
#include <exception>
#include <iostream>
#include <list>
#include <map>
#include <sstream>
#include <string>

namespace BT {
	/// Useful class for tagging key-value pairs.
	class options
	{
		std::map<std::string, std::string> _mapStr;
	public:
		virtual ~options() {}
		options() {}
		/// List all stored values.
		void enumVals(std::ostream& out, int level = 1) const {
			for (const auto& v : _mapStr) {
				out.write("\t", level);
				out << v.first << ":\t" << v.second << std::endl;
			}
		}
		/// Does a key of the specified name exist?
		bool has(const std::string & key) const noexcept {
			if (_mapStr.count(key)) return true;
			return false;
		}
		/// Retrieves an option.
		template <class T> T get(const std::string & key, bool &result) const {
			if (!has(key)) {
				result = false;
				return T();
			}
			/*BT_throw()
				.add("Reason", "key does not exist")
				.add("key", key);
				*/
			std::string valS = _mapStr.at(key);
			std::istringstream i{ valS };
			T res;
			i >> res;
			result = true;
			return res;
		}
		/// Retrieves an option. Returns defaultval if nonexistant.
		template <class T> T get(const std::string & key, const T & defaultval, bool &result) const {
			if (!has(key)) {
				result = false;  return defaultval;
			}
			return get<T>(key, result);
		}
		/// Adds or replaces an option.
		template <class T> options& set(const std::string & key, const T & value) {
			std::ostringstream o;
			o << value;
			std::string valS = o.str();
			_mapStr[key] = valS;
			return *this;
		}
		/// Adds an option. Throws if the same name already exists.
		template <class T> options& add(const std::string & key, const T & value) {
			if (has(key)) throw;
			/*(BT_throw()
				.add("Reason", "key already exists")
				.add<std::string>("key", key)
				.add<T>("newValue", value);
				*/
			return this->set<T>(key, value);
		}
	};

	class xError;
}

/// Add info about there an exception was thrown.
#define BT_RSpushErrorvars \
	.push() \
	.add<std::string>("source_filename", std::string(__FILE__)) \
	.add<int>("source_line", (int)__LINE__) \
	.add<std::string>("source_function", std::string(BT_FUNCSIG))
/// \todo Detect if inherits from std::exception or not.
/// If inheritable, check if it is an xError. If yes, push a new context.
/// If not inheritable, push a new context with what() as the expression.
/// If not an exception, then create a new xError and push the appropriate type in a context.
#define BT_throw ::BT::xError() BT_RSpushErrorvars

namespace BT {
	/// xError is the base BetterThrow error class.
	class xError : virtual public BT_ERROR_INHERITS_FROM
	{
		std::list<options> stk;
		mutable std::string emessage;
		void invalidate() { emessage = ""; }
	public:
		xError() {}
		virtual ~xError() {}
		/// Print the error message.
		virtual const char* what() const noexcept {
			if (emessage.size()) return emessage.c_str();
			std::ostringstream o;
			// Pull from stack
			//int i = 1;
			for (const auto& e : stk) {
				//o << "Throw frame" << std::endl; // " << i << std::endl;
				e.enumVals(o);
				//++i;
			}
			emessage = o.str();
			return emessage.c_str();
		}

		/// Add another exception frame.
		xError& push(const options& op) {
			stk.push_back(op);
			invalidate();
			return *this;
		}
		/// Add another exception frame.
		xError& push() {
			stk.push_back(options());
			invalidate();
			return *this;
		}

		/// Add a key-value pair to the error message.
		template <class T> xError& add(const std::string& key, const T value) {
			if (!stk.size()) push();
			stk.back().add<T>(key, value);
			invalidate();
			return *this;
		}

		/// Throw the error.
		void operator()() {
			xError e = *this;
			throw e;
			//std::throw_with_nested(e);
		}
	};

	/// Convenience function for unwinding an exception stack.
	inline void print_exception(const std::exception& e, std::ostream& out = std::cerr, int level = 0)
	{
		out << "Exception: level: " << level << "\n" << e.what() << std::endl;
		try {
			std::rethrow_if_nested(e);
		}
		catch (const std::exception & f) {
			print_exception(f, out, level + 1);
		}
		catch (...) {
			out << "exception: level: " << level << "\n\tException at this level is not derived from std::exception." << std::endl;
		}
	}

}

