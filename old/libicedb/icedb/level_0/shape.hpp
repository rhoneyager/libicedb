#pragma once
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace icedb {
	namespace level_0 {
		namespace shape {
			template <typename E, typename datatype = double>
			class VecExpression {
			public:
				datatype operator[](size_t i) const { return static_cast<E const&>(*this)[i]; }
				size_t size()               const { return static_cast<E const&>(*this).size(); }

				// The following overload conversions to E, the template argument type;
				// e.g., for VecExpression<VecSum>, this is a conversion to VecSum.
				operator E& () { return static_cast<E&>(*this); }
				operator const E& () const { return static_cast<const E&>(*this); }

			};

			template <typename datatype = double>
			class Vec : public VecExpression<Vec, datatype> {
				std::vector<datatype> elems;
			public:
				datatype operator[](size_t i) const { return elems[i]; }
				datatype &operator[](size_t i) { return elems[i]; }
				size_t size() const { return elems.size(); }
				Vec(size_t n) : elems(n) {}
				// construct vector using initializer list 
				Vec(std::initializer_list<datatype>init) {
					for (auto i : init)
						elems.push_back(i);
				}
				// A Vec can be constructed from any VecExpression, forcing its evaluation.
				template <typename E>
				Vec(VecExpression<E> const& vec) : elems(vec.size()) {
					for (size_t i = 0; i != vec.size(); ++i) {
						elems[i] = vec[i];
					}
				}
			};

			template <typename E1, typename E2, typename datatype = double>
			class VecSum : public VecExpression<VecSum<E1, E2>, datatype > {
				E1 const& _u;
				E2 const& _v;
			public:
				VecSum(E1 const& u, E2 const& v) : _u(u), _v(v) {
					assert(u.size() == v.size());
				}
				double operator[](size_t i) const { return _u[i] + _v[i]; }
				size_t size()               const { return _v.size(); }
			};

			template <typename E1, typename E2, typename datatype = double>
			VecSum<E1, E2, datatype> const
				operator+(E1 const& u, E2 const& v) {
				return VecSum<E1, E2, datatype>(u, v);
			}

			template <typename E1, typename E2>
			class ColVec;
			class shape {
			private:
				shape();
				std::map<std::string, std::string> strAttrs;
				std::map<std::string, float> floatAttrs;
				std::vector<float> points;
			public:
				~shape();
				static std::shared_ptr<shape> generate();
			};
		}
	}
}
