///////////////////////////////////////////////////////////////
//  Copyright 2013 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_

#ifndef BOOST_MP_FLOAT128_HPP
#define BOOST_MP_FLOAT128_HPP

extern "C" {
#include <quadmath.h>
}

#include <boost/multiprecision/number.hpp>

namespace boost{
namespace multiprecision{
namespace backends{

struct float128_backend;

}

using backends::float128_backend;

template<>
struct number_category<backends::float128_backend> : public mpl::int_<number_kind_floating_point> {};
template<>
struct number_category<__float128> : public mpl::int_<number_kind_floating_point> {};

typedef number<float128_backend, et_off> float128;

namespace backends{

struct float128_backend
{
   typedef mpl::list<signed char, short, int, long, long long>   signed_types;
   typedef mpl::list<unsigned char, unsigned short, 
      unsigned int, unsigned long, unsigned long long>           unsigned_types;
   typedef mpl::list<float, double, long double>                 float_types;
   typedef int                                                   exponent_type;

private:
   __float128 m_value;
public:
   float128_backend() : m_value(0) {}
   float128_backend(const float128_backend& o) : m_value(o.m_value) {}
   float128_backend& operator = (const float128_backend& o) 
   {
      m_value = o.m_value;
      return *this;
   }
   template <class T>
   float128_backend(const T& i, const typename enable_if_c<is_convertible<T, __float128>::value>::type* = 0)
      : m_value(i) {}
   template <class T>
   typename enable_if_c<is_arithmetic<T>::value || is_convertible<T, __float128>::value, float128_backend&>::type operator = (const T& i)
   {
      m_value = i;
      return *this;
   }
   float128_backend& operator = (const char* s)
   {
      char* p_end;
      m_value = strtoflt128(s, &p_end);
      if(p_end - s != (std::ptrdiff_t)std::strlen(s))
      {
         BOOST_THROW_EXCEPTION(std::runtime_error("Unable to interpret input string as a floating point value"));
      }
      return *this;
   }
   void swap(float128_backend& o)
   {
      std::swap(m_value, o.value());
   }
   std::string str(std::streamsize digits, std::ios_base::fmtflags f)const
   {
      char buf[100];
      std::string format = "%";
      if(f & std::ios_base::showpos)
         format += "+";
      if(f & std::ios_base::showpoint)
         format += "#";
      format += ".*";
      if(digits == 0)
         digits = 36;
      format += "Q";
      if(f & std::ios_base::scientific)
         format += "e";
      else if(f & std::ios_base::fixed)
         format += "f";
      else
         format += "g";

      int v = quadmath_snprintf (buf, 100, format.c_str(), digits, m_value);

      if((v < 0) || (v >= 99))
         BOOST_THROW_EXCEPTION(std::runtime_error("Formatting quad-float value failed."));
      return buf;
   }
   void negate()
   {
      m_value = -m_value;
   }
   int compare(const float128_backend& o)const
   {
      return m_value == o.m_value ? 0 : m_value < o.m_value ? -1 : 1;
   }
   template <class T>
   int compare(const T& i)const
   {
      return m_value == i ? 0 : m_value < i ? -1 : 1;
   }
   __float128& value()
   {
      return m_value;
   }
   const __float128& value()const
   {
      return m_value;
   }
};

inline void eval_add(float128_backend& result, const float128_backend& a)
{
   result.value() += a.value();
}
template <class A>
inline void eval_add(float128_backend& result, const A& a)
{
   result.value() += a;
}
inline void eval_subtract(float128_backend& result, const float128_backend& a)
{
   result.value() -= a.value();
}
template <class A>
inline void eval_subtract(float128_backend& result, const A& a)
{
   result.value() -= a;
}
inline void eval_multiply(float128_backend& result, const float128_backend& a)
{
   result.value() *= a.value();
}
template <class A>
inline void eval_multiply(float128_backend& result, const A& a)
{
   result.value() *= a;
}
inline void eval_divide(float128_backend& result, const float128_backend& a)
{
   result.value() /= a.value();
}
template <class A>
inline void eval_divide(float128_backend& result, const A& a)
{
   result.value() /= a;
}

inline void eval_add(float128_backend& result, const float128_backend& a, const float128_backend& b)
{
   result.value() = a.value() + b.value();
}
template <class A>
inline void eval_add(float128_backend& result, const float128_backend& a, const A& b)
{
   result.value() = a.value() + b;
}
inline void eval_subtract(float128_backend& result, const float128_backend& a, const float128_backend& b)
{
   result.value() = a.value() - b.value();
}
template <class A>
inline void eval_subtract(float128_backend& result, const float128_backend& a, const A& b)
{
   result.value() = a.value() - b;
}
template <class A>
inline void eval_subtract(float128_backend& result, const A& a, const float128_backend& b)
{
   result.value() = a - b.value();
}
inline void eval_multiply(float128_backend& result, const float128_backend& a, const float128_backend& b)
{
   result.value() = a.value() * b.value();
}
template <class A>
inline void eval_multiply(float128_backend& result, const float128_backend& a, const A& b)
{
   result.value() = a.value() * b;
}
inline void eval_divide(float128_backend& result, const float128_backend& a, const float128_backend& b)
{
   result.value() = a.value() / b.value();
}

template <class R>
inline void eval_convert_to(R* result, const float128_backend& val)
{
   *result = static_cast<R>(val.value());
}

inline void eval_frexp(float128_backend& result, const float128_backend& arg, int* exp)
{
   result.value() = frexpq(arg.value(), exp);
}

inline void eval_ldexp(float128_backend& result, const float128_backend& arg, int exp)
{
   result.value() = ldexpq(arg.value(), exp);
}

inline void eval_floor(float128_backend& result, const float128_backend& arg)
{
   result.value() = floorq(arg.value());
}
inline void eval_ceil(float128_backend& result, const float128_backend& arg)
{
   result.value() = ceilq(arg.value());
}
inline void eval_sqrt(float128_backend& result, const float128_backend& arg)
{
   result.value() = sqrtq(arg.value());
}
inline int eval_fpclassify(const float128_backend& arg)
{
   return isnanq(arg.value()) ? FP_NAN : isinfq(arg.value()) ? FP_INFINITE : arg.value() == 0 ? FP_ZERO : FP_NORMAL;
}

inline void eval_increment(float128_backend& arg)
{
   ++arg.value();
}
inline void eval_decrement(float128_backend& arg)
{
   --arg.value();
}

/*********************************************************************
*
* abs/fabs:
*
*********************************************************************/

inline void eval_abs(float128_backend& result, const float128_backend& arg)
{
   result.value() = fabsq(arg.value());
}
inline void eval_fabs(float128_backend& result, const float128_backend& arg)
{
   result.value() = fabsq(arg.value());
}

/*********************************************************************
*
* Floating point functions:
*
*********************************************************************/

inline void eval_trunc(float128_backend& result, const float128_backend& arg)
{
   if(isnanq(arg.value()) || isinf(arg.value()))
   {
      result = boost::math::policies::raise_rounding_error(
            "boost::multiprecision::trunc<%1%>(%1%)", 0, 
            number<float128_backend, et_off>(arg), 
            number<float128_backend, et_off>(arg), 
            boost::math::policies::policy<>()).backend();
      return;
   }
   result.value() = truncq(arg.value());
}
/*
// 
// This doesn't actually work... rely on our own default version instead.
//
inline void eval_round(float128_backend& result, const float128_backend& arg)
{
   if(isnanq(arg.value()) || isinf(arg.value()))
   {
      result = boost::math::policies::raise_rounding_error(
            "boost::multiprecision::trunc<%1%>(%1%)", 0, 
            number<float128_backend, et_off>(arg), 
            number<float128_backend, et_off>(arg), 
            boost::math::policies::policy<>()).backend();
      return;
   }
   result.value() = roundq(arg.value());
}
*/
inline void eval_exp(float128_backend& result, const float128_backend& arg)
{
   result.value() = expq(arg.value());
}
inline void eval_log(float128_backend& result, const float128_backend& arg)
{
   result.value() = logq(arg.value());
}
inline void eval_log10(float128_backend& result, const float128_backend& arg)
{
   result.value() = log10q(arg.value());
}
inline void eval_sin(float128_backend& result, const float128_backend& arg)
{
   result.value() = sinq(arg.value());
}
inline void eval_cos(float128_backend& result, const float128_backend& arg)
{
   result.value() = cosq(arg.value());
}
inline void eval_tan(float128_backend& result, const float128_backend& arg)
{
   result.value() = tanq(arg.value());
}
inline void eval_asin(float128_backend& result, const float128_backend& arg)
{
   result.value() = asinq(arg.value());
}
inline void eval_acos(float128_backend& result, const float128_backend& arg)
{
   result.value() = acosq(arg.value());
}
inline void eval_atan(float128_backend& result, const float128_backend& arg)
{
   result.value() = atanq(arg.value());
}
inline void eval_sinh(float128_backend& result, const float128_backend& arg)
{
   result.value() = sinhq(arg.value());
}
inline void eval_cosh(float128_backend& result, const float128_backend& arg)
{
   result.value() = coshq(arg.value());
}
inline void eval_tanh(float128_backend& result, const float128_backend& arg)
{
   result.value() = tanhq(arg.value());
}
inline void eval_fmod(float128_backend& result, const float128_backend& a, const float128_backend& b)
{
   result.value() = fmodq(a.value(), b.value());
}
inline void eval_pow(float128_backend& result, const float128_backend& a, const float128_backend& b)
{
   result.value() = powq(a.value(), b.value());
}
inline void eval_atan2(float128_backend& result, const float128_backend& a, const float128_backend& b)
{
   result.value() = atan2q(a.value(), b.value());
}

} // namespace backends

}} // namespaces

namespace std{

template <boost::multiprecision::expression_template_option ExpressionTemplates>
class numeric_limits<boost::multiprecision::number<boost::multiprecision::backends::float128_backend, ExpressionTemplates> >
{
   typedef boost::multiprecision::number<boost::multiprecision::backends::float128_backend, ExpressionTemplates> number_type;
public:
   BOOST_STATIC_CONSTEXPR bool is_specialized = true;
   static number_type (min)() BOOST_NOEXCEPT { return FLT128_MIN; }
   static number_type (max)() BOOST_NOEXCEPT { return FLT128_MAX; }
   static number_type lowest() BOOST_NOEXCEPT { return -(max)(); }
   BOOST_STATIC_CONSTEXPR int digits = FLT128_MANT_DIG;
   BOOST_STATIC_CONSTEXPR int digits10 = 33;
   BOOST_STATIC_CONSTEXPR int max_digits10 = 36;
   BOOST_STATIC_CONSTEXPR bool is_signed = true;
   BOOST_STATIC_CONSTEXPR bool is_integer = false;
   BOOST_STATIC_CONSTEXPR bool is_exact = false;
   BOOST_STATIC_CONSTEXPR int radix = 2;
   static number_type epsilon() { return FLT128_EPSILON; }
   static number_type round_error() { return 0; }
   BOOST_STATIC_CONSTEXPR int min_exponent = FLT128_MIN_EXP;
   BOOST_STATIC_CONSTEXPR int min_exponent10 = min_exponent * 301L / 1000L;
   BOOST_STATIC_CONSTEXPR int max_exponent = FLT128_MAX_EXP;
   BOOST_STATIC_CONSTEXPR int max_exponent10 = max_exponent * 301L / 1000L;
   BOOST_STATIC_CONSTEXPR bool has_infinity = true;
   BOOST_STATIC_CONSTEXPR bool has_quiet_NaN = true;
   BOOST_STATIC_CONSTEXPR bool has_signaling_NaN = false;
   BOOST_STATIC_CONSTEXPR float_denorm_style has_denorm = denorm_absent;
   BOOST_STATIC_CONSTEXPR bool has_denorm_loss = false;
   static number_type infinity() { return 1.0q / 0.0q; }
   static number_type quiet_NaN() { return number_type("nan"); }
   static number_type signaling_NaN() { return 0; }
   static number_type denorm_min() { return 0; }
   BOOST_STATIC_CONSTEXPR bool is_iec559 = true;
   BOOST_STATIC_CONSTEXPR bool is_bounded = false;
   BOOST_STATIC_CONSTEXPR bool is_modulo = false;
   BOOST_STATIC_CONSTEXPR bool traps = false;
   BOOST_STATIC_CONSTEXPR bool tinyness_before = false;
   BOOST_STATIC_CONSTEXPR float_round_style round_style = round_toward_zero;
};

} // namespace std


#endif
