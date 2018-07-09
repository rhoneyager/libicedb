#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <complex>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
//#include <boost/shared_ptr.hpp>
#include "ddVersions.h"
#include "parids.h"
//#include <Ryan_Debug/hash.h>
//#include <Ryan_Debug/registry.h>
//#include <Ryan_Debug/io.h>
//#include "../registry.h"
//#include "../io.h"

namespace icedb {
	namespace io {
		namespace ddscat {

			class rotations;

			/// \brief Contains the functions for parsing ddscat.par files. 
			/// \todo Take these functions and move them into the cpp file.
			namespace ddParParsers
			{
				/// \brief Takes the internal key id and converts it into 
				/// the standard comment string
				/// \todo Change idString to also take version information.
				/// The comments of some of the keys change in v73.
				void idString(ParId id, std::string &key, size_t version = 730);
				bool commentString(ParId id, std::string &key, size_t version = 730);

				/// A very abstract base class
				class ddParLine
				{
				public:
					friend struct std::less<ddscat::ddParParsers::ddParLine>;
					ddParLine(ParId id = UNKNOWN) { _id = id; _endWriteWithEndl = true; }
					virtual ~ddParLine() {}
					/// Read does NOT split keys and vals!!!!!
					virtual void read(std::istream &in)
					{
						// No key / val separation here...
						std::string lin;
						std::getline(in, lin);
						read(lin);
					}
					virtual void read(const std::string &val) = 0;
					virtual void write(std::ostream &out, size_t version, const std::string &suffix = "") = 0;

					void writeComment(std::ostream &out, size_t version)
					{
						std::string comment;
						if (commentString(_id, comment, version))
							out << comment << std::endl;
					}
					//inline size_t line() const { return _line; }
					//void line(size_t nl) { _line = nl; }
					inline ParId id() const { return _id; }
					virtual bool versionValid(size_t ver) const;
					void endWriteWithEndl(bool val) { _endWriteWithEndl = val; }

					bool operator==(ddParLine &rhs);
					bool operator!=(ddParLine &rhs);
					bool operator<(ddParLine &rhs);

				protected:
					//size_t _line;
					ParId _id;
					bool _endWriteWithEndl;
				private:
				};

				/// Added this as a level of abstraction to prevent code duplication
				template <class T>
				class ddParLineSimpleBase : public ddParLine
				{
				public:
					ddParLineSimpleBase(ParId id = UNKNOWN)
						: ddParLine(id) {}
					virtual ~ddParLineSimpleBase() {}
					virtual void read(const std::string &val) = 0;
					virtual void write(std::ostream &out, size_t version, const std::string &suffix = "") = 0;

					void get(T &val) const { val = _val; }
					void set(const T &val) { _val = val; }
				protected:
					T _val;
				};

				// std::string is specialized later
				template <class T>
				class ddParLineSimple : public ddParLineSimpleBase<T>
				{
				public:
					ddParLineSimple(ParId id = UNKNOWN)
						: ddParLineSimpleBase<T>(id) {}
					~ddParLineSimple() {}
					virtual void write(std::ostream &out, size_t version, const std::string &suffix = "")
					{
						this->writeComment(out, version);
						std::string idstr;
						out << this->_val << " ";
						if (this->_endWriteWithEndl)
						{
							idString(this->_id, idstr);
							out << " = " << idstr << suffix << std::endl;
						}
					}
					virtual void read(const std::string &val)
					{
						std::istringstream strm(val);
						T trgt;
						strm >> trgt;
						this->set(trgt);
						//set( boost::lexical_cast<T>(val) );
					}
				};

				// Put parser def here so the subsequent class can load it
				void pString(const std::string &in, std::string &out);

				template <>
				class ddParLineSimple <std::string> : public ddParLineSimpleBase<std::string>
				{
				public:
					ddParLineSimple(ParId id = UNKNOWN)
						: ddParLineSimpleBase(id) {}
					~ddParLineSimple() {}
					virtual void write(std::ostream &out, size_t version, const std::string &suffix = "")
					{
						this->writeComment(out, version);
						std::string idstr;
						out << "\'" << _val << "\'";
						if (this->_endWriteWithEndl)
						{
							idString(this->_id, idstr);
							out << " = " << idstr << suffix << std::endl;
						}
					}
					virtual void read(const std::string &val)
					{
						std::string pstr;
						pString(val, pstr);
						set(pstr);
					}
				};

				template <class T>
				class ddParLineSimplePlural : public ddParLine
				{
				public:
					ddParLineSimplePlural(ParId id = UNKNOWN)
						: ddParLine(id) { }
					virtual ~ddParLineSimplePlural() {}
					virtual void write(std::ostream &out, size_t version, const std::string &suffix = "")
					{
						this->writeComment(out, version);
						std::string idstr;
						for (auto it = _val.begin(); it != _val.end(); ++it)
							out << *it << " ";
						if (this->_endWriteWithEndl)
						{
							idString(this->_id, idstr);
							out << " = " << idstr << suffix << std::endl;
						}
					}
					virtual void read(const std::string &val)
					{
						_val.clear();
						// Parse based on spaces
						typedef boost::tokenizer<boost::char_separator<char> >
							tokenizer;
						boost::char_separator<char> sep(" \t(),");
						tokenizer tcom(val, sep);
						for (auto it = tcom.begin(); it != tcom.end(); ++it)
						{
							std::istringstream strm(*it);
							T trgt;
							strm >> trgt;
							_val.push_back(trgt);
							//set(trgt);
							//_val.push_back(boost::lexical_cast<T>(*it));
						}
					}

					void get(size_t index, T &val) const { val = _val.at(index); }
					void set(size_t index, const T &val) { _val[index] = val; }
					void get(std::vector<T> &val) const { val = _val; }
					void set(const std::vector<T> &val) { _val = val; }
					void resize(size_t sz) { _val.resize(sz); }
				protected:
					mutable std::vector<T> _val;
				};

				/// This is a special case for paired numbers.
				template <class T>
				class ddParTuples : public ddParLineSimplePlural<T>
				{
				public:
					ddParTuples(size_t tuplesize, ParId id = UNKNOWN)
						: ddParLineSimplePlural<T>(id) { _tuplesize = tuplesize; }
					virtual ~ddParTuples() {}
					virtual void write(std::ostream &out, size_t version, const std::string &suffix = "")
					{
						this->writeComment(out, version);
						std::string idstr;
						for (size_t i = 0; i < this->_val.size(); i++)
						{
							if (i % _tuplesize == 0)
								out << "(";
							out << this->_val[i];
							if ((i % _tuplesize) == (_tuplesize - 1))
							{
								out << ") ";
							}
							else {
								out << ",";
							}
						}
						if (this->_endWriteWithEndl)
						{
							idString(this->_id, idstr);
							out << " = " << idstr << suffix << std::endl;
						}
					}
				protected:
					size_t _tuplesize;
				};

				template <class T, class R>
				class ddParLineMixed : public ddParLine
				{
				public:
					ddParLineMixed(size_t numT, size_t nVals, ParId id = UNKNOWN)
						: ddParLine(id) {
						setSep(numT, nVals);
					}
					virtual ~ddParLineMixed() {}
					virtual void read(const std::string &val)
					{
						// Separate based on spaces
						_t.clear();
						_r.clear();
						_t.resize(_numT);
						_r.resize(_nVals - _numT);
						// Parse based on spaces
						typedef boost::tokenizer<boost::char_separator<char> >
							tokenizer;
						boost::char_separator<char> sep(" \t()");
						tokenizer tcom(val, sep);
						size_t num = 0;
						for (auto it = tcom.begin(); it != tcom.end(); ++it)
						{
							//val.push_back(*it);
							// Select where to insert based on position
							if (num < _numT)
							{
								ddParLineSimple<T> in;
								in.read(*it);
								_t[num] = in;
								//_t.push_back(in);
							}
							else {
								ddParLineSimple<R> in;
								in.read(*it);
								_r[num - _numT] = in;
								//_r.push_back(in);
							}
							num++;
						}
					}
					virtual void write(std::ostream &out, size_t version, const std::string &suffix = "")
					{
						this->writeComment(out, version);
						std::string idstr;
						// Each separate member is used to write
						// Suppress the endline emitted by the members
						for (auto it = _t.begin(); it != _t.end(); ++it)
						{
							it->endWriteWithEndl(false);
							it->write(out, version);
						}
						for (auto it = _r.begin(); it != _r.end(); ++it)
						{
							it->endWriteWithEndl(false);
							it->write(out, version);
						}
						if (this->_endWriteWithEndl)
						{
							idString(this->_id, idstr);
							out << " = " << idstr << suffix << std::endl;
						}
					}
					void setSep(size_t numT, size_t nVals) const
					{
						_numT = numT;
						_nVals = nVals;
						// Set array sizes
						_t.resize(numT);
						_r.resize(nVals - numT);
					}
#pragma warning( push ) // Suppress warning. MSVC warning is because of how it branches,
#pragma warning( disable : 4244 ) // even though that part of code is never reached
#pragma warning( disable : 4146 ) // annoying boost garbage
					template <class S>
					void get(size_t index, S &val) const
					{
						// Phrased like this to allow MSVC compile
						if (index >= _numT)
						{
							R pval;
							index -= _numT;
							_r[index].get(pval);
							val = boost::lexical_cast<S>(pval);
						}
						else {
							T pval;
							_t[index].get(pval);
							val = boost::lexical_cast<S>(pval);
						}
					}

					template <class S>
					void set(size_t index, const S &val)
					{
						if (index >= _numT)
						{
							index -= _numT;
							std::ostringstream srm;
							srm << val;
							std::istringstream irm(srm.str());
							R pval;
							irm >> pval;
							_r[index].set(pval);
						}
						else {
							std::ostringstream srm;
							srm << val;
							std::istringstream irm(srm.str());
							T pval;
							irm >> pval;
							_t[index].set(pval);
						}
					}
#pragma warning( pop ) 
				protected:
					mutable size_t _numT, _nVals;
					// TODO: fix mutability / setSep constness
					mutable std::vector<ddParLineSimple<T> > _t;
					mutable std::vector<ddParLineSimple<R> > _r;
				};

				std::shared_ptr<ddParLine> mapKeys(const std::string &key);
			}


#define accessorSimple(name,id,valtype) \
	inline valtype name() const { return __getSimple<valtype>(id); } \
	inline void name(const valtype &v) { __setSimple<valtype>(id, v); }

			// Special case where a bool is stored in the file as an int
#define accessorSimpleBool(name,id) \
	inline bool name() const { return __getSimpleBool(id); } \
	inline void name(const bool &v) { __setSimpleBool(id, v); }

#define accessorSimplePlural(name,id,valtype,sz) \
	inline valtype name(size_t index) const { return __getSimplePlural<valtype>(id, index); } \
	inline void name(size_t index, const valtype &v) { __setSimplePlural<valtype>(id, index, sz, v); } 

#define accessorSimplePluralTuple(name,id,valtype,sz,tuplesz) \
	inline valtype name(size_t index) const { return __getSimplePlural<valtype>(id, index); } \
	inline void name(size_t index, const valtype &v) { __setSimplePluralTuple<valtype>(id, index, sz, v, tuplesz); } 


#define accessorString(getname,setname,id) \
	inline void getname(std::string &val) const { __getString(id, val); } \
	inline std::string getname() const { std::string val; __getString(id, val); return val; } \
	inline void setname(const std::string &v) { __setString(id, v); }

#define accessorStringBool(name,id,bfalse,btrue) \
	inline bool name() const { return __getStringBool(id, bfalse, btrue); } \
	inline void name(bool v) { __setStringBool(id, v, bfalse, btrue); }

		/// Provides local readers and writers for ddscat ddpar data (it's a binder)
			class implementsDDPAR //:
				//private Ryan_Debug::io::implementsIObasic<ddPar, ddPar_IO_output_registry,
				//ddPar_IO_input_registry, ddPar_Standard>
			{
			public:
				virtual ~implementsDDPAR() {}
			protected:
				implementsDDPAR();
			private:
				static const std::set<std::string>& known_formats();
			};

			/**
			* \brief Provides ddscat.par manipulation.
			*
			* Used to both extract information necessary for shapefile formation, and used when
			* upgrading ddscat versions. It also provides writing functionality for the files.
			**/
			class ddPar :
				virtual public std::enable_shared_from_this<ddPar>,
				//virtual public ::Ryan_Debug::registry::usesDLLregistry<
				//::rtmath::ddscat::ddPar_IO_input_registry,
				//::Ryan_Debug::registry::IO_class_registry_reader<::rtmath::ddscat::ddPar> >,
				//virtual public ::Ryan_Debug::registry::usesDLLregistry<
				//::rtmath::ddscat::ddPar_IO_output_registry,
				//::Ryan_Debug::registry::IO_class_registry_writer<::rtmath::ddscat::ddPar> >,
				//virtual public ::Ryan_Debug::io::implementsStandardWriter<ddPar, ddPar_IO_output_registry>,
				//virtual public ::Ryan_Debug::io::implementsStandardReader<ddPar, ddPar_IO_input_registry>,
				virtual public implementsDDPAR
			{
			public:
				/// Load the default ddscat.par file, used in setting default values
				static std::shared_ptr<const ddPar> defaultInstance();
			private:
				ddPar();
				ddPar(const ddPar &src); // Copy constructor
				//ddPar(const std::string &filename, bool populateDefaults = true);
			public:
				~ddPar();
				ddPar & operator=(const ddPar &rhs);

				/** The generators are required for file loading to work. **/
				static std::shared_ptr<ddPar> generate();
				static std::shared_ptr<ddPar> generate(const std::string &filename, bool populateDefaults = true);
				static std::shared_ptr<ddPar> generate(const std::shared_ptr<const ddPar>);

				//void readFile(const std::string &filename, bool overlay = false);
				/// \todo Add in line number matching as a backup when key parsing fails
				void read(std::istream &stream, bool overlay = false);
				void write(std::ostream&) const;
				/// Write a standard DDSCAT par file to the output stream
				static void writeDDSCAT(const std::shared_ptr<const ddPar>, std::ostream &); //, std::shared_ptr<Ryan_Debug::registry::IO_options>);
				/// Read a standard DDSCAT par file from an input stream
				static void readDDSCAT(std::shared_ptr<ddPar>, std::istream &, bool overlay = false);
				/// \note Default parameter case is split because of function binding.
				static void readDDSCATdef(std::shared_ptr<ddPar>, std::istream&); // , std::shared_ptr<Ryan_Debug::registry::IO_options>);

				bool operator==(const ddPar &rhs) const;
				bool operator!=(const ddPar &rhs) const;
				std::shared_ptr<ddPar> clone() const;
				inline size_t version() const { return _version; }
				inline void version(size_t nv) { _version = nv; }

				// Easy to use accessor functions
				accessorStringBool(doTorques, ddParParsers::CMTORQ, "NOTORQ", "DOTORQ");
				accessorString(getSolnMeth, setSolnMeth, ddParParsers::CMDSOL);
				accessorString(getFFTsolver, setFFTsolver, ddParParsers::CMDFFT);
				accessorString(getCalpha, setCalpha, ddParParsers::CALPHA);
				accessorString(getBinning, setBinning, ddParParsers::CBINFLAG);
				accessorSimplePlural(Imem, ddParParsers::DIMENSION, size_t, 3);

				accessorString(getShape, setShape, ddParParsers::CSHAPE);
				accessorSimplePlural(shpar, ddParParsers::SHAPEPARAMS, double, 3);

				void setDiels(const std::vector<std::string>&);
				void getDiels(std::vector<std::string>&) const;
				
				accessorSimpleBool(doNearField, ddParParsers::NRFLD);
#ifdef near
#undef near // Annoying MSVC thing
#endif
				accessorSimplePlural(near, ddParParsers::FRACT_EXTENS, double, 6);
				accessorSimple(maxTol, ddParParsers::TOL, double);
				accessorSimple(maxIter, ddParParsers::MXITER, size_t);
				accessorSimple(gamma, ddParParsers::GAMMA, double);
				accessorSimple(etasca, ddParParsers::ETASCA, double);

				void setWavelengths(double min, double max, size_t n, const std::string &spacing);
				void getWavelengths(double &min, double &max, size_t &n, std::string &spacing) const;
				void getWavelengths(std::set<double> &) const;
				std::string getWavelengths() const;

				accessorSimple(nAmbient, ddParParsers::NAMBIENT, double);

				void setAeff(double min, double max, size_t n, const std::string &spacing);
				void getAeff(double &min, double &max, size_t &n, std::string &spacing) const;
				void getAeff(std::set<double> &) const;
				std::string getAeff() const;

				accessorSimplePluralTuple(PolState, ddParParsers::POLSTATE, double, 6, 2);
				std::string namePolState() const;
				accessorSimple(OrthPolState, ddParParsers::IORTH, size_t);
				accessorSimpleBool(writePol, ddParParsers::IWRPOL);
				accessorSimpleBool(writeSca, ddParParsers::IWRKSC);

				void getRots(rotations &rots) const;
				void setRots(const rotations &rots);

				accessorSimplePlural(firstOri, ddParParsers::IWAV, int, 3);

				void getSIJ(std::set<size_t> &sij) const;
				void setSIJ(const std::set<size_t> &sij);

				accessorString(getCMDFRM, setCMDFRM, ddParParsers::CMDFRM);
				size_t numPlanes() const;
				//accessorSimple(numPlanes, ddParParsers::NPLANES, size_t);
				// Scattering planes
				void getPlane(size_t n, double &phi, double &thetan_min, double &thetan_max, double &dtheta) const;
				void setPlane(size_t n, double phi, double thetan_min, double thetan_max, double dtheta);

				// The older interface
				void insertKey(ddParParsers::ParId key, std::shared_ptr<ddParParsers::ddParLine> ptr);
				void getKey(ddParParsers::ParId key, std::shared_ptr<ddParParsers::ddParLine> &res);
				void getKey(ddParParsers::ParId key, std::shared_ptr<const ddParParsers::ddParLine> &res) const;
				inline std::shared_ptr<ddParParsers::ddParLine> getKey(ddParParsers::ParId key)
				{
					std::shared_ptr<ddParParsers::ddParLine> res;//= nullptr;
					getKey(key, res);
					return res;
				}
				bool exists(ddParParsers::ParId key) const;
				void delKey(ddParParsers::ParId key);
				size_t size() const { return _parsedData.size(); }
				void getPlane(size_t key, std::shared_ptr<ddParParsers::ddParLineSimplePlural<double> > &res);
				void getPlane(size_t key, std::shared_ptr<const ddParParsers::ddParLineSimplePlural<double> > &res) const;
				inline std::shared_ptr<ddParParsers::ddParLineSimplePlural<double> > getPlane(size_t key)
				{
					std::shared_ptr<ddParParsers::ddParLineSimplePlural<double> > res;//= nullptr;
					getPlane(key, res);
					return res;
				}
				void insertPlane(size_t key, std::shared_ptr<ddParParsers::ddParLineSimplePlural<double> > &res);
				void delPlane(size_t key);
				void populateDefaults(bool overwrite = false, const std::string &src = "") const;
			private:
				void _init();
				size_t _version;

				mutable std::map<ddParParsers::ParId,
					std::shared_ptr<ddParParsers::ddParLine> >
					_parsedData;

				mutable std::map<size_t,
					std::shared_ptr<ddParParsers::ddParLineSimplePlural<double> > >
					_scaPlanes;

				mutable std::vector<
					std::shared_ptr<ddParParsers::ddParLineSimple<std::string> > >
					_diels;

				//mutable std::vector< Ryan_Debug::hash::HASH_t > _dielHashes;

				std::string _filename;

				template<class T>
				T __getSimple(ddParParsers::ParId key) const;

				template<class T>
				void __setSimple(ddParParsers::ParId key, T val);

				bool __getSimpleBool(ddParParsers::ParId key) const;
				void __setSimpleBool(ddParParsers::ParId key, bool val);

				template<class valtype>
				valtype __getSimplePlural(ddParParsers::ParId key, size_t index) const;

				template<class valtype>
				void __setSimplePlural(ddParParsers::ParId key, size_t index, size_t maxSize, const valtype &v);

				//template<class valtype>
				//valtype __getSimplePluralTuple(ddParParsers::ParId key, size_t index) const;

				template<class valtype>
				void __setSimplePluralTuple(ddParParsers::ParId key, size_t index, size_t maxSize, const valtype &v, size_t tuplesz);


				void __getString(ddParParsers::ParId id, std::string &val) const;
				void __setString(ddParParsers::ParId id, const std::string &val);

				bool __getStringBool(ddParParsers::ParId id, const std::string &bfalse, const std::string &btrue) const;
				void __setStringBool(ddParParsers::ParId id, bool v, const std::string &bfalse, const std::string &btrue);

			};
		}

#undef accessorSimple
#undef accessorSimpleBool
#undef accessorSimplePlural
#undef accessorString
#undef accessorStringBool

	}
}

