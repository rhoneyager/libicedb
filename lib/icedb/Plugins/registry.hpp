#pragma once

#pragma warning(push)
#pragma warning( disable : 4661 ) // Exporting vector
#include "../defs.h"
#include <functional>
#include <iostream>
#include <map>
//#include <list>
#include <set>
#include <string>
#include <sstream>
#include <vector>
#include <boost/lexical_cast.hpp>
#include <memory>
#include "../Utils/options.hpp"
//#include "debug.h"
#include "../Utils/dlls.hpp"
#include "../Errors/error.hpp"
//#include "info.h"
//#include "logging_base.h"

namespace icedb
{
	namespace io
	{
		template <class obj_class,
		class output_registry_class>
		class implementsStandardWriter;
		template <class obj_class,
		class input_registry_class>
		class implementsStandardReader;
		template <class obj_class,
		class input_registry_class>
		class implementsStandardSingleReader;
	}
	namespace registry
	{
	
		/**
		* \brief Template base class that provides a DLL hook registry for a class.
		*
		* The registry is implemented on a per-class basis because different classes 
		* have different requirements for the DLL registry. Some classes will have both 
		* reading and writing functions. Each type of function that is registered may 
		* have a different function signature.
		*
		* \param signature is the type that gets registered by the DLLs
		* \param registryName is a dummy class to provide multiple registries for the same derived class.
		**/
		template<class registryName, typename signature>
		class usesDLLregistry
		{
		public:
			typedef typename std::vector<signature> hookStorageType;
		protected:
			usesDLLregistry() {}
			template <class T, class U>
			friend class ::icedb::io::implementsStandardWriter;
			template <class T, class U>
			friend class ::icedb::io::implementsStandardReader;
			template <class T, class U>
			friend class ::icedb::io::implementsStandardSingleReader;
			
		public:
			/// \note Implemented as a function-internal static function to avoid gcc template issue.
			static std::shared_ptr<hookStorageType> getHooks()
			{
				static std::shared_ptr<hookStorageType> hooks;
				if (!hooks) hooks = 
					std::shared_ptr<hookStorageType>(new hookStorageType);
				add_hook_table(ICEDB_FUNCSIG, hooks.get());
				return hooks;
			}
		//public:
			virtual ~usesDLLregistry() {}
			static void registerHook(const signature &f)
			{
				// Log every time a hook is registered, along with the table contents before insert.
				std::shared_ptr<hookStorageType> hookstore = getHooks();
				hookstore->push_back(f); 
			}
		};

		

		/**
		* \param base is the raw pointer type (IOhandler or DBhandler)
		* \param derived is the pointer type being cast to / constructed
		* \param constructor is a function that creates a new object (shared_ptr<derived>).
		*		 Use std::bind and lambdas to feed it any necessary parameters.
		**/
		template<class base, class derived>
		std::shared_ptr<derived> construct_handle
			(const std::shared_ptr<base> sh, const char* id,
			const std::function<std::shared_ptr<derived>()> constructor)
		{
			std::shared_ptr<derived> h;
			if (!sh)
				h = std::shared_ptr<derived>(constructor());
			else {
				if (std::string(sh->getId()) != std::string(id))
					ICEDB_throw(::icedb::error::error_types::xDuplicateHook)
					//.add<std::string>("Reason", "Bad passed plugin. The ids do not match.")
					//.add<std::string>("ID_1", sh->getId())
					//.add<std::string>("ID_2", std::string(id));
					;
				h = std::dynamic_pointer_cast<derived>(sh);
			}
			return h;
		}

		/*
		/// \brief Convenient options specification class for use with an IO class registry.
		/// 
		/// Used because std::map doesn't like to go beyond template boundaries
		class options
		{
		protected:
			options();
			std::map<std::string, std::string> _mapStr;
		public:
			virtual ~options();
			void enumVals(std::ostream &out) const;
			static inline std::shared_ptr<options> generate() 
			{ auto res = std::shared_ptr<options>(new options); return res; }
			inline bool hasVal(const std::string &key) const
			{
				if (_mapStr.count(key)) return true;
				return false;
			}
			template <class T> T getVal(const std::string &key) const
			{
				if (!hasVal(key)) ICEDB_throw(::icedb::error::error_types::xMissingKey)
					.add<std::string>("Key", key);
				std::string valS = _mapStr.at(key);
				T res = boost::lexical_cast<T>(valS);
				return res;
			}
			template <class T> T getVal(const std::string &key, const T& defaultval) const
			{
				if (!hasVal(key)) return defaultval;
				return getVal<T>(key);
			}
			template <class T>
			void setVal(const std::string &key, const T &value)
			{
				std::string valS = boost::lexical_cast<std::string>(value);
				_mapStr[key] = valS;
			}
			inline void setVal(const std::string &key, const IOhandler::IOtype val) { setVal<IOhandler::IOtype>(key, val); }

		};
		*/
		typedef options IO_options;
		
		/*class IO_options : public options
		{
		private:
			IO_options();
		public:
			virtual ~IO_options();
			static inline std::shared_ptr<IO_options> generate(IOhandler::IOtype v = IOhandler::IOtype::TRUNCATE)
			{
				auto res = std::shared_ptr<IO_options>(new IO_options); res->iotype(v); return res;
			}
			
			// Some convenient definitions
			void filename(const std::string& val) { setVal<std::string>("filename", val); }
			std::string filename() const { return getVal<std::string>("filename", ""); }
			void extension(const std::string& val) { setVal<std::string>("extension", val); }
			std::string extension() const { return getVal<std::string>("extension", ""); }
			void filetype(const std::string &val) { setVal<std::string>("filetype", val); }
			std::string filetype() const { return getVal<std::string>("filetype", ""); }
			void exportType(const std::string &val) { setVal<std::string>("exportType", val); }
			std::string exportType() const { return getVal<std::string>("exportType", ""); }
			void iotype(IOhandler::IOtype val) { setVal<IOhandler::IOtype>("ioType", val); }
			IOhandler::IOtype iotype() const { return getVal<IOhandler::IOtype>("ioType", IOhandler::IOtype::TRUNCATE); }
		};
		*/

		/// Convenient template pattern for defining an IO class registry
		template<class object>
		struct IO_class_registry
		{
			virtual ~IO_class_registry() {}
			/** \brief If set, indicates that multiple IO operations are possible with this plugin.
			 * \param IO_options specifies the filename, file type, type of object to export, ...
			 * It catches everything because of the limitation in MSVC2012 regarding std::bind number of params.
			 * \param IOhandler is the plugin-provided opaque object that keeps track of 
			 * the state of the object being accessed.
			 **/
			typedef std::function<bool(std::shared_ptr<IOhandler>, std::shared_ptr<IO_options>
				)> io_multi_matcher_type;
			io_multi_matcher_type io_multi_matches;
			std::string registered_name;
		};

		template<class object>
		struct IO_class_registry_writer : IO_class_registry<object>
		{
			virtual ~IO_class_registry_writer() {}
			/** \brief Definition for an object that can handle multiple reads/writes.
			 * \param IOhandler is the plugin-provided opaque object that keeps track of 
			 * the state of the object being accessed.
			 * \param object* is a pointer to the object being read/written
			 * \param IO_options specifies the filename, file type, type of object to export, ...
			 * \returns Pointer to a IOhandler object (for example after the first write).
			 **/
			typedef std::function<std::shared_ptr<IOhandler>
			//	(std::shared_ptr<IOhandler>, std::shared_ptr<IO_options>, const std::shared_ptr<const object>)> io_multi_type;
				(std::shared_ptr<IOhandler>, std::shared_ptr<IO_options>, const object*)> io_multi_type;
			io_multi_type io_multi_processor;
		};

		/// These exist for inheritance, so as to select objects to be read.
		template <class T>
		struct collectionTyped {
			collectionTyped() {} virtual ~collectionTyped() {}
		//virtual void filter() = 0;
		/** Using too many types of shared pointers, so the filter just uses the object's raw pointer. **/
		virtual bool filter(const T*) const = 0;
		virtual bool filter(std::shared_ptr<const T> p) const { return filter(p.get()); }
		};

		template<class object>
		struct IO_class_registry_reader : IO_class_registry<object>
		{
			virtual ~IO_class_registry_reader() {}
			/** \brief Definition for an object that can handle multiple reads/writes.
			 * \param IOhandler is the plugin-provided opaque object that keeps track of 
			 * the state of the object being accessed.
			 * \param object* is a pointer to the object being read/written
			 * \param IO_options specifies the filename, file type, type of object to export, ...
			 * \returns Pointer to a IOhandler object (for example after the first read).
			 **/
			typedef std::function<std::shared_ptr<IOhandler>
				(std::shared_ptr<IOhandler>, std::shared_ptr<IO_options>, 
				std::shared_ptr<object>, std::shared_ptr<const registry::collectionTyped<object> >)> io_multi_type;
			io_multi_type io_multi_processor;

			/** \brief Handles reading multiple objects from a single source
			 * \param IOhandler is the plugin-provided opaque object that keeps track of 
			 * the state of the object being accessed.
			 * \param std::vector<boost::shared_ptr<object> > & is a pointer to the object container.
			 * \param IO_options specifies the filename, file type, type of object to export, ...
			 * It also provides the ability to select objects from the source matching various criteria.
			 * Selection abilities vary based on the plugin.
			 * \returns Pointer to a IOhandler object (for example after the first read).
			 * \deprecated Use io_iterate_type instead.
			 **/
			typedef std::function<std::shared_ptr<IOhandler>
				(std::shared_ptr<IOhandler>, std::shared_ptr<IO_options>, 
				std::vector<std::shared_ptr<object> > &,
				std::shared_ptr<const registry::collectionTyped<object> >)> io_vector_type;

			/** \brief Handles reading multiple objects from a single source
			 * \param IOhandler is the plugin-provided opaque object that keeps track of 
			 * the state of the object being accessed.
			 * \param  is a pointer to the object container.
			 * \param IO_options specifies the filename, file type, type of object to export, ...
			 * It also provides the ability to select objects from the source matching various criteria.
			 * Selection abilities vary based on the plugin.
			 * \returns Pointer to a IOhandler object (for example after the first read).
			 **/
			typedef std::function<std::shared_ptr<IOhandler>
				(
				 	std::shared_ptr<IOhandler>, 
					std::shared_ptr<IO_options>, 
					std::function<void(
						std::shared_ptr<icedb::registry::IOhandler>,
						std::shared_ptr<icedb::registry::IO_options>,
						std::shared_ptr<object>
						) > ,
					std::shared_ptr<const registry::collectionTyped<object> >
				)> io_iterate_type;
			io_iterate_type io_iterator_processor;
		};



		/** \brief Match file type (basic model)
		* \param filename is the file name
		* \param ext is the extension to match (provided by the plugin)
		* \param type is the file's extension (provided by the filename / saver)
		* \param op is the export operation (provided by the lib caller)
		* \param opref is the export operation to match (provided by the plugin)
		**/
		ICEDB_DL bool match_file_type(
			const char* filename, 
			const char* type, const char* ext, 
			const char* op = "", const char* opref = "");

		/// Matches
		ICEDB_DL bool match_file_type_multi(
			std::shared_ptr<registry::IOhandler> h,
			const char* pluginid,
			std::shared_ptr<IO_options> opts,
			std::shared_ptr<IO_options> opts2);
	}
}


#pragma warning(pop)
