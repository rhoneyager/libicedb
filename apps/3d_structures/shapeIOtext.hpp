#pragma once
#include <memory>
#include "shapeForwards.hpp"
namespace scatdb {
	namespace plugins {
		namespace builtin {
			namespace shape {
				void register_shape_text();
				std::shared_ptr<::scatdb::shape::shape> readDDSCAT(const char* in);
				std::shared_ptr<::scatdb::shape::shape> readTextRaw(const char *iin);
				void readHeader(const char* in, std::string &desc, size_t &np,
					std::shared_ptr<::scatdb::shape::shapeHeaderStorage_t> hdr,
					size_t &headerEnd);
				void readTextContents(const char *iin, size_t headerEnd,
					std::shared_ptr<::scatdb::shape::shapeStorage_t> data);
				std::shared_ptr<::scatdb::shape::shape> readTextFile(
					const std::string &filename);
				void writeDDSCAT(const std::string &filename, ::scatdb::shape::shape_ptr);
				void writeTextRaw(const std::string &filename, ::scatdb::shape::shape_ptr);
			}
		}
	}
}
