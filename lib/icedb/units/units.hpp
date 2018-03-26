#pragma once
#include "../defs.h"
#include <string>
#include <memory>
namespace icedb {
	/** \brief Provides convenient runtime conversion functions for converting
	 * different units. 
	 *
	 * This includes interconversions between prefixes and conversions
	 * to different unit types. Complex unit systems may also be introduced, allowing for
	 * calculation of conversion factors in equations.
	 **/
	namespace units {
		namespace implementations { struct Unithandler; struct spectralUnits; struct simpleUnits; }
		typedef std::shared_ptr<const implementations::Unithandler> Unithandler_p;
		class converter;
		typedef std::shared_ptr<const converter> converter_p;
		/** \brief Base conversion class
		*
		* Class is virtual. May be overridden with classes that do formulaic operations,
		* such as converters to density in ppmv.
		*
		* Now, with the appropriate DLL loaded, the udunits system will be used for most conversions.
		* The derived classes still have a bit of code for when udunits is not installed.
		**/
		class converter
		{
		public:
			virtual ~converter();
			/// Convert to the output units
			/// \throws If the conversion is invalid
			virtual double convert(double inVal) const;
			/// Check if two units can be interconverted
			static bool canConvert(const std::string &inUnits, const std::string &outUnits);
			/// Create a converter to convert between two types of units.
			/// \returns nullptr if no valid converter can be created for these units.
			static std::shared_ptr<const converter> generate(
				const std::string &inUnits, const std::string& outUnits);
			/// Is this conversion valid?
			bool isValid() const;
			converter(const std::string &inUnits, const std::string &outUnits);
		protected:
			/// Get a converter for these units.
			/// \returns nullptr if no valid converter can be created
			static Unithandler_p getConverter(
				const std::string &inUnits, const std::string &outUnits);
			converter();
			Unithandler_p h;
			friend struct implementations::spectralUnits;
			friend struct implementations::simpleUnits;
			friend struct implementations::Unithandler;
		};

		/// \brief Perform interconversions between frequency, wavelength and wavenumber
		/// (GHz, Hz, m, cm, um, cm^-1, m^-1)
		class conv_spec : public converter
		{
		public:
			conv_spec(const std::string &inUnits, const std::string &outUnits);
			static std::shared_ptr<const converter> generate(
				const std::string &inUnits, const std::string& outUnits);
		};
	}
 }

