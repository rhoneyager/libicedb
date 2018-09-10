#include "defs.hpp"
#include "plugin-example.hpp"
#include <icedb/error.hpp>
#include <icedb/shape.hpp>
#include <icedb/registry.hpp>
#include <icedb/io.hpp>
#include <iostream>
#include <HH/Files.hpp>
#include <HH/Datasets.hpp>
#include <memory>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace icedb {
	namespace plugins {
		namespace example {
			namespace Shapes {
				icedb::Shapes::NewShapeProperties readExample(
					std::shared_ptr<icedb::plugins::example::example_handle> h,
					std::shared_ptr<icedb::registry::IO_options> opts)
				{
					////////////////////////////////////////////////////////////////////
					// Your code goes here.
					// Read from the handle "h" and populate the variables in "result".
					//h->file; // (e.g. shared pointer to an open FILE handle)

					icedb::Shapes::NewShapeProperties result;

					return result;
					////////////////////////////////////////////////////////////////////
				}
			}
		}
	}

	using std::shared_ptr;
	using namespace icedb::plugins::example::Shapes;

	// The plugin system looks for this function to read in a single shape.
	// Use this "as-is" in your code.
	namespace registry {
		template<> shared_ptr<IOhandler>
			read_file_type_multi<icedb::Shapes::NewShapeProperties>
				(shared_ptr<IOhandler> sh, shared_ptr<IO_options> opts,
					shared_ptr<icedb::Shapes::NewShapeProperties > s,
					shared_ptr<const icedb::registry::collectionTyped<icedb::Shapes::NewShapeProperties> > filter)
			{
				// Prepare to read the shape - open a "handle" to the file if it is not already open.
				std::string filename = opts->filename();
				IOhandler::IOtype iotype = opts->getVal<IOhandler::IOtype>("iotype", IOhandler::IOtype::READONLY);
				using std::shared_ptr;
				std::shared_ptr<icedb::plugins::example::example_handle> h = registry::construct_handle
					<registry::IOhandler, icedb::plugins::example::example_handle>(
						sh, PLUGINID, [&]() {return std::shared_ptr<icedb::plugins::example::example_handle>(
							new icedb::plugins::example::example_handle(filename.c_str(), iotype)); });

				// Actually read the shape
				*s = readExample(h, opts);

				// Return the opened "handle".
				return h;
			}
	}
}
