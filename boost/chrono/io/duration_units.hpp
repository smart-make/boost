//
//  (C) Copyright Howard Hinnant
//  (C) Copyright 2011 Vicente J. Botet Escriba
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//

#ifndef BOOST_CHRONO_IO_DURATION_UNITS_HPP
#define BOOST_CHRONO_IO_DURATION_UNITS_HPP

#include <boost/chrono/config.hpp>
#include <boost/ratio/ratio_io.hpp>
#include <boost/chrono/duration.hpp>
#include <boost/chrono/io/duration_style.hpp>
#include <boost/chrono/process_cpu_clocks.hpp>
#include <boost/chrono/io/ios_base_state.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/scoped_array.hpp>
#include <boost/chrono/detail/scan_keyword.hpp>
#include <string>
#include <ios>
#include <locale>
#include <algorithm>

namespace boost
{
  namespace chrono
  {
    namespace detail
    {
      template <typename Period>
      struct is_localizable: false_type
      {
      };

      template <>
      struct is_localizable<atto> : true_type
      {
      };
      template <>
      struct is_localizable<femto> : true_type
      {
      };
      template <>
      struct is_localizable<pico> : true_type
      {
      };
      template <>
      struct is_localizable<nano> : true_type
      {
      };
      template <>
      struct is_localizable<micro> : true_type
      {
      };
      template <>
      struct is_localizable<milli> : true_type
      {
      };
      template <>
      struct is_localizable<centi> : true_type
      {
      };
      template <>
      struct is_localizable<deci> : true_type
      {
      };
      template <>
      struct is_localizable<ratio<1> > : true_type
      {
      };
      template <>
      struct is_localizable<deca> : true_type
      {
      };
      template <>
      struct is_localizable<hecto> : true_type
      {
      };
      template <>
      struct is_localizable<kilo> : true_type
      {
      };
      template <>
      struct is_localizable<mega> : true_type
      {
      };
      template <>
      struct is_localizable<giga> : true_type
      {
      };
      template <>
      struct is_localizable<tera> : true_type
      {
      };
      template <>
      struct is_localizable<peta> : true_type
      {
      };
      template <>
      struct is_localizable<exa> : true_type
      {
      };
      template <>
      struct is_localizable<ratio<60> > : true_type
      {
      };
      template <>
      struct is_localizable<ratio<3600> > : true_type
      {
      };

    }
    class rt_ratio
    {
    public:
      template <typename Period>
      rt_ratio(Period const&) :
        num(Period::type::num), den(Period::type::den)
      {
      }

      rt_ratio(intmax_t n = 0, intmax_t d = 0) :
        num(n), den(d)
      {
      }

      intmax_t num;
      intmax_t den;
    };

    /**
     * @c duration_units facet gives useful information about the duration units,
     * as the number of plural forms, the plural form associated to a duration,
     * the text associated to a plural form and a duration's period,
     */
    template <typename CharT = char, class OutputIterator = std::ostreambuf_iterator<CharT> >
    class duration_units: public std::locale::facet
    {
    public:
      /**
       * Type of character the facet is instantiated on.
       */
      typedef CharT char_type;
      /**
       * Type of character string passed to member functions.
       */
      typedef std::basic_string<CharT> string_type;
      /**
       * Type of iterator used to scan the character buffer.
       */
      typedef OutputIterator iter_type;

      static std::locale::id id;

      explicit duration_units(size_t refs = 0) :
        std::locale::facet(refs)
      {
      }

      /**
       *
       * @tparam Rep
       * @tparam Period
       * @Requires Rep must be explicitly convertible to int_least64_t.
       * @Requires Period is named.
       *
       * @param s
       * @param ios
       * @param d
       * @Effects puts the unit associated to the duration @c d in @c s taken in account the @c ios state flags.
       * The implementation uses the non template virtual function do_put_unit as if
       * @code
       *   return do_put_unit(s, ios, get_duration_style(ios), Period(), int_least64_t(d.count()));
       * @codeend
       *
       * where @get_duration_style gives the duration style associated to @ios.
       * @return s
       */
      template <typename Rep, typename Period>
      typename enable_if<detail::is_localizable<Period>, iter_type>::type put_unit(iter_type s, std::ios_base& ios,
          duration<Rep, Period> const& d) const
      {
        return do_put_unit(s, ios, get_duration_style(ios), Period(), int_least64_t(d.count()));
      }

      /**
       *
       * @tparam Rep
       * @tparam Period
       * @Requires Rep must be explicitly convertible to int_least64_t.
       * @Requires Period is not named, that is its textual representation is in the form [N/D].
       *
       * @param s
       * @param ios
       * @param d
       * @Effects puts the unit associated to the duration @c d in @c s taken in account the @c ios state flags.
       * The implementation uses the non template virtual function do_put_unit as if
       * @code
       *   return do_put_unit(s, ios, get_duration_style(ios), rt_ratio(Period()), int_least64_t(d.count()));
       * @codeend
       *
       * where @get_duration_style gives the duration style associated to @ios and
       * rt_ratio is a class that flats the template ration on a run-time ration so we can use it in virtual functions.
       * @return s
       */
      template <typename Rep, typename Period>
      typename disable_if<detail::is_localizable<Period>, iter_type>::type put_unit(iter_type s, std::ios_base& ios,
          duration<Rep, Period> const& d) const
      {
        return do_put_unit(s, ios, get_duration_style(ios), rt_ratio(Period()), int_least64_t(d.count()));
      }

      const string_type* get_n_d_prefix_units_start() const
      {
        return do_get_n_d_prefix_units_start();
      }
      const string_type* get_n_d_prefix_units_end() const
      {
        return do_get_n_d_prefix_units_end();
      }

      const string_type* get_prefix_units_start() const
      {
        return do_get_prefix_units_start();
      }
      const string_type* get_prefix_units_end() const
      {
        return do_get_prefix_units_end();
      }

      bool match_n_d_prefix_unit(const string_type* k) const
      {
        return do_match_n_d_prefix_unit(k);
      }
      bool match_prefix_unit(const string_type* k, rt_ratio& rt) const
      {
        return do_match_prefix_unit(k, rt);
      }

      /**
       *
       * @return the pattern to be used by default.
       */
      string_type get_pattern() const
      {
        return do_get_pattern();
      }

    protected:

      virtual ~duration_units()
      {
      }
      virtual string_type do_get_pattern() const=0;
      virtual const string_type* do_get_n_d_prefix_units_start() const = 0;
      virtual const string_type* do_get_n_d_prefix_units_end() const = 0;
      virtual const string_type* do_get_prefix_units_start() const = 0;
      virtual const string_type* do_get_prefix_units_end() const = 0;
      virtual bool do_match_n_d_prefix_unit(const string_type* k) const = 0;
      virtual bool do_match_prefix_unit(const string_type* k, rt_ratio& rt) const = 0;

      // used for ouput
      virtual iter_type
      do_put_unit(iter_type s, std::ios_base& ios, duration_style_type style, atto, int_least64_t) const = 0;
      virtual iter_type
      do_put_unit(iter_type s, std::ios_base& ios, duration_style_type style, femto, int_least64_t) const = 0;
      virtual iter_type
      do_put_unit(iter_type s, std::ios_base& ios, duration_style_type style, pico, int_least64_t) const = 0;
      virtual iter_type
      do_put_unit(iter_type s, std::ios_base& ios, duration_style_type style, nano, int_least64_t) const = 0;
      virtual iter_type
      do_put_unit(iter_type s, std::ios_base& ios, duration_style_type style, micro, int_least64_t) const = 0;
      virtual iter_type
      do_put_unit(iter_type s, std::ios_base& ios, duration_style_type style, milli, int_least64_t) const = 0;
      virtual iter_type
      do_put_unit(iter_type s, std::ios_base& ios, duration_style_type style, centi, int_least64_t) const = 0;
      virtual iter_type
      do_put_unit(iter_type s, std::ios_base& ios, duration_style_type style, deci, int_least64_t) const = 0;
      virtual iter_type
      do_put_unit(iter_type s, std::ios_base& ios, duration_style_type style, ratio<1> , int_least64_t) const = 0;
      virtual iter_type
      do_put_unit(iter_type s, std::ios_base& ios, duration_style_type style, deca, int_least64_t) const = 0;
      virtual iter_type
      do_put_unit(iter_type s, std::ios_base& ios, duration_style_type style, hecto, int_least64_t) const = 0;
      virtual iter_type
      do_put_unit(iter_type s, std::ios_base& ios, duration_style_type style, kilo, int_least64_t) const = 0;
      virtual iter_type
      do_put_unit(iter_type s, std::ios_base& ios, duration_style_type style, mega, int_least64_t) const = 0;
      virtual iter_type
      do_put_unit(iter_type s, std::ios_base& ios, duration_style_type style, tera, int_least64_t) const = 0;
      virtual iter_type
      do_put_unit(iter_type s, std::ios_base& ios, duration_style_type style, peta, int_least64_t) const = 0;
      virtual iter_type
      do_put_unit(iter_type s, std::ios_base& ios, duration_style_type style, exa, int_least64_t) const = 0;
      virtual iter_type
          do_put_unit(iter_type s, std::ios_base& ios, duration_style_type style, rt_ratio, int_least64_t) const = 0;
      virtual iter_type
      do_put_unit(iter_type s, std::ios_base& ios, duration_style_type style, ratio<60> , int_least64_t) const = 0;
      virtual iter_type
      do_put_unit(iter_type s, std::ios_base& ios, duration_style_type style, ratio<3600> , int_least64_t) const = 0;

    };

    template <typename CharT, class OutputIterator>
    std::locale::id duration_units<CharT, OutputIterator>::id;

    // This class is used to define the strings for the default English
    template <typename CharT = char, class OutputIterator = std::ostreambuf_iterator<CharT> >
    class duration_units_default: public duration_units<CharT, OutputIterator>
    {
      static const std::size_t pfs_ = 2;

    public:
      typedef CharT char_type;
      typedef std::basic_string<CharT> string_type;
      typedef OutputIterator iter_type;

      explicit duration_units_default(size_t refs = 0) :
        duration_units<CharT, OutputIterator> (refs)
      {
        string_type* it = n_d_prefix_units_;
        it = fill_units(it, ratio<1> ());
        it = prefix_units_;
        it = fill_units(it, atto());
        it = fill_units(it, femto());
        it = fill_units(it, pico());
        it = fill_units(it, nano());
        it = fill_units(it, micro());
        it = fill_units(it, milli());
        it = fill_units(it, centi());
        it = fill_units(it, deci());
        it = fill_units(it, deca());
        it = fill_units(it, hecto());
        it = fill_units(it, kilo());
        it = fill_units(it, mega());
        it = fill_units(it, giga());
        it = fill_units(it, tera());
        it = fill_units(it, peta());
        it = fill_units(it, exa());
        it = fill_units(it, ratio<1> ());
        it = fill_units(it, ratio<60> ());
        it = fill_units(it, ratio<3600> ());
      }
      ~duration_units_default()
      {
      }

    protected:

      bool do_match_n_d_prefix_unit(const string_type* k) const
      {
        std::size_t index = (k - n_d_prefix_units_) / (pfs_ + 1);
        switch (index)
        {
        case 0:
          break;
        default:
          return false;
        }
        return true;
      }
      bool do_match_prefix_unit(const string_type* k, rt_ratio& rt) const
      {
        std::size_t index = (k - prefix_units_) / (pfs_ + 1);
        switch (index)
        {
        case 0:
          rt = rt_ratio(atto());
          break;
        case 1:
          rt = rt_ratio(femto());
          break;
        case 2:
          rt = rt_ratio(pico());
          break;
        case 3:
          rt = rt_ratio(nano());
          break;
        case 4:
          rt = rt_ratio(micro());
          break;
        case 5:
          rt = rt_ratio(milli());
          break;
        case 6:
          rt = rt_ratio(centi());
          break;
        case 7:
          rt = rt_ratio(deci());
          break;
        case 8:
          rt = rt_ratio(deca());
          break;
        case 9:
          rt = rt_ratio(hecto());
          break;
        case 10:
          rt = rt_ratio(kilo());
          break;
        case 11:
          rt = rt_ratio(mega());
          break;
        case 12:
          rt = rt_ratio(giga());
          break;
        case 13:
          rt = rt_ratio(tera());
          break;
        case 14:
          rt = rt_ratio(peta());
          break;
        case 15:
          rt = rt_ratio(exa());
          break;
        case 16:
          rt = rt_ratio(ratio<1> ());
          break;
        case 17:
          rt = rt_ratio(ratio<60> ());
          break;
        case 18:
          rt = rt_ratio(ratio<3600> ());
          break;
        default:
          std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
          return false;
        }
        return true;
      }
      const string_type* do_get_n_d_prefix_units_start() const
      {
        return n_d_prefix_units_;
      }
      const string_type* do_get_n_d_prefix_units_end() const
      {
        return n_d_prefix_units_ + (pfs_ + 1);
      }

      virtual const string_type* do_get_prefix_units_start() const
      {
        return prefix_units_;
      }
      virtual const string_type* do_get_prefix_units_end() const
      {
        return prefix_units_ + 19 * (pfs_ + 1);
      }

      virtual std::size_t do_get_plural_forms() const
      {
        return pfs_;
      }

      virtual std::size_t do_get_plural_form(int_least64_t value) const
      {
        return (value == -1 || value == 1) ? 0 : 1;
      }

      string_type do_get_pattern() const
      {
        static const CharT t[] =
        { '%', 'v', ' ', '%', 'u' };
        static const string_type pattern(t, t + sizeof (t) / sizeof (t[0]));

        return pattern;
      }

      iter_type do_put_unit(iter_type s, std::ios_base& ios, duration_style_type style, atto u, int_least64_t value) const
      {
        string_type str = do_get_ratio_prefix(style, u);
        std::copy(str.begin(), str.end(), s);
        return do_put_unit(s, ios, style, ratio<1> (), value);
      }
      iter_type do_put_unit(iter_type s, std::ios_base& ios, duration_style_type style, femto u, int_least64_t value) const
      {
        string_type str = do_get_ratio_prefix(style, u);
        std::copy(str.begin(), str.end(), s);
        return do_put_unit(s, ios, style, ratio<1> (), value);
      }
      iter_type do_put_unit(iter_type s, std::ios_base& ios, duration_style_type style, pico u, int_least64_t value) const
      {
        string_type str = do_get_ratio_prefix(style, u);
        std::copy(str.begin(), str.end(), s);
        return do_put_unit(s, ios, style, ratio<1> (), value);
      }

      iter_type do_put_unit(iter_type s, std::ios_base& ios, duration_style_type style, nano u, int_least64_t value) const
      {
        string_type str = do_get_ratio_prefix(style, u);
        std::copy(str.begin(), str.end(), s);
        return do_put_unit(s, ios, style, ratio<1> (), value);
      }
      iter_type do_put_unit(iter_type s, std::ios_base& ios, duration_style_type style, micro u, int_least64_t value) const
      {
        string_type str = do_get_ratio_prefix(style, u);
        std::copy(str.begin(), str.end(), s);
        return do_put_unit(s, ios, style, ratio<1> (), value);
      }
      iter_type do_put_unit(iter_type s, std::ios_base& ios, duration_style_type style, milli u, int_least64_t value) const
      {
        string_type str = do_get_ratio_prefix(style, u);
        std::copy(str.begin(), str.end(), s);
        return do_put_unit(s, ios, style, ratio<1> (), value);
      }
      iter_type do_put_unit(iter_type s, std::ios_base& ios, duration_style_type style, centi u, int_least64_t value) const
      {
        string_type str = do_get_ratio_prefix(style, u);
        std::copy(str.begin(), str.end(), s);
        return do_put_unit(s, ios, style, ratio<1> (), value);
      }
      iter_type do_put_unit(iter_type s, std::ios_base& ios, duration_style_type style, deci u, int_least64_t value) const
      {
        string_type str = do_get_ratio_prefix(style, u);
        std::copy(str.begin(), str.end(), s);
        return do_put_unit(s, ios, style, ratio<1> (), value);
      }
      iter_type do_put_unit(iter_type s, std::ios_base&, duration_style_type style, ratio<1> u, int_least64_t value) const
      {
        string_type str = do_get_plural_form(style, u, do_get_plural_form(value));
        return std::copy(str.begin(), str.end(), s);
      }
      iter_type do_put_unit(iter_type s, std::ios_base& ios, duration_style_type style, deca u, int_least64_t value) const
      {
        string_type str = do_get_ratio_prefix(style, u);
        std::copy(str.begin(), str.end(), s);
        return do_put_unit(s, ios, style, ratio<1> (), value);
      }
      iter_type do_put_unit(iter_type s, std::ios_base& ios, duration_style_type style, hecto u, int_least64_t value) const
      {
        string_type str = do_get_ratio_prefix(style, u);
        std::copy(str.begin(), str.end(), s);
        return do_put_unit(s, ios, style, ratio<1> (), value);
      }

      iter_type do_put_unit(iter_type s, std::ios_base& ios, duration_style_type style, kilo u, int_least64_t value) const
      {
        string_type str = do_get_ratio_prefix(style, u);
        std::copy(str.begin(), str.end(), s);
        return do_put_unit(s, ios, style, ratio<1> (), value);
      }
      iter_type do_put_unit(iter_type s, std::ios_base& ios, duration_style_type style, mega u, int_least64_t value) const
      {
        string_type str = do_get_ratio_prefix(style, u);
        std::copy(str.begin(), str.end(), s);
        return do_put_unit(s, ios, style, ratio<1> (), value);
      }
      iter_type do_put_unit(iter_type s, std::ios_base& ios, duration_style_type style, giga u, int_least64_t value) const
      {
        string_type str = do_get_ratio_prefix(style, u);
        std::copy(str.begin(), str.end(), s);
        return do_put_unit(s, ios, style, ratio<1> (), value);
      }
      iter_type do_put_unit(iter_type s, std::ios_base& ios, duration_style_type style, tera u, int_least64_t value) const
      {
        string_type str = do_get_ratio_prefix(style, u);
        std::copy(str.begin(), str.end(), s);
        return do_put_unit(s, ios, style, ratio<1> (), value);
      }
      iter_type do_put_unit(iter_type s, std::ios_base& ios, duration_style_type style, peta u, int_least64_t value) const
      {
        string_type str = do_get_ratio_prefix(style, u);
        std::copy(str.begin(), str.end(), s);
        return do_put_unit(s, ios, style, ratio<1> (), value);
      }
      iter_type do_put_unit(iter_type s, std::ios_base& ios, duration_style_type style, exa u, int_least64_t value) const
      {
        string_type str = do_get_ratio_prefix(style, u);
        std::copy(str.begin(), str.end(), s);
        return do_put_unit(s, ios, style, ratio<1> (), value);
      }

      iter_type do_put_unit(iter_type s, std::ios_base&, duration_style_type style, ratio<60> u, int_least64_t value) const
      {
        string_type str = do_get_plural_form(style, u, do_get_plural_form(value));
        return std::copy(str.begin(), str.end(), s);
      }

      iter_type do_put_unit(iter_type s, std::ios_base&, duration_style_type style, ratio<3600> u, int_least64_t value) const
      {
        string_type str = do_get_plural_form(style, u, do_get_plural_form(value));
        return std::copy(str.begin(), str.end(), s);
      }

      iter_type do_put_unit(iter_type s, std::ios_base& ios, duration_style_type style, rt_ratio rtr,
          int_least64_t value) const
      {
        *s++ = CharT('[');
        std::use_facet<std::num_put<CharT, iter_type> >(ios.getloc()).put(s, ios, ' ', rtr.num);
        *s++ = CharT('/');
        std::use_facet<std::num_put<CharT, iter_type> >(ios.getloc()).put(s, ios, ' ', rtr.den);
        *s++ = CharT(']');

        return do_put_unit(s, ios, style, ratio<1> (), value);
      }

      string_type do_get_plural_form(duration_style_type style, ratio<1> , std::size_t pf) const
      {
        static const CharT t[] =
        { 's' };
        static const string_type symbol(t, t + sizeof (t) / sizeof (t[0]));
        static const CharT u[] =
        { 's', 'e', 'c', 'o', 'n', 'd' };
        static const string_type singular(u, u + sizeof (u) / sizeof (u[0]));
        static const CharT v[] =
        { 's', 'e', 'c', 'o', 'n', 'd', 's' };
        static const string_type plural(v, v + sizeof (v) / sizeof (v[0]));

        if (style == duration_style::symbol)
        {
          return symbol;
        }
        if (pf == 0)
        {
          return singular;
        }
        if (pf == 1)
        {
          return plural;
        }
        // assert
        throw "exception";
      }

      virtual string_type do_get_plural_form(duration_style_type style, ratio<60> , std::size_t pf) const
      {
        static const CharT t[] =
        { 'm', 'i', 'n' };
        static const string_type symbol(t, t + sizeof (t) / sizeof (t[0]));

        static const CharT u[] =
        { 'm', 'i', 'n', 'u', 't', 'e' };
        static const string_type singular(u, u + sizeof (u) / sizeof (u[0]));
        static const CharT v[] =
        { 'm', 'i', 'n', 'u', 't', 'e', 's' };
        static const string_type plural(v, v + sizeof (v) / sizeof (v[0]));

        if (style == duration_style::symbol) return symbol;
        if (pf == 0) return singular;
        if (pf == 1) return plural;
        // assert
        throw "exception";
      }

      virtual string_type do_get_plural_form(duration_style_type style, ratio<3600> , std::size_t pf) const
      {
        static const CharT t[] =
        { 'h' };
        static const string_type symbol(t, t + sizeof (t) / sizeof (t[0]));
        static const CharT u[] =
        { 'h', 'o', 'u', 'r' };
        static const string_type singular(u, u + sizeof (u) / sizeof (u[0]));
        static const CharT v[] =
        { 'h', 'o', 'u', 'r', 's' };
        static const string_type plural(v, v + sizeof (v) / sizeof (v[0]));

        if (style == duration_style::symbol) return symbol;
        if (pf == 0) return singular;
        if (pf == 1) return plural;
        // assert
        throw "exception";
      }
      virtual string_type do_get_plural_form(duration_style_type style, atto u, std::size_t pf) const
      {
        return do_get_ratio_prefix(style, u) + do_get_plural_form(style, ratio<1> (), pf);
      }
      virtual string_type do_get_plural_form(duration_style_type style, femto u, std::size_t pf) const
      {
        return do_get_ratio_prefix(style, u) + do_get_plural_form(style, ratio<1> (), pf);
      }
      virtual string_type do_get_plural_form(duration_style_type style, pico u, std::size_t pf) const
      {
        return do_get_ratio_prefix(style, u) + do_get_plural_form(style, ratio<1> (), pf);
      }
      virtual string_type do_get_plural_form(duration_style_type style, nano u, std::size_t pf) const
      {
        return do_get_ratio_prefix(style, u) + do_get_plural_form(style, ratio<1> (), pf);
      }
      virtual string_type do_get_plural_form(duration_style_type style, micro u, std::size_t pf) const
      {
        return do_get_ratio_prefix(style, u) + do_get_plural_form(style, ratio<1> (), pf);
      }
      virtual string_type do_get_plural_form(duration_style_type style, milli u, std::size_t pf) const
      {
        return do_get_ratio_prefix(style, u) + do_get_plural_form(style, ratio<1> (), pf);
      }
      virtual string_type do_get_plural_form(duration_style_type style, centi u, std::size_t pf) const
      {
        return do_get_ratio_prefix(style, u) + do_get_plural_form(style, ratio<1> (), pf);
      }
      virtual string_type do_get_plural_form(duration_style_type style, deci u, std::size_t pf) const
      {
        return do_get_ratio_prefix(style, u) + do_get_plural_form(style, ratio<1> (), pf);
      }
      virtual string_type do_get_plural_form(duration_style_type style, deca u, std::size_t pf) const
      {
        return do_get_ratio_prefix(style, u) + do_get_plural_form(style, ratio<1> (), pf);
      }
      virtual string_type do_get_plural_form(duration_style_type style, hecto u, std::size_t pf) const
      {
        return do_get_ratio_prefix(style, u) + do_get_plural_form(style, ratio<1> (), pf);
      }
      virtual string_type do_get_plural_form(duration_style_type style, kilo u, std::size_t pf) const
      {
        return do_get_ratio_prefix(style, u) + do_get_plural_form(style, ratio<1> (), pf);
      }
      virtual string_type do_get_plural_form(duration_style_type style, mega u, std::size_t pf) const
      {
        return do_get_ratio_prefix(style, u) + do_get_plural_form(style, ratio<1> (), pf);
      }
      virtual string_type do_get_plural_form(duration_style_type style, giga u, std::size_t pf) const
      {
        return do_get_ratio_prefix(style, u) + do_get_plural_form(style, ratio<1> (), pf);
      }
      virtual string_type do_get_plural_form(duration_style_type style, tera u, std::size_t pf) const
      {
        return do_get_ratio_prefix(style, u) + do_get_plural_form(style, ratio<1> (), pf);
      }
      virtual string_type do_get_plural_form(duration_style_type style, peta u, std::size_t pf) const
      {
        return do_get_ratio_prefix(style, u) + do_get_plural_form(style, ratio<1> (), pf);
      }
      virtual string_type do_get_plural_form(duration_style_type style, exa u, std::size_t pf) const
      {
        return do_get_ratio_prefix(style, u) + do_get_plural_form(style, ratio<1> (), pf);
      }

    protected:

      virtual string_type do_get_ratio_prefix(duration_style_type style, atto) const
      {
        if (style == duration_style::symbol) return ratio_string<atto, CharT>::symbol();
        return ratio_string<atto, CharT>::prefix();
      }
      virtual string_type do_get_ratio_prefix(duration_style_type style, femto) const
      {
        if (style == duration_style::symbol) return ratio_string<femto, CharT>::symbol();
        return ratio_string<femto, CharT>::prefix();
      }
      virtual string_type do_get_ratio_prefix(duration_style_type style, pico) const
      {
        if (style == duration_style::symbol) return ratio_string<pico, CharT>::symbol();
        return ratio_string<pico, CharT>::prefix();
      }
      virtual string_type do_get_ratio_prefix(duration_style_type style, nano) const
      {
        if (style == duration_style::symbol) return ratio_string<nano, CharT>::symbol();
        return ratio_string<nano, CharT>::prefix();
      }
      virtual string_type do_get_ratio_prefix(duration_style_type style, micro) const
      {
        if (style == duration_style::symbol) return ratio_string<micro, CharT>::symbol();
        return ratio_string<micro, CharT>::prefix();
      }
      virtual string_type do_get_ratio_prefix(duration_style_type style, milli) const
      {
        if (style == duration_style::symbol) return ratio_string<milli, CharT>::symbol();
        return ratio_string<milli, CharT>::prefix();
      }
      virtual string_type do_get_ratio_prefix(duration_style_type style, centi) const
      {
        if (style == duration_style::symbol) return ratio_string<centi, CharT>::symbol();
        return ratio_string<centi, CharT>::prefix();
      }
      virtual string_type do_get_ratio_prefix(duration_style_type style, deci) const
      {
        if (style == duration_style::symbol) return ratio_string<deci, CharT>::symbol();
        return ratio_string<deci, CharT>::prefix();
      }
      virtual string_type do_get_ratio_prefix(duration_style_type style, deca) const
      {
        if (style == duration_style::symbol) return ratio_string<deca, CharT>::symbol();
        return ratio_string<deca, CharT>::prefix();
      }
      virtual string_type do_get_ratio_prefix(duration_style_type style, hecto) const
      {
        if (style == duration_style::symbol) return ratio_string<hecto, CharT>::symbol();
        return ratio_string<hecto, CharT>::prefix();
      }
      virtual string_type do_get_ratio_prefix(duration_style_type style, kilo) const
      {
        if (style == duration_style::symbol) return ratio_string<kilo, CharT>::symbol();
        return ratio_string<kilo, CharT>::prefix();
      }
      virtual string_type do_get_ratio_prefix(duration_style_type style, mega) const
      {
        if (style == duration_style::symbol) return ratio_string<mega, CharT>::symbol();
        return ratio_string<mega, CharT>::prefix();
      }
      virtual string_type do_get_ratio_prefix(duration_style_type style, giga) const
      {
        if (style == duration_style::symbol) return ratio_string<giga, CharT>::symbol();
        return ratio_string<giga, CharT>::prefix();
      }
      virtual string_type do_get_ratio_prefix(duration_style_type style, tera) const
      {
        if (style == duration_style::symbol) return ratio_string<tera, CharT>::symbol();
        return ratio_string<tera, CharT>::prefix();
      }
      virtual string_type do_get_ratio_prefix(duration_style_type style, peta) const
      {
        if (style == duration_style::symbol) return ratio_string<peta, CharT>::symbol();
        return ratio_string<peta, CharT>::prefix();
      }
      virtual string_type do_get_ratio_prefix(duration_style_type style, exa) const
      {
        if (style == duration_style::symbol) return ratio_string<exa, CharT>::symbol();
        return ratio_string<exa, CharT>::prefix();
      }

      /**
       *
       * @param style
       * @param pf
       * @return the translation associated to the plural form given as parameter.
       */
      template <typename Period>
      typename enable_if<detail::is_localizable<Period>, string_type>::type get_plural_form(duration_style_type style,
          std::size_t pf) const
      {
        return do_get_plural_form(style, Period(), pf);
      }

    private:
      template <typename Period>
      string_type* fill_units(string_type* it, Period) const
      {
        for (std::size_t pf = 0; pf < pfs_; ++pf)
        {
          *it++ = get_plural_form<Period> (duration_style::prefix, pf);
        }
        *it++ = get_plural_form<Period> (duration_style::symbol, 0);
        return it;
      }

      string_type n_d_prefix_units_[3];
      string_type prefix_units_[19 * 3];

    };

  } // chrono

} // boost

#endif  // header
