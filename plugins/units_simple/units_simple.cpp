#include "../../libicedb/icedb/defs.h"
#include "../../libicedb/icedb/misc/os_functions.h"
#include "../../libicedb/icedb/dlls/plugins.h"
#include "../../libicedb/icedb/units/unitsInterface.hpp"
#include "../../libicedb/icedb/misc/memInterfaceImpl.hpp"
#include "../../libicedb/icedb/misc/utilInterfaceImpl.hpp"

ICEDB_DLL_PLUGINS_COMMON(units_simple);

ICEDB_DLL_BASE_HANDLE* hndDll, *hndSelf;
std::shared_ptr<interface_ICEDB_core_util> i_util;
std::shared_ptr<interface_ICEDB_core_mem> i_mem;

struct simpleUnits {
	static bool canConvert(const std::string &in, const std::string &out) {
		simpleUnits test(in, out, true);
		if (test._valid) return true;
		return false;
	}
	static std::shared_ptr<const simpleUnits> constructConverter(
		const std::string &in, const std::string &out) {
		std::shared_ptr<simpleUnits> res(new simpleUnits(in, out, true));
		return res;
	}
	static simpleUnits* constructConverterP(
		const std::string &in, const std::string &out) {
		simpleUnits*  res = new simpleUnits(in, out, true);
		return res;
	}
	simpleUnits(const std::string &in, const std::string &out, bool init) :
		_inOffset(0), _outOffset(0), _convFactor(1),
		_inUnits(in), _outUnits(out), _valid(true)
	{
		if (!init) return;
		if (validLength(in, out)) return;
		if (validFreq(in, out)) return;
		if (validVol(in, out)) return;
		if (validPres(in, out)) return;
		if (validMass(in, out)) return;
		if (validTemp(in, out)) return;
		if (validDens(in, out)) return;

		_valid = false;
	}
	bool validLength(const std::string &_inUnits, const std::string &_outUnits) {
		bool inV = false, outV = false;
		_convFactor = 1.f;
		if (_inUnits == "nm") { _convFactor /= 1e9; inV = true; }
		if (_inUnits == "um" || _inUnits == "microns" || _inUnits == "micrometers")
		{
			_convFactor /= 1e6; inV = true;
		}
		if (_inUnits == "mm") { _convFactor /= 1e3; inV = true; }
		if (_inUnits == "cm") { _convFactor *= 0.01; inV = true; }
		if (_inUnits == "km") { _convFactor *= 1000.; inV = true; }
		if (_inUnits == "m") inV = true;
		if (_outUnits == "nm") { _convFactor *= 1e9; outV = true; }
		if (_outUnits == "um" || _outUnits == "microns" || _outUnits == "micrometers")
		{
			_convFactor *= 1e6; outV = true;
		}
		if (_outUnits == "mm") { _convFactor *= 1e3; outV = true; }
		if (_outUnits == "cm") { _convFactor *= 100.; outV = true; }
		if (_outUnits == "km") { _convFactor /= 1000.; outV = true; }
		if (_outUnits == "m") outV = true;

		if (inV && outV) { return true; }
		return false;
	}
	bool validFreq(const std::string &_inUnits, const std::string &_outUnits) {
		bool inV = false, outV = false;
		_convFactor = 1.f;
		if (_inUnits == "GHz") { _convFactor *= 1e9; inV = true; }
		if (_inUnits == "MHz") { _convFactor *= 1e6; inV = true; }
		if (_inUnits == "KHz") { _convFactor *= 1e3; inV = true; }
		if (_inUnits == "Hz" || _inUnits == "s^-1" || _inUnits == "1/s") inV = true;
		if (_outUnits == "GHz") { _convFactor /= 1e9; outV = true; }
		if (_outUnits == "MHz") { _convFactor /= 1e6; outV = true; }
		if (_outUnits == "KHz") { _convFactor /= 1e3; outV = true; }
		if (_outUnits == "Hz" || _outUnits == "s^-1" || _outUnits == "1/s") outV = true;

		if (inV && outV) { return true; }
		return false;
	}
	bool validVol(const std::string &_inUnits, const std::string &_outUnits) {
		bool inV = false, outV = false;
		_convFactor = 1.f;
		std::string in = _inUnits, out = _outUnits;

		// If it doesn't end in ^3, add it. Used to prevent awkward string manipulations.
		if (in.find("^3") == std::string::npos) in.append("^3");
		if (out.find("^3") == std::string::npos) out.append("^3");
		if (in == "nm^3") { _convFactor /= 1e27; inV = true; }
		if (in == "um^3") { _convFactor /= 1e18; inV = true; }
		if (in == "mm^3") { _convFactor /= 1e9; inV = true; }
		if (in == "cm^3") { _convFactor /= 1e6; inV = true; }
		if (in == "km^3") { _convFactor *= 1e6; inV = true; }
		if (in == "m^3") inV = true;
		if (out == "nm^3") { _convFactor *= 1e27; outV = true; }
		if (out == "um^3") { _convFactor *= 1e18; outV = true; }
		if (out == "mm^3") { _convFactor *= 1e9; outV = true; }
		if (out == "cm^3") { _convFactor *= 1e6; outV = true; }
		if (out == "km^3") { _convFactor /= 1e6; outV = true; }
		if (out == "m^3") outV = true;
		if (inV && outV) { return true; }
		return false;
	}
	bool validPres(const std::string &in, const std::string &out) {
		bool inV = false, outV = false;
		_convFactor = 1.f;
		if (in == "mb" || in == "millibar") { _convFactor *= 100; inV = true; }
		if (in == "hPa") { _convFactor *= 100; inV = true; }
		if (in == "Pa") inV = true;
		if (in == "kPa") { _convFactor *= 1000; inV = true; }
		if (in == "bar") { _convFactor *= 100000; inV = true; }
		if (out == "mb" || out == "millibar" || out == "hPa") { _convFactor /= 100; outV = true; }
		if (out == "bar") { _convFactor /= 100000; outV = true; }
		if (out == "Pa") outV = true;
		if (out == "kPa") { _convFactor /= 1000; outV = true; }
		if (inV && outV) { return true; }
		return false;
	}
	bool validMass(const std::string &in, const std::string &out) {
		bool inV = false, outV = false;
		_convFactor = 1.f;
		if (in == "ug") { _convFactor /= 1e9; inV = true; }
		if (in == "mg") { _convFactor /= 1e6; inV = true; }
		if (in == "g") { _convFactor /= 1e3; inV = true; }
		if (in == "kg") inV = true;
		if (out == "ug") { _convFactor *= 1e9; outV = true; }
		if (out == "mg") { _convFactor *= 1e6; outV = true; }
		if (out == "g") { _convFactor *= 1e3; outV = true; }
		if (out == "kg") outV = true;
		if (inV && outV) { return true; }
		return false;
	}
	bool validTemp(const std::string &in, const std::string &out) {
		_convFactor = 1.f;
		bool inV = false, outV = false;
		// K - Kelvin, C - Celsius, F - Fahrenheit, R - Rankine
		if (in == "K" || in == "degK") inV = true;
		if (in == "C" || in == "degC") { inV = true; _inOffset += 273.15; }
		if (in == "F" || in == "degF") { inV = true; _convFactor *= 5. / 9.; _inOffset += 459.67; }
		if (in == "R" || in == "degR") { inV = true; _convFactor *= 5. / 9; }
		if (out == "K" || out == "degK") outV = true;
		if (out == "C" || out == "degC") { outV = true; _outOffset -= 273.15; }
		if (out == "F" || out == "degF") { outV = true; _convFactor *= 9. / 5.; _outOffset -= 459.67; }
		if (out == "R" || out == "degR") { outV = true; _convFactor *= 9. / 5.; }
		if (inV && outV) { return true; }
		return false;
	}
	bool validDens(const std::string &in, const std::string &out) {
		_convFactor = 1.f;
		// Handling only number density here
		// TODO: do other types of conversions
		// Most further stuff requires knowledge of R, thus knowledge of 
		// relative humidity
		bool inV = false, outV = false;
		if (in == "m^-3") inV = true;
		if (in == "cm^-3") { inV = true; _convFactor *= 1e6; }
		if (out == "cm^-3") { outV = true; _convFactor /= 1e6; }
		if (out == "m^-3") outV = true;
		if (inV && outV) { return true; }
		if (in == "ppmv" && out == "ppmv") { return true; } // ppmv identity
		return false;
	}
	~simpleUnits() {}
	double _inOffset, _outOffset, _convFactor;
	std::string _inUnits, _outUnits;
	bool _valid;
	bool isValid() const { return _valid; }
	double convert(double inVal) const
	{
		if (_valid) return ((inVal + _inOffset) * _convFactor) + (_outOffset);
		return -9999;
	}
};



extern "C" {
	double convert(ICEDB_unit_converter_s* p, double val) {
		simpleUnits *c = static_cast<simpleUnits*>(p->_p);
		return c->convert(val);
	}
	SHARED_EXPORT_ICEDB bool canConvert(const char* type, const char* inUnits, const char* outUnits) {
		std::string sType(type), sIn(inUnits), sOut(outUnits);
        if (sType == "") return simpleUnits::canConvert(sIn, sOut);
        if (sType == "simple") return simpleUnits::canConvert(sIn, sOut);
		return false;
	}
	SHARED_EXPORT_ICEDB ICEDB_unit_converter_s* makeConverter(const char* type, const char* inUnits, const char* outUnits) {
        if (type) {
            std::string stype(type);
            if (stype != "simple" && stype != "") {
                return nullptr;
            }
        }
		ICEDB_unit_converter_s *res = new ICEDB_unit_converter_s;
		res->convert = convert;
		auto p = simpleUnits::constructConverterP(std::string(inUnits), std::string(outUnits));
		res->_p = static_cast<void*>(p);
        //static const char* simpleUnitsName = "simple";
        //res->ctype = simpleUnitsName;
		res->ctype = i_util->strdup_s(i_util.get(), "simple", 7);
		return res;
	}
	SHARED_EXPORT_ICEDB void freeConverter(ICEDB_unit_converter_p p) {
        if (!p) return;
        if (!p->ctype) return;
        std::string stype(p->ctype);
        if (stype != "" && stype != "simple") return; // TODO: throw invalid converter
		i_mem->free(i_mem.get(), (void*) p->ctype);
		p->ctype = nullptr;
        //delete p->ctype;
        if (p->_p) {
            simpleUnits *s = static_cast<simpleUnits*>(p->_p);
            //if (!s) // todo: throw            //simpleUnits*
            delete s;
            p->_p = nullptr;
        }
		if (p) delete p;
	}
	

	SHARED_EXPORT_ICEDB bool Register(ICEDB_register_interface_f fReg, ICEDB_get_module_f fMod, ICEDB_DLL_BASE_HANDLE* hDll, ICEDB_DLL_BASE_HANDLE* hSelf) {
		const size_t sz = 2048;
		char buf[sz] = "";
		hndDll = hDll;
		hndSelf = hSelf;
		i_util = std::shared_ptr<interface_ICEDB_core_util>(create_ICEDB_core_util(hDll), destroy_ICEDB_core_util);
		i_mem = std::shared_ptr<interface_ICEDB_core_mem>(create_ICEDB_core_mem(hDll), destroy_ICEDB_core_mem);

		fMod((void*)Register, sz, buf);
		fReg("units", -1, buf);
		return true;
	}

	SHARED_EXPORT_ICEDB void Unregister(ICEDB_register_interface_f fUnReg, ICEDB_get_module_f fMod) {
		const size_t sz = 2048;
		char buf[sz] = "";
		fUnReg("units", -1, fMod((void*)Unregister, sz, buf));
		i_util = nullptr;
		i_mem = nullptr;
		hndSelf = nullptr;

	}
}
