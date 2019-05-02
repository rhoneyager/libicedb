#pragma once
#include "../defs.h"
#include <memory>
#include <string>
#include <iostream>
#include "optionsForwards.hpp"

namespace icedb {
	namespace registry {
		/// Base class for external data access, throgh file I/O or database
		struct ICEDB_DL handler_external
		{
		protected:
			handler_external(const char* id);
			/// Ensures that plugins do not collide
			const char* id;
		public:
			inline const char* getId() { return id; }
			virtual ~handler_external() {}
		};

		/// Base class to handle multiple IO operations on a single file
		/// \todo Add functions for iteration of sub-objects.
		struct ICEDB_DL IOhandler : public handler_external
		{
		protected:
			IOhandler(const char* id);
		public:
			virtual ~IOhandler() {}
			/// If modifying these, change IO_options::setVal and getVal.
			enum class IOtype
			{
				READONLY,
				READWRITE,
				EXCLUSIVE,
				TRUNCATE,
				DEBUG,
				CREATE
			};
		};

		class ICEDB_DL options : public std::enable_shared_from_this<options>
		{
		protected:
			options();
			std::shared_ptr<options_inner> p;
		public:
			virtual ~options();
			static std::shared_ptr<options> generate();
			void enumVals(std::ostream &out) const;
			bool hasVal(const std::string &key) const;
			inline bool has(const std::string &key) const { return hasVal(key); }
			/// Retrieves an option. Throws if nonexistant.
			template <class T> T getVal(const std::string &key) const;
			template <class T> T get(const std::string &key) const { return getVal<T>(key); }
			/// Retrieves an option. Returns defaultval if nonexistant.
			template <class T> T getVal(const std::string &key, const T& defaultval) const;
			template <class T> T get(const std::string &key, const T& defaultval) const { return getVal<T>(key, defaultval); }
			/// Adds or replaces an option.
			template <class T> options_ptr setVal(const std::string &key, const T &value);
			template <class T> options_ptr set(const std::string &key, const T &value) { return setVal<T>(key, value); }
			/// Adds an option. Throws if the same name already exists.
			template <class T> options_ptr add(const std::string &key, const T &value);

			// Some convenient definitions
			inline options_ptr filename(const std::string& val) { return setVal<std::string>("filename", val); }
			inline std::string filename() const { return getVal<std::string>("filename", ""); }
			inline options_ptr extension(const std::string& val) { return setVal<std::string>("extension", val); }
			inline std::string extension() const { return getVal<std::string>("extension", ""); }
			inline options_ptr filetype(const std::string &val) { return setVal<std::string>("filetype", val); }
			inline std::string filetype() const { return getVal<std::string>("filetype", ""); }
			inline options_ptr exportType(const std::string &val) { return setVal<std::string>("exportType", val); }
			inline std::string exportType() const { return getVal<std::string>("exportType", ""); }
			options_ptr iotype(IOhandler::IOtype val) { return setVal<IOhandler::IOtype>("ioType", val); }
			IOhandler::IOtype iotype() const { return getVal<IOhandler::IOtype>("ioType", IOhandler::IOtype::TRUNCATE); }

			std::shared_ptr<options> clone() const;
		};

		ICEDB_DL std::ostream & operator<<(std::ostream&, const icedb::registry::IOhandler::IOtype&);
		ICEDB_DL std::istream & operator>>(std::istream&, icedb::registry::IOhandler::IOtype&);
		ICEDB_DL std::ostream & operator<<(std::ostream&, const icedb::registry::options&);
	}
}
