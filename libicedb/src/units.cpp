#include "../icedb/units/units.hpp"
#include "../icedb/misc/mem.h"
#include "../icedb/dlls/dlls.h"
#include "../icedb/dlls/dlls.hpp"
#include "../icedb/units/unitsInterface.hpp"
#include "../icedb/dlls/dllsImpl.hpp"
#include "../icedb/units/unitsInterfaceImpl.hpp"
#include <vector>
#include <string>

ICEDB_BEGIN_DECL_C
struct ICEDB_UNIT_CONVERTER {
	icedb::units::iface::unitscpp::pointer_type converter;
	ICEDB_unit_converter_s* cimpl;
	ICEDB_UNIT_CONVERTER() : converter(nullptr), cimpl(nullptr) {}
};

ICEDB_UNIT_CONVERTER_p ICEDB_create_unit_converter(const char* type, const char* inUnits, const char* outUnits) {
	ICEDB_UNIT_CONVERTER_p res = new ICEDB_UNIT_CONVERTER;
	//ICEDB_UNIT_CONVERTER_p res = (ICEDB_UNIT_CONVERTER_p) ICEDB_malloc(sizeof(ICEDB_UNIT_CONVERTER));
	res->converter = nullptr;
	std::vector<std::string> dlls = icedb::dll::query_interface("units");
	for (const auto &f : dlls) {
		auto hnd = icedb::dll::Dll_Base_Handle::generate(f.c_str());
		auto iface = icedb::units::iface::unitscpp::generate(hnd);
		if (!iface->Bind_canConvert) continue;
		if (!iface->Bind_freeConverter) continue;
		if (!iface->Bind_makeConverter) continue;
		if (iface->canConvert(type, inUnits, outUnits)) {
			res->converter = iface;
			res->cimpl = iface->makeConverter(type, inUnits, outUnits);
			break;
		}
	}
	if (!res->converter || !res->cimpl) {
		if (res->cimpl) {
			res->converter->freeConverter(res->cimpl);
			res->cimpl = nullptr;
		}
		if (res->converter) {
			ICEDB_destroy_unit_converter(res);
			res = nullptr;
		}
		return nullptr;
	}
	return res;
}
void ICEDB_destroy_unit_converter(ICEDB_UNIT_CONVERTER_p p) {
	if (p) {
		if (p->cimpl) {
			p->converter->freeConverter(p->cimpl);
			p->cimpl = nullptr;
		}
		p->converter = nullptr;
		delete p;
	}
}
double ICEDB_unit_convert(ICEDB_UNIT_CONVERTER_p p, double in) {
	return p->cimpl->convert(p->cimpl, in);
	//return p->converter->convert(in);
}
ICEDB_END_DECL_C





ICEDB_BEGIN_DECL_CPP

namespace icedb {
	namespace units {
		converter::converter() {}
		converter::~converter() {}
		bool converter::isValid() const {
			if (!p) return false;
			if (!p->cimpl) return false;
			if (!p->converter) return false;
			return true;
		}
		converter::converter_p converter::generate(
			const char* type, const char* iunits, const char* ounits) {
			converter::converter_p res(new converter);
			res->p = std::shared_ptr<ICEDB_UNIT_CONVERTER>(
				ICEDB_create_unit_converter("", iunits, ounits),
				ICEDB_destroy_unit_converter);
			if (res->p) return res;
			return nullptr;
		}
		double converter::convert(double val) const {
			if (!p) ICEDB_DEBUG_RAISE_EXCEPTION();
			if (!p->converter) ICEDB_DEBUG_RAISE_EXCEPTION();
			if (!p->cimpl) ICEDB_DEBUG_RAISE_EXCEPTION();
			return p->cimpl->convert(p->cimpl, val);
			//return p->converter->convert(val);
		}
	}
}

ICEDB_END_DECL_CPP

