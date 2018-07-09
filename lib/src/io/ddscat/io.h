#pragma once
/** \brief Provides a series of standardized reading and writing functions
* for classes to use to ensure uniform semantics.
**/
#include <string>
#include <set>
#include <thread>
#include <memory>
#include <mutex>
#include <memory>
#include "registry.h"
#include "plugin.h"
//#include "defs.h"

namespace icedb
{
	namespace io
	{
		/// Provides uniform access semantics for compressible text file reading and writing
		namespace TextFiles
		{
			/// Opaque pointer to hide boost stream internals
			class hSerialization;

			/// Handles serialization IO for various classes. Also works for compressible text files (ddpar, ...)
			struct serialization_handle : public icedb::registry::IOhandler
			{
				serialization_handle(const char* filename,
					icedb::registry::IOhandler::IOtype t);
				virtual ~serialization_handle();
				void open(const char* filename, IOtype t);

				/// Separate definition from getID(), as this is static
				static const char* getSHid();

				/** \brief Match file type for serialized data
				* \param filename is the file name.
				* \param type is the file's extension (provided by the filename / saver).
				* \param op is the export operation (provided by the lib caller).
				**/
				static bool match_file_type(
					const char* filename,
					const char* type,
					const std::set<std::string> &mtypes,
					const char* op = "");
				static bool match_file_type(
					const char* filename,
					const char* type,
					const char* op = "")
				{
					return match_file_type(filename, type, known_formats(), op);
				}

				/** \brief Match file type for serialized data
				* \param h is and existing IO handler.
				* \param pluginid is the plugin identifier (fails if not the serialization id).
				* \param opts are the IO_oprions for the desired file read/write.
				**/
				static bool match_file_type_multi(
					std::shared_ptr<icedb::registry::IOhandler> h,
					const char* pluginid,
					std::shared_ptr<icedb::registry::IO_options> opts, 
					const std::set<std::string> &mtypes);

				std::weak_ptr<std::ostream> writer;
				std::weak_ptr<std::istream> reader;

				/// Provides known extensions (xml, st * compression pairs) to match against
				static const std::set<std::string>& known_formats();

				/// Reports whether compression is enabled at all (master switch in io.cpp)
				static bool compressionEnabled();
			private:
				/// Load file (prepare input stream)
				void load(const char*);
				/// Create file (prepare output stream)
				void create(const char*);

				std::unique_ptr<hSerialization> h;
			};

			template <class obj_class>
			std::shared_ptr<icedb::registry::IOhandler> writeFunc(
				std::shared_ptr<icedb::registry::IOhandler> sh,
				std::shared_ptr<icedb::registry::IO_options> opts,
				const std::shared_ptr<const obj_class> obj,
				const std::function<void(const std::shared_ptr<const obj_class>, std::ostream&,
				std::shared_ptr<icedb::registry::IO_options>)> &writer)
			{
				std::string exporttype = opts->exportType();
				std::string filename = opts->filename();
				registry::IOhandler::IOtype iotype = opts->iotype();
				std::string key = opts->getVal<std::string>("key", "");
				using std::shared_ptr;

				std::shared_ptr<serialization_handle> h;
				if (!sh)
					h = std::shared_ptr<serialization_handle>(new serialization_handle(filename.c_str(), iotype));
				else {
					if (sh->getId() != std::string(serialization_handle::getSHid()))
						RDthrow(error::xDuplicateHook())
						<< error::otherErrorText("Passed plugin is "
						"the wrong one. It is not the serialization "
						"plugin.");
					h = std::dynamic_pointer_cast<serialization_handle>(sh);
				}

				// serialization_handle handles compression details
				// Write to a stream, not to a file
				writer(obj, *(h->writer.lock().get()), opts); //, filename);
				
				return h; // Pass back the handle
			};

			template <class obj_class>
			std::shared_ptr<icedb::registry::IOhandler> readFunc(
				std::shared_ptr<icedb::registry::IOhandler> sh,
				std::shared_ptr<icedb::registry::IO_options> opts,
				std::shared_ptr<obj_class> obj,
				const std::function<void(std::shared_ptr<obj_class>, std::istream&,
				std::shared_ptr<icedb::registry::IO_options>)> &reader)
			{
				std::string exporttype = opts->exportType();
				std::string filename = opts->filename();
				registry::IOhandler::IOtype iotype = opts->getVal<registry::IOhandler::IOtype>
					("iotype", registry::IOhandler::IOtype::READONLY);
				std::string key = opts->getVal<std::string>("key", "");
				using std::shared_ptr;

				std::shared_ptr<serialization_handle> h;
				if (!sh)
					h = std::shared_ptr<serialization_handle>(new serialization_handle(filename.c_str(), iotype));
				else {
					if (sh->getId() != std::string(serialization_handle::getSHid()))
						RDthrow(error::xDuplicateHook())
						<< error::otherErrorText("Bad passed plugin. "
						"It is not the serialization plugin.");
					h = std::dynamic_pointer_cast<serialization_handle>(sh);
				}

				// serialization_handle handles compression details
				// Read from a stream, not to a file. Filename is for serialization method detection.
				reader(obj, *(h->reader.lock().get()), opts); //, filename);

				return h; // Pass back the handle
			};

		}

		/// Exists to give the implementsIO template a uniform lock, preventing a race condition
		std::mutex&  getLock();

		/** \brief Template that registers reading and writing methods with the io registry
		 * 
		 * This is the base template that is used when implementing a custom reader/writer to the 
		 * core library code, such as the ddscat readers in ddPar, ddOutputSingle and shapefile.
		 * It is also leveraged in the serialization code.
		 * Any code that reads text files serially and would like optional compression can take advantage of this.
		 *
		 * \todo Split into input and output objects.
		 **/
		template <class obj_class,
		class output_registry_class,
		class input_registry_class,
		class obj_io_separator>
		class implementsIO
		{
		public:
			virtual ~implementsIO() {}
		protected:
			/// \note Cannot use a reference because it ruins the assignment operator
			/// \todo Add custom copy constructor / assignment operator
			const std::set<std::string> matchExts;
		protected:
			implementsIO(const std::set<std::string> &exts) : matchExts(exts) {}
			virtual void makeWriter(icedb::registry::IO_class_registry_writer<obj_class> &writer) = 0;
			virtual void makeReader(icedb::registry::IO_class_registry_reader<obj_class> &reader) = 0;

			virtual void doImplementsIOsetup()
			{
				// Call the binder code
				std::mutex &mlock = getLock();
				// Prevent threading clashes
				{
					std::lock_guard<std::mutex> lck(mlock);
					static bool inited = false; // No need for a static class def.
					if (!inited)
					{
						setup();
						inited = true;
					}
				}
			}
		private:
			virtual void setup()
			{
				emit_io_log("Performing io setup", icedb::log::normal);

				using namespace registry;
				using namespace std;

				static std::vector<IO_class_registry_writer<obj_class> > writers;
				static std::vector<IO_class_registry_reader<obj_class> > readers;

				// Link the functions to the registry
				// Note: the standard genAndRegisterIOregistryPlural_writer will not work here, 
				// as function names would clash.

				// Custom matcher function will match all serialization-supported types!
				//const std::set<std::string> &exts = serialization_handle::known_formats();
				for (const auto &ext : matchExts)
				{
					// ! Generate writer
					using namespace std::placeholders;
					IO_class_registry_writer<obj_class> writer;

					std::ostringstream o;
					o << "Creating & registering reader and writer for extension " << ext;
					emit_io_log(o.str(), icedb::log::normal);

					makeWriter(writer);
					writers.push_back(std::move(writer));
					// ! Register writer
#ifdef _MSC_FULL_VER
					obj_class::usesDLLregistry<output_registry_class, IO_class_registry_writer<obj_class> >::registerHook(*(writers.rbegin()));
#else
					obj_class::template usesDLLregistry<output_registry_class, IO_class_registry_writer<obj_class> >::registerHook(*(writers.rbegin()));
#endif

					// ! Generate reader
					IO_class_registry_reader<obj_class> reader;
					makeReader(reader);
					readers.push_back(std::move(reader));
					// ! Register reader
#ifdef _MSC_FULL_VER
					obj_class::usesDLLregistry<input_registry_class, IO_class_registry_reader<obj_class> >::registerHook(*(readers.rbegin()));
#else
					obj_class::template usesDLLregistry<input_registry_class, IO_class_registry_reader<obj_class> >::registerHook(*(readers.rbegin()));
#endif

				}
			};
		};

		template <class obj_class,
		class output_registry_class,
		class input_registry_class,
		class obj_io_separator>
		class implementsIObasic :
			protected implementsIO<obj_class, output_registry_class,
			input_registry_class, obj_io_separator>
		{
		public:
			virtual ~implementsIObasic() {}
		private:
			typedef const std::function<void(const std::shared_ptr<const obj_class>, std::ostream&, std::shared_ptr<icedb::registry::IO_options>)> outFunc;
			outFunc &outF;
			typedef const std::function<void(std::shared_ptr<obj_class>, std::istream&, std::shared_ptr<icedb::registry::IO_options>)> inFunc;
			inFunc &inF;
		protected:
			implementsIObasic(outFunc &outF, inFunc &inF, const std::set<std::string> &exts) : 
				outF(outF), inF(inF), implementsIO<obj_class, output_registry_class,
				input_registry_class, obj_io_separator>(exts)
			{
				implementsIO<obj_class, output_registry_class,
					input_registry_class, obj_io_separator>::doImplementsIOsetup();
			}
			virtual void makeWriter(icedb::registry::IO_class_registry_writer<obj_class> &writer)
			{
				std::ostringstream o;
				o << "Creating writer (shid " << icedb::io::TextFiles::serialization_handle::getSHid()
					<< "), matching extensions : ";
				for (const auto &e : this->matchExts)
					o << std::endl << e;

				emit_io_log(o.str(), icedb::log::normal);

				writer.io_multi_matches = std::bind(
					icedb::io::TextFiles::serialization_handle::match_file_type_multi,
					std::placeholders::_1, 
					icedb::io::TextFiles::serialization_handle::getSHid(), 
					std::placeholders::_2, this->matchExts);
				auto writerBinder = [&](
					std::shared_ptr<icedb::registry::IOhandler> sh,
					std::shared_ptr<icedb::registry::IO_options> opts,
					const std::shared_ptr<const obj_class> obj, outFunc outF) -> std::shared_ptr<icedb::registry::IOhandler>
				{
					using namespace icedb::registry;
					using namespace icedb::io::TextFiles;
					std::string exporttype = opts->exportType();
					std::string filename = opts->filename();
					IOhandler::IOtype iotype = opts->iotype();
					std::string key = opts->getVal<std::string>("key", "");
					using std::shared_ptr;

					std::shared_ptr<serialization_handle> h;
					if (!sh)
						h = std::shared_ptr<serialization_handle>(new serialization_handle(filename.c_str(), iotype));
					else {
						if (sh->getId() != std::string(serialization_handle::getSHid()))
							RDthrow(::icedb::error::xDuplicateHook())
							<< ::icedb::error::otherErrorText("Bad passed plugin. It is not the serialization plugin.");
						h = std::dynamic_pointer_cast<serialization_handle>(sh);
					}

					// serialization_handle handles compression details
					// Write to a stream, not to a file
					outF(obj, *(h->writer.lock().get()), opts);

					return h; // Pass back the handle
				};
				//writer.io_multi_processor = std::move(writerBinder); // std::bind(writerBinder, _1, _2, _3);
				writer.io_multi_processor = std::bind(writerBinder,
					std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,
					outF);


			}
			virtual void makeReader(icedb::registry::IO_class_registry_reader<obj_class> &reader)
			{
				std::ostringstream o;
				o << "Creating reader (shid " << icedb::io::TextFiles::serialization_handle::getSHid()
					<< "), matching extensions : ";
				for (const auto &e : this->matchExts)
					o << std::endl << e;

				emit_io_log(o.str(), icedb::log::normal);

				reader.io_multi_matches = std::bind(
					icedb::io::TextFiles::serialization_handle::match_file_type_multi,
					std::placeholders::_1, icedb::io::TextFiles::serialization_handle::getSHid(), 
					std::placeholders::_2, this->matchExts);
				auto readerBinder = [&](
					std::shared_ptr<icedb::registry::IOhandler> sh,
					std::shared_ptr<icedb::registry::IO_options> opts,
					std::shared_ptr<obj_class> obj, inFunc inF) -> std::shared_ptr<icedb::registry::IOhandler>
				{
					using namespace icedb::registry;
					using namespace icedb::io::TextFiles;
					std::string exporttype = opts->exportType();
					std::string filename = opts->filename();
					IOhandler::IOtype iotype = opts->getVal<IOhandler::IOtype>("iotype", IOhandler::IOtype::READONLY);
					std::string key = opts->getVal<std::string>("key", "");
					using std::shared_ptr;

					std::shared_ptr<serialization_handle> h;
					if (!sh)
						h = std::shared_ptr<serialization_handle>(new serialization_handle(filename.c_str(), iotype));
					else {
						if (sh->getId() != std::string(serialization_handle::getSHid()))
							RDthrow(::icedb::error::xDuplicateHook())
							<< ::icedb::error::otherErrorText("Bad passed plugin. It is not the serialization plugin.");
						h = std::dynamic_pointer_cast<serialization_handle>(sh);
					}

					// serialization_handle handles compression details
					// Read from a stream, not to a file. Filename is for serialization method detection.
					inF(obj, *(h->reader.lock().get()), opts);
					//reader(obj, *(h->reader.get()), filename);

					return h; // Pass back the handle
				};
				reader.io_multi_processor = std::bind(readerBinder,
					std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,
					inF);

				// The vector reader gets ignored for the standard reader class. The standard file types 
				// get handled by the single object reader.
				/*
				auto vectorReaderBinder = [&](
					std::shared_ptr<icedb::registry::IOhandler> sh,
					std::shared_ptr<icedb::registry::IO_options> opts,
					std::vector<std::shared_ptr<obj_class> > &obj, inFunc inF) 
					-> std::shared_ptr<icedb::registry::IOhandler>
				{
					using namespace icedb::registry;
					using namespace icedb::io::TextFiles;
					std::string exporttype = opts->exportType();
					std::string filename = opts->filename();
					IOhandler::IOtype iotype = opts->getVal<IOhandler::IOtype>("iotype", IOhandler::IOtype::READONLY);
					std::string key = opts->getVal<std::string>("key", "");
					using std::shared_ptr;

					std::shared_ptr<serialization_handle> h;
					if (!sh)
						h = std::shared_ptr<serialization_handle>(new serialization_handle(filename.c_str(), iotype));
					else {
						if (sh->getId() != std::string(serialization_handle::getSHid()))
							RDthrow debug::xDuplicateHook("Bad passed plugin");
						h = std::dynamic_pointer_cast<serialization_handle>(sh);
					}

					// serialization_handle handles compression details
					// Read from a stream, not to a file. Filename is for serialization method detection.
					inF(obj, *(h->reader.lock().get()), opts); // CHANGE THIS
					//reader(obj, *(h->reader.get()), filename);

					return h; // Pass back the handle
				};
				reader.io_vector_processor = std::bind(vectorReaderBinder,
					std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
					inF);
				*/
			}
		};

		template <class obj_class,
		class output_registry_class>
		class implementsStandardWriter :
			virtual public boost::enable_shared_from_this<obj_class>
		{
		protected:
			/// \brief Variable controls if compression is used for certain recognized 
			/// file types (usually variants of text files). Can be turned off in 
			/// class constructor, and can be overridden in IO_options.
			bool autoCompress;
			// Controls whether boost::serialization can write this file.
			//bool serializable;
			implementsStandardWriter(bool autoCompress = true, bool canSerialize = false) :
				autoCompress(autoCompress)//, serializable(canSerialize)
			{}

			/// This actually handles the template writing i/o. It can report the 
			/// success of the write to a calling parent class.
			bool baseWrite(const std::string &filename, const std::string &outtype) const
			{
				auto opts = icedb::registry::IO_options::generate();
				opts->setVal<bool>("autocompress", autoCompress);
				opts->filename(filename);
				opts->setVal("key", filename);
				registry::IOhandler::IOtype accessType = registry::IOhandler::IOtype::TRUNCATE;
				opts->iotype(accessType);
				opts->filetype(outtype);
				auto res = writeMulti(nullptr, opts);
				//auto res = writeMulti(filename.c_str(), nullptr, filename.c_str(),
				//	outtype.c_str(), registry::IOhandler::IOtype::TRUNCATE, opts);
				if (res) return true;
				return false;
			}
		public:
			virtual ~implementsStandardWriter() {}

			/// Duplicate to avoid clashes and having to speify a full template name...
			virtual void writeFile(const std::string &filename, const std::string &outtype = "") const
			{
				baseWrite(filename, outtype);
			}

			virtual void write(const std::string &filename, const std::string &outtype = "") const
			{
				baseWrite(filename, outtype);
			}

			std::shared_ptr<registry::IOhandler> writeMulti(
				std::shared_ptr<icedb::registry::IOhandler> handle,
				std::shared_ptr<icedb::registry::IO_options> opts
				) const
			{
				std::ostringstream o;
				o << "Performing write on file " << opts->filename();
				emit_io_log(o.str(), icedb::log::normal);


				// All of these objects can handle their own compression
				typename ::icedb::registry::IO_class_registry_writer<obj_class>::io_multi_type dllsaver = nullptr;
				// Process dll hooks first
				auto hooks = ::icedb::registry::usesDLLregistry<output_registry_class,
					::icedb::registry::IO_class_registry_writer<obj_class> >::getHooks();
				for (const auto &hook : *hooks)
				{
					if (!hook.io_multi_matches) continue; // Sanity check
					if (!hook.io_multi_processor) continue; // Sanity check
					//if (hook.io_multi_matches(filename, ctype, handle))
					if (hook.io_multi_matches(handle, opts))
					{
						std::ostringstream s;
						s << "Found a match for " << opts->filename();
						emit_io_log(s.str(), icedb::log::normal);

						dllsaver = hook.io_multi_processor;
						break;
					}
				}
				if (dllsaver)
				{
					try {	
						// Most of these types aren't compressible or implement their
						// own compression schemes. So, it's not handled at this level.
						return dllsaver(handle, opts, this->shared_from_this()); //dynamic_cast<const obj_class*>(this));
						//return dllsaver(handle, filename, dynamic_cast<const obj_class*>(this), key, accessType);
					} catch (::boost::exception &e) {
						std::ostringstream s;
						s << "Unable to save file: " << opts->filename() << "\nThrowing error.";
						emit_io_log(s.str(), icedb::log::error);
						e << ::icedb::error::file_name(opts->filename());
						throw;
					}
					
				} else {
					// Cannot match a file type to save.
					// Should never occur.
					std::ostringstream s;
					s << "File format unknown for file: " << opts->filename();
					emit_io_log(s.str(), icedb::log::warning);
					RDthrow(error::xUnknownFileFormat())
					<< error::file_name(opts->filename());
				}
				return nullptr; // Should never be reached
			}

			bool canWriteMulti(
				std::shared_ptr<icedb::registry::IOhandler> handle,
				std::shared_ptr<icedb::registry::IO_options> opts
				) const
			{
				auto hooks = ::icedb::registry::usesDLLregistry<output_registry_class,
					::icedb::registry::IO_class_registry_writer<obj_class> >::getHooks();
				for (const auto &hook : *hooks)
				{
					if (!hook.io_multi_matches) continue; // Sanity check
					if (!hook.io_multi_processor) continue; // Sanity check
					if (hook.io_multi_matches(handle, opts))
					{
						return true;
					}
				}
				return false;
			}
		};

		
		template <class obj_class,
		class input_registry_class>
		class implementsStandardSingleReader :
			virtual public boost::enable_shared_from_this<obj_class>
		{
		protected:
			// Controls whether boost::serialization can write this file.
			//bool serializable;
			implementsStandardSingleReader()
			{}

			/// This actually handles the template writing i/o. It can report the 
			/// success of the write to a calling parent class.
			bool baseRead(const std::string &filename, const std::string &intype,
				std::shared_ptr<const icedb::registry::collectionTyped<obj_class> > filter = nullptr)
			{
				auto opts = icedb::registry::IO_options::generate();
				opts->filename(filename);
				opts->setVal("key", filename);
				registry::IOhandler::IOtype accessType = registry::IOhandler::IOtype::READONLY;
				opts->iotype(accessType);
				opts->filetype(intype);
				auto res = readMulti(nullptr, opts, filter);
				//auto res = readMulti(filename.c_str(), nullptr, filename.c_str(),
				//	outtype.c_str(), registry::IOhandler::IOtype::TRUNCATE, opts);
				if (res) return true;
				return false;
			}
		public:
			virtual ~implementsStandardSingleReader() {}

			/// Duplicate to avoid clashes and having to specify a full template name...
			virtual void readFile(const std::string &filename, const std::string &intype = "",
				std::shared_ptr<const icedb::registry::collectionTyped<obj_class> > filter = nullptr)
			{
				baseRead(filename, intype, filter);
			}

			virtual void read(const std::string &filename, const std::string &intype = "",
				std::shared_ptr<const icedb::registry::collectionTyped<obj_class> > filter = nullptr)
			{
				baseRead(filename, intype, filter);
			}

			std::shared_ptr<registry::IOhandler> readMulti(
				std::shared_ptr<icedb::registry::IOhandler> handle,
				std::shared_ptr<icedb::registry::IO_options> opts,
				std::shared_ptr<const icedb::registry::collectionTyped<obj_class> > filter = nullptr
				)
			{
				std::ostringstream o;
				o << "Performing single read on file " << opts->filename();
				emit_io_log(o.str(), icedb::log::normal);

				// All of these objects can handle their own compression
				typename ::icedb::registry::IO_class_registry_reader<obj_class>::io_multi_type dllsaver = nullptr;
				// Process dll hooks first
				auto hooks = ::icedb::registry::usesDLLregistry<input_registry_class,
					::icedb::registry::IO_class_registry_reader<obj_class> >::getHooks();
				for (const auto &hook : *hooks)
				{
					if (!hook.io_multi_matches) continue; // Sanity check
					if (!hook.io_multi_processor) continue; // Sanity check
					//if (hook.io_multi_matches(filename, ctype, handle))
					if (hook.io_multi_matches(handle, opts))
					{
						std::ostringstream s;
						s << "Found a match";
						emit_io_log(s.str(), icedb::log::normal);
						dllsaver = hook.io_multi_processor;
						break;
					}
				}
				if (dllsaver)
				{
					// Most of these types aren't compressible or implement their
					// own compression schemes. So, it's not handled at this level.
					return dllsaver(handle, opts, this->shared_from_this(), filter); //dynamic_cast<obj_class*>(this), filter);
					//return dllsaver(handle, filename, dynamic_cast<const obj_class*>(this), key, accessType);
				} else {
					// Cannot match a file type to save.
					// Should never occur.
					std::ostringstream s;
					s << "File format unknown for file: " << opts->filename();
					emit_io_log(s.str(), icedb::log::warning);

					RDthrow(error::xUnknownFileFormat()) 
						<< error::file_name(opts->filename());
				}
				return nullptr; // Should never be reached
			}

			static bool canReadMulti(
				std::shared_ptr<icedb::registry::IOhandler> handle,
				std::shared_ptr<icedb::registry::IO_options> opts
				)
			{
				auto hooks = ::icedb::registry::usesDLLregistry<input_registry_class,
					::icedb::registry::IO_class_registry_reader<obj_class> >::getHooks();
				for (const auto &hook : *hooks)
				{
					if (!hook.io_multi_matches) continue; // Sanity check
					if (!hook.io_multi_processor && !hook.io_iterator_processor) continue; // Sanity check
					if (hook.io_multi_matches(handle, opts))
					{
						return true;
					}
				}
				return false;
			}
		};

		template <class obj_class>
		std::shared_ptr<obj_class> customGenerator()
		{
			std::shared_ptr<obj_class> res(new obj_class);
			return res;
		}

	} namespace config {
		class configsegment;
	} namespace io {
		template <>
		std::shared_ptr<::icedb::config::configsegment> 
		//icedb_DLEXPORT
		customGenerator<::icedb::config::configsegment>();


		template <class obj_class,
		class input_registry_class>
		class implementsStandardReader : public implementsStandardSingleReader<obj_class, input_registry_class>
		{
		protected:
			implementsStandardReader() : implementsStandardSingleReader<obj_class, input_registry_class>()
			{}
		public:
			virtual ~implementsStandardReader() {}

			/// Read all matching contained results into a vector
			static std::shared_ptr<registry::IOhandler> readIterate(
				std::shared_ptr<icedb::registry::IOhandler> handle,
				std::shared_ptr<icedb::registry::IO_options> opts,
				std::function<void(
					std::shared_ptr<icedb::registry::IOhandler>,
					std::shared_ptr<icedb::registry::IO_options>,
					std::shared_ptr<obj_class>
					) > v,
				std::shared_ptr<const icedb::registry::collectionTyped<obj_class> > filter
				)
			{
				// Brief invocation of the object constructor to ensure that any custom handlers get registered.
				//std::shared_ptr<obj_class> obj = 
				::icedb::io::customGenerator<obj_class>();

				std::ostringstream o;
				o << "Performing iterative read on file " << opts->filename();
				emit_io_log(o.str(), icedb::log::normal);

				// All of these objects can handle their own compression
				typename ::icedb::registry::IO_class_registry_reader<obj_class>::io_iterate_type dllv = nullptr;
				typename ::icedb::registry::IO_class_registry_reader<obj_class>::io_multi_type dllm = nullptr;
				// Process dll hooks first
				auto hooks = ::icedb::registry::usesDLLregistry<input_registry_class,
					::icedb::registry::IO_class_registry_reader<obj_class> >::getHooks();
				for (const auto &hook : *hooks)
				{
					if (!hook.io_multi_matches) continue; // Sanity check
					//if (hook.io_multi_matches(filename, ctype, handle))
					if (hook.io_multi_matches(handle, opts))
					{
						if (hook.io_iterator_processor)
							dllv = hook.io_iterator_processor;
						else if (hook.io_multi_processor)
							dllm = hook.io_multi_processor;
						else continue; // No vector or multi reader - shouldn't happen if io_multi_matches, but fail to next plugin
						std::ostringstream s;
						s << "Found a match";
						emit_io_log(s.str(), icedb::log::normal);

						break;
					}
				}
				if (dllv || dllm)
				{
					try {
						// Most of these types aren't compressible or implement their
						// own compression schemes. So, it's not handled at this level.
						//
						// dllv is useful for stull like hdf reads, where loading the whole 
						// object before filtration may be undesirable.
						if (dllv) return dllv(handle, opts, v, filter);
						else {
							// obj_Class instance created using a generator template, which can be overridden if
							// the obj_class has no publicly-available constructor (if it only can be used 
							// as a shared_ptr, for example).
							//std::shared_ptr<obj_class> obj = obj_class::generate();
							//std::shared_ptr<obj_class> obj(new obj_class);
							std::shared_ptr<obj_class> obj = ::icedb::io::customGenerator<obj_class>();
							auto res = dllm(handle, opts, obj, filter);
							v(handle, opts, obj);
							return res;
						}
					} catch (::boost::exception &e) {
						std::ostringstream o;
						o << "Unable to read file: " << opts->filename() << "\nThrowing error.";
						emit_io_log(o.str(), icedb::log::error);

						e << ::icedb::error::file_name(opts->filename());
						throw;
					}
					//return dllsaver(handle, filename, dynamic_cast<const obj_class*>(this), key, accessType);
				} else {
					// Cannot match a file type to read.
					// Should never occur.
					std::ostringstream o;
					o << "File format unknown for file: " << opts->filename();
					emit_io_log(o.str(), icedb::log::warning);

					RDthrow(error::xUnknownFileFormat())
						<< error::file_name(opts->filename());
				}
				return nullptr; // Should never be reached
			}

			/// Read all matching contained results into a vector
			static std::shared_ptr<registry::IOhandler> readVector(
				std::shared_ptr<icedb::registry::IOhandler> handle,
				std::shared_ptr<icedb::registry::IO_options> opts,
				std::vector<std::shared_ptr<obj_class> > &v,
				std::shared_ptr<const icedb::registry::collectionTyped<obj_class> > filter
				)
			{
				std::ostringstream o;
				o << "Performing vector read on file " << opts->filename();
				emit_io_log(o.str(), icedb::log::normal);

				auto implVectorInserter = [&](
						std::shared_ptr<icedb::registry::IOhandler>,
						std::shared_ptr<icedb::registry::IO_options>,
						std::shared_ptr<obj_class> obj) -> void
				{
					v.push_back(obj);
				};
				auto res = readIterate(handle,
						opts,
						implVectorInserter,
						//std::bind(implVectorInserter, std::placeholders::_1,
						//	std::placeholders::_2,std::placeholders::_3),
						filter);
				return res;
			}

		};

		/// Quick template to read objects, depending on vector read support.
		template <class T>
		void readObjs(std::vector<std::shared_ptr<T> > &rinputs, const std::string &fname, 
			std::shared_ptr<const icedb::registry::collectionTyped<T> > filter = nullptr)
		{
			auto iopts = registry::IO_options::generate(registry::IOhandler::IOtype::READONLY);
			iopts->filename(fname);
			//if (T::canReadMulti(nullptr, iopts)) // This is wrong. Indicates if it can be read at all, not just for vectors.
				T::readVector(nullptr, iopts, rinputs, filter);
			//else {
			//	std::shared_ptr<T> s(new T);
			//	s->readFile(fname);
			//	rinputs.push_back(s);
			//}
		};

		/**
		* These don't really fit the standard io plugin spec, as they refer to database entries.
		* Database search and update semantics are different, and it is much better to do bulk searches 
		* and updates than one-by-one.
		* Of course, the io plugins could be usable, but selecting ranges for read with IO_options is bad, 
		* and handling many writes would either involve code complexity or poor code performance.
		**/
		template <class obj_class, class registry_class, class index_class, class comp_class, class query_reg_class>
		class implementsDBbasic
		{
		public:
			static std::shared_ptr<index_class> makeQuery() { return index_class::generate(); }
			/* // Doesn't compile in MSVC. Complains about the cast.
			std::shared_ptr<icedb::registry::DBhandler> updateEntry(typename registry_class::updateType t,
				std::shared_ptr<icedb::registry::DBhandler> p = nullptr, 
				std::shared_ptr<registry::DB_options> o = nullptr) const
			{
				auto c = makeCollection();
				std::shared_ptr<obj_class> obj = std::shared_ptr<obj_class>(new obj_class);
				*obj = (dynamic_cast<obj_class*>(this)); // TODO:
				c->insert(obj);
				return updateCollection(c, t, p, o);
			}
			*/
			static typename index_class:: collection makeCollection()
			{
				return typename index_class::collection
					(new std::set<std::shared_ptr<const obj_class>, comp_class >());
			}
			static std::shared_ptr<icedb::registry::DBhandler> 
				updateCollection(typename index_class::collection c, 
				typename registry_class::updateType t, 
				std::shared_ptr<icedb::registry::DBhandler> p = nullptr, 
				std::shared_ptr<registry::DB_options> o = nullptr)
			{
				std::ostringstream os;
				os << "Updating database";
				emit_io_log(os.str(), icedb::log::normal);

				auto hooks = ::icedb::registry::usesDLLregistry<query_reg_class, registry_class >::getHooks();
				for (const auto &h : *(hooks.get()))
				{
					if (!h.fMatches) continue;
					if (!h.fInsertUpdate) continue;
					if (h.fMatches(p, o))
						return h.fInsertUpdate(c, t, p, o);
				}
				return nullptr;
			}
		};

	}
}
