//  Unit test for boost::lexical_cast.
//
//  See http://www.boost.org for most recent version, including documentation.
//
//  Copyright Terje Sletteb and Kevlin Henney, 2005.
//  Copyright Alexander Nasonov, 2006.
//  Copyright Antony Polukhin, 2011-2025.
//
//  Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).
//
// Note: The unit test no longer compile on MSVC 6, but lexical_cast itself works for it.

//
// We need this #define before any #includes: otherwise msvc will emit warnings
// deep within std::string, resulting from our (perfectly legal) use of basic_string
// with a custom traits class:
//
#define _SCL_SECURE_NO_WARNINGS

#include <boost/lexical_cast.hpp>

#include <boost/cstdint.hpp>

#include <boost/core/lightweight_test.hpp>

#include <boost/type_traits/integral_promotion.hpp>
#include <boost/type_traits/make_unsigned.hpp>
#include <string>
#include <vector>
#include <memory>

#if (defined(BOOST_HAS_LONG_LONG) || defined(BOOST_HAS_MS_INT64)) \
    && !(defined(BOOST_MSVC) && BOOST_MSVC < 1300)
#define LCAST_TEST_LONGLONG
#endif

#if defined(BOOST_NO_STRINGSTREAM) || defined(BOOST_NO_STD_WSTRING)
#define BOOST_LCAST_NO_WCHAR_T
#endif

// There's no typeinfo for unsigned __int128 in Xcode_15.0.1
#if defined(BOOST_HAS_INT128) && !defined(BOOST_LEXICAL_CAST_TEST_NO_128_INTS) && !defined(__APPLE__)
# define BOOST_LCAST_TEST_128 1
#endif

// Test all 65536 values if true:
bool const lcast_test_small_integral_types_completely = false;

// lcast_integral_test_counter: use when testing all values of an integral
// types is not possible. Max. portable value is 32767.
int const lcast_integral_test_counter=500;

using namespace boost;

template<class T, class CharT>
void test_conversion_from_integral_to_char(CharT zero)
{
    BOOST_TEST(lexical_cast<CharT>(static_cast<T>(0)) == zero + 0);
    BOOST_TEST(lexical_cast<CharT>(static_cast<T>(1)) == zero + 1);
    BOOST_TEST(lexical_cast<CharT>(static_cast<T>(2)) == zero + 2);
    BOOST_TEST(lexical_cast<CharT>(static_cast<T>(3)) == zero + 3);
    BOOST_TEST(lexical_cast<CharT>(static_cast<T>(4)) == zero + 4);
    BOOST_TEST(lexical_cast<CharT>(static_cast<T>(5)) == zero + 5);
    BOOST_TEST(lexical_cast<CharT>(static_cast<T>(6)) == zero + 6);
    BOOST_TEST(lexical_cast<CharT>(static_cast<T>(7)) == zero + 7);
    BOOST_TEST(lexical_cast<CharT>(static_cast<T>(8)) == zero + 8);
    BOOST_TEST(lexical_cast<CharT>(static_cast<T>(9)) == zero + 9);

    BOOST_TEST_THROWS(lexical_cast<CharT>(static_cast<T>(10)), bad_lexical_cast);

    T t = (std::numeric_limits<T>::max)();
    BOOST_TEST_THROWS(lexical_cast<CharT>(t), bad_lexical_cast);
}

template<class T, class CharT>
void test_conversion_from_char_to_integral(CharT zero)
{
    BOOST_TEST(lexical_cast<T>( static_cast<CharT>(zero + 0)) == static_cast<T>(0) );
    BOOST_TEST(lexical_cast<T>( static_cast<CharT>(zero + 1)) == static_cast<T>(1) );
    BOOST_TEST(lexical_cast<T>( static_cast<CharT>(zero + 2)) == static_cast<T>(2) );
    BOOST_TEST(lexical_cast<T>( static_cast<CharT>(zero + 3)) == static_cast<T>(3) );
    BOOST_TEST(lexical_cast<T>( static_cast<CharT>(zero + 4)) == static_cast<T>(4) );
    BOOST_TEST(lexical_cast<T>( static_cast<CharT>(zero + 5)) == static_cast<T>(5) );
    BOOST_TEST(lexical_cast<T>( static_cast<CharT>(zero + 6)) == static_cast<T>(6) );
    BOOST_TEST(lexical_cast<T>( static_cast<CharT>(zero + 7)) == static_cast<T>(7) );
    BOOST_TEST(lexical_cast<T>( static_cast<CharT>(zero + 8)) == static_cast<T>(8) );
    BOOST_TEST(lexical_cast<T>( static_cast<CharT>(zero + 9)) == static_cast<T>(9) );

    BOOST_TEST_THROWS(lexical_cast<T>( static_cast<CharT>(zero + 10)), bad_lexical_cast);
    BOOST_TEST_THROWS(lexical_cast<T>( static_cast<CharT>(zero - 1)), bad_lexical_cast);
}

template<class T>
void test_conversion_from_integral_to_integral()
{
    T t = 0;
    BOOST_TEST(lexical_cast<T>(t) == t);

    // Next two variables are used to suppress warnings.
    int st = 32767; unsigned int ut = st;
    t = st;
    BOOST_TEST(lexical_cast<short>(t) == st);
    BOOST_TEST(lexical_cast<unsigned short>(t) == ut);
    BOOST_TEST(lexical_cast<int>(t) == st);
    BOOST_TEST(lexical_cast<unsigned int>(t) == ut);
    BOOST_TEST(lexical_cast<long>(t) == st);
    BOOST_TEST(lexical_cast<unsigned long>(t) == ut);

    t = (std::numeric_limits<T>::max)();
    BOOST_TEST(lexical_cast<T>(t) == t);

    t = (std::numeric_limits<T>::min)();
    BOOST_TEST(lexical_cast<T>(t) == t);
}




// Replace "-,999" with "-999".
template<class CharT>
std::basic_string<CharT> to_str_gcc_workaround(std::basic_string<CharT> str)
{
    std::locale loc;
    std::numpunct<CharT> const& np = BOOST_USE_FACET(std::numpunct<CharT>, loc);
    std::ctype<CharT> const& ct = BOOST_USE_FACET(std::ctype<CharT>, loc);

    if(np.grouping().empty())
        return str;

    CharT prefix[3] = { ct.widen('-'), np.thousands_sep(), CharT() };

    if(str.find(prefix) != 0)
        return str;

    prefix[1] = CharT();
    str.replace(0, 2, prefix);
    return str;
}

template<class CharT, class T>
std::basic_string<CharT> to_str(T t)
{
    std::basic_ostringstream<CharT> o;
    o << t;
    return to_str_gcc_workaround(o.str());
}


template<class T, class CharT>
void test_conversion_from_integral_to_string(CharT)
{
    typedef std::numeric_limits<T> limits;
    typedef std::basic_string<CharT> string_type;

    T t;

    t = (limits::min)();
    BOOST_TEST(lexical_cast<string_type>(t) == to_str<CharT>(t));

    t = (limits::max)();
    BOOST_TEST(lexical_cast<string_type>(t) == to_str<CharT>(t));

    if(limits::digits <= 16 && lcast_test_small_integral_types_completely)
        // min and max have already been tested.
        for(t = 1 + (limits::min)(); t != (limits::max)(); ++t)
            BOOST_TEST(lexical_cast<string_type>(t) == to_str<CharT>(t));
    else
    {
        T const min_val = (limits::min)();
        T const max_val = (limits::max)();
        T const half_max_val = max_val / 2;
        T const cnt = lcast_integral_test_counter; // to suppress warnings
        T const counter = cnt < half_max_val ? cnt : half_max_val;

        T i = 0;

        // Test values around min:
        t = min_val;
        for(i = 0; i < counter; ++i, ++t)
            BOOST_TEST(lexical_cast<string_type>(t) == to_str<CharT>(t));

        // Test values around max:
        t = max_val;
        for(i = 0; i < counter; ++i, --t)
            BOOST_TEST(lexical_cast<string_type>(t) == to_str<CharT>(t));

        // Test values around zero:
        if(limits::is_signed)
            for(t = static_cast<T>(-counter); t < static_cast<T>(counter); ++t)
                BOOST_TEST(lexical_cast<string_type>(t) == to_str<CharT>(t));

        // Test values around 100, 1000, 10000, ...
        T ten_power = 100;
        for(int e = 2; e < limits::digits10; ++e, ten_power *= 10)
        {
            // ten_power + 100 probably never overflows
            for(t = ten_power - 100; t != ten_power + 100; ++t)
                BOOST_TEST(lexical_cast<string_type>(t) == to_str<CharT>(t));
        }
    }
}

template<class T, class CharT>
void test_conversion_from_string_to_integral(CharT)
{
    typedef std::numeric_limits<T> limits;
    typedef std::basic_string<CharT> string_type;

    string_type s;
    string_type const zero = to_str<CharT>(0);
    string_type const nine = to_str<CharT>(9);
    T const min_val = (limits::min)();
    T const max_val = (limits::max)();

    s = to_str<CharT>(min_val);
    BOOST_TEST_EQ(lexical_cast<T>(s), min_val);
    if(limits::is_signed)
    {
        BOOST_TEST_THROWS(lexical_cast<T>(s + zero), bad_lexical_cast);
        BOOST_TEST_THROWS(lexical_cast<T>(s + nine), bad_lexical_cast);
    }

    s = to_str<CharT>(max_val);
    BOOST_TEST_EQ(lexical_cast<T>(s), max_val);
    {
        BOOST_TEST_THROWS(lexical_cast<T>(s + zero), bad_lexical_cast);
        BOOST_TEST_THROWS(lexical_cast<T>(s + nine), bad_lexical_cast);

        s = to_str<CharT>(max_val);
        for (int i =1; i <=10; ++i) {
            s[s.size()-1] += 1;
            BOOST_TEST_THROWS(lexical_cast<T>( s ), bad_lexical_cast);
        }

        s = to_str<CharT>(max_val);
        std::locale loc;
        typedef std::numpunct<char> numpunct;
        if ( BOOST_USE_FACET(numpunct, loc).grouping().empty() ) {
            // Following tests work well for locale C
            BOOST_TEST_EQ(lexical_cast<T>(to_str<CharT>(0)+s), max_val);
            BOOST_TEST_EQ(lexical_cast<T>(to_str<CharT>(0)+to_str<CharT>(0)+s), max_val);
            BOOST_TEST_EQ(lexical_cast<T>(to_str<CharT>(0)+to_str<CharT>(0)+to_str<CharT>(0)+s), max_val);
        }

        for (int i =1; i <=256; ++i) {
            BOOST_TEST_THROWS(lexical_cast<T>( to_str<CharT>(i)+s ), bad_lexical_cast);
        }

        typedef typename boost::integral_promotion<T>::type promoted;
        if ( !(boost::is_same<T, promoted>::value) )
        {
            promoted prom = max_val;
            s = to_str<CharT>(max_val);
            for (int i =1; i <=256; ++i) {
                BOOST_TEST_THROWS(lexical_cast<T>( to_str<CharT>(prom+i) ), bad_lexical_cast);
                BOOST_TEST_THROWS(lexical_cast<T>( to_str<CharT>(i)+s ), bad_lexical_cast);
            }
        }
    }

    if(limits::digits <= 16 && lcast_test_small_integral_types_completely)
        // min and max have already been tested.
        for(T t = 1 + min_val; t != max_val; ++t)
            BOOST_TEST(lexical_cast<T>(to_str<CharT>(t)) == t);
    else
    {
        T const half_max_val = max_val / 2;
        T const cnt = lcast_integral_test_counter; // to suppress warnings
        T const counter = cnt < half_max_val ? cnt : half_max_val;

        T t;
        T i;

        // Test values around min:
        t = min_val;
        for(i = 0; i < counter; ++i, ++t)
            BOOST_TEST(lexical_cast<T>(to_str<CharT>(t)) == t);

        // Test values around max:
        t = max_val;
        for(i = 0; i < counter; ++i, --t)
            BOOST_TEST(lexical_cast<T>(to_str<CharT>(t)) == t);

        // Test values around zero:
        if(limits::is_signed)
            for(t = static_cast<T>(-counter); t < static_cast<T>(counter); ++t)
                BOOST_TEST(lexical_cast<T>(to_str<CharT>(t)) == t);

        // Test values around 100, 1000, 10000, ...
        T ten_power = 100;
        for(int e = 2; e < limits::digits10; ++e, ten_power *= 10)
        {
            // ten_power + 100 probably never overflows
            for(t = ten_power - 100; t != ten_power + 100; ++t)
                BOOST_TEST(lexical_cast<T>(to_str<CharT>(t)) == t);
        }
    }
}

template<class T>
void test_conversion_from_to_integral_for_locale()
{
    std::locale current_locale;
    typedef std::numpunct<char> numpunct;
    numpunct const& np = BOOST_USE_FACET(numpunct, current_locale);
    if ( !np.grouping().empty() )
    {
        BOOST_TEST_THROWS(
                lexical_cast<T>( std::string("100") + np.thousands_sep() + np.thousands_sep() + "0" )
                , bad_lexical_cast);
        BOOST_TEST_THROWS(lexical_cast<T>( std::string("100") + np.thousands_sep() ), bad_lexical_cast);
        BOOST_TEST_THROWS(lexical_cast<T>( np.thousands_sep() + std::string("100") ), bad_lexical_cast);

        // Exception must not be thrown, when we are using no separators at all
        BOOST_TEST( lexical_cast<T>("30000") == static_cast<T>(30000) );
    }


    test_conversion_from_integral_to_integral<T>();

    // This is a part of test_conversion_from_integral_to_string<T>('0') method,
    // but with BOOST_TEST_EQ instead of BOOST_TEST. It is required to see
    // what is produced by the to_str<char>(t) method in situations when result
    // is different. BOOST_TEST does not work with wchar_t.
    typedef std::numeric_limits<T> limits;
    T t = (limits::min)();
    BOOST_TEST_EQ(lexical_cast<std::string>(t), to_str<char>(t));

    test_conversion_from_integral_to_string<T>('0');
    test_conversion_from_string_to_integral<T>('0');
#if !defined(BOOST_LCAST_NO_WCHAR_T)
    if (lexical_cast<std::wstring>(t) != to_str<wchar_t>(t)) {
        // Something went wrong, and now we are attempting to find and print the
        // difference.
        std::wstring wstr = to_str<wchar_t>(t);
        std::string lcast_str = lexical_cast<std::string>(t);
        std::string str;
        str.reserve(wstr.size());
        for (std::size_t i = 0; i < wstr.size(); ++i) {
            str.push_back(static_cast<char>(wstr[i]));
        }

        BOOST_TEST_EQ(lcast_str.length(), lexical_cast<std::wstring>(t).length());
        BOOST_TEST_EQ(to_str<char>(t), str);
        BOOST_TEST_EQ(lcast_str, str);
    }

    test_conversion_from_integral_to_string<T>(L'0');
    test_conversion_from_string_to_integral<T>(L'0');
#endif
}

struct restore_oldloc
{
    std::locale oldloc;
    ~restore_oldloc() { std::locale::global(oldloc); }
};

template<class T>
void test_conversion_from_to_integral_minimal()
{
    char const zero = '0';
    signed char const szero = '0';
    unsigned char const uzero = '0';
    test_conversion_from_integral_to_char<T>(zero);
    test_conversion_from_char_to_integral<T>(zero);
    test_conversion_from_integral_to_char<T>(szero);
    test_conversion_from_char_to_integral<T>(szero);
    test_conversion_from_integral_to_char<T>(uzero);
    test_conversion_from_char_to_integral<T>(uzero);
#if !defined(BOOST_LCAST_NO_WCHAR_T) && !defined(BOOST_NO_INTRINSIC_WCHAR_T)
    wchar_t const wzero = L'0';
    test_conversion_from_integral_to_char<T>(wzero);
    test_conversion_from_char_to_integral<T>(wzero);
#endif
#if !defined(BOOST_NO_CXX11_CHAR16_T) && !defined(BOOST_NO_CXX11_UNICODE_LITERALS) && !defined(_LIBCPP_VERSION) && !defined(BOOST_MSVC)
    char16_t const u16zero = u'0';
    test_conversion_from_integral_to_char<T>(u16zero);
    test_conversion_from_char_to_integral<T>(u16zero);
#endif
#if !defined(BOOST_NO_CXX11_CHAR32_T) && !defined(BOOST_NO_CXX11_UNICODE_LITERALS) && !defined(_LIBCPP_VERSION) && !defined(BOOST_MSVC)
    char32_t const u32zero = u'0';
    test_conversion_from_integral_to_char<T>(u32zero);
    test_conversion_from_char_to_integral<T>(u32zero);
#endif

    BOOST_TEST(lexical_cast<T>("-1") == static_cast<T>(-1));
    BOOST_TEST(lexical_cast<T>("-9") == static_cast<T>(-9));
    BOOST_TEST(lexical_cast<T>(-1) == static_cast<T>(-1));
    BOOST_TEST(lexical_cast<T>(-9) == static_cast<T>(-9));

    BOOST_TEST_THROWS(lexical_cast<T>("-1.0"), bad_lexical_cast);
    BOOST_TEST_THROWS(lexical_cast<T>("-9.0"), bad_lexical_cast);
    BOOST_TEST(lexical_cast<T>(-1.0) == static_cast<T>(-1));
    BOOST_TEST(lexical_cast<T>(-9.0) == static_cast<T>(-9));

    BOOST_TEST(lexical_cast<T>(static_cast<T>(1)) == static_cast<T>(1));
    BOOST_TEST(lexical_cast<T>(static_cast<T>(9)) == static_cast<T>(9));
    BOOST_TEST_THROWS(lexical_cast<T>(1.1f), bad_lexical_cast);
    BOOST_TEST_THROWS(lexical_cast<T>(1.1), bad_lexical_cast);
    BOOST_TEST_THROWS(lexical_cast<T>(1.1L), bad_lexical_cast);
    BOOST_TEST_THROWS(lexical_cast<T>(1.0001f), bad_lexical_cast);
    BOOST_TEST_THROWS(lexical_cast<T>(1.0001), bad_lexical_cast);
    BOOST_TEST_THROWS(lexical_cast<T>(1.0001L), bad_lexical_cast);

    BOOST_TEST(lexical_cast<T>("+1") == static_cast<T>(1) );
    BOOST_TEST(lexical_cast<T>("+9") == static_cast<T>(9) );
    BOOST_TEST(lexical_cast<T>("+10") == static_cast<T>(10) );
    BOOST_TEST(lexical_cast<T>("+90") == static_cast<T>(90) );
    BOOST_TEST_THROWS(lexical_cast<T>("++1"), bad_lexical_cast);
    BOOST_TEST_THROWS(lexical_cast<T>("-+9"), bad_lexical_cast);
    BOOST_TEST_THROWS(lexical_cast<T>("--1"), bad_lexical_cast);
    BOOST_TEST_THROWS(lexical_cast<T>("+-9"), bad_lexical_cast);
    // test_conversion_from_to_integral_for_locale

    // Overflow test case from David W. Birdsall
    std::string must_owerflow_str =             (sizeof(T) < 16 ?  "160000000000000000000" :  "1600000000000000000000000000000000000000");
    std::string must_owerflow_negative_str =    (sizeof(T) < 16 ? "-160000000000000000000" : "-1600000000000000000000000000000000000000");
    for (int i = 0; i < 15; ++i) {
        BOOST_TEST_THROWS(lexical_cast<T>(must_owerflow_str), bad_lexical_cast);
        BOOST_TEST_THROWS(lexical_cast<T>(must_owerflow_negative_str), bad_lexical_cast);

        must_owerflow_str += '0';
        must_owerflow_negative_str += '0';
    }
}

template<class T>
void test_conversion_from_to_integral()
{
    test_conversion_from_to_integral_minimal<T>();
    typedef std::numpunct<char> numpunct;

    restore_oldloc guard;
    std::locale const& oldloc = guard.oldloc;

    std::string grouping1 = BOOST_USE_FACET(numpunct, oldloc).grouping();
    std::string grouping2(grouping1);

    test_conversion_from_to_integral_for_locale<T>();

    try
    {
        std::locale newloc("");
        std::locale::global(newloc);

        grouping2 = BOOST_USE_FACET(numpunct, newloc).grouping();
    }
    catch(std::exception const& ex)
    {
        std::string msg("Failed to set system locale: ");
        msg += ex.what();
        std::cerr << msg;
    }

    if(grouping1 != grouping2)
        test_conversion_from_to_integral_for_locale<T>();

    if(grouping1.empty() && grouping2.empty())
        std::cerr << "Formatting with thousands_sep has not been tested";
}

void test_conversion_from_to_short()
{
    test_conversion_from_to_integral<short>();
}

void test_conversion_from_to_ushort()
{
    test_conversion_from_to_integral<unsigned short>();
}

void test_conversion_from_to_int()
{
    test_conversion_from_to_integral<int>();
}

void test_conversion_from_to_uint()
{
    test_conversion_from_to_integral<unsigned int>();
}

void test_conversion_from_to_long()
{
    test_conversion_from_to_integral<long>();
}

void test_conversion_from_to_ulong()
{
    test_conversion_from_to_integral<unsigned long>();
}

void test_conversion_from_to_intmax_t()
{
    test_conversion_from_to_integral<boost::intmax_t>();
}

void test_conversion_from_to_uintmax_t()
{
    test_conversion_from_to_integral<boost::uintmax_t>();
}

#if defined(BOOST_HAS_LONG_LONG)

void test_conversion_from_to_longlong()
{
    test_conversion_from_to_integral<boost::long_long_type>();
}

void test_conversion_from_to_ulonglong()
{
    test_conversion_from_to_integral<boost::ulong_long_type>();
}

#elif defined(BOOST_HAS_MS_INT64)

void test_conversion_from_to_longlong()
{
    test_conversion_from_to_integral<__int64>();
}

void test_conversion_from_to_ulonglong()
{
    test_conversion_from_to_integral<unsigned __int64>();
}

#endif


#ifdef BOOST_LCAST_TEST_128

template <bool Specialized, class T>
struct test_if_specialized {
    static void test() {}
};

template <class T>
struct test_if_specialized<true, T> {
    static void test() {
        test_conversion_from_to_integral_minimal<T>();
    }
};

void test_conversion_from_to_int128()
{
    test_if_specialized<
        std::numeric_limits<boost::int128_type>::is_specialized,
        boost::int128_type
    >::test();
}

void test_conversion_from_to_uint128()
{
    test_if_specialized<
        std::numeric_limits<boost::int128_type>::is_specialized,
        boost::uint128_type
    >::test();
}
#endif

template <typename SignedT>
void test_integral_conversions_on_min_max_impl()
{
    typedef SignedT signed_t;
    typedef typename boost::make_unsigned<signed_t>::type unsigned_t;

    typedef std::numeric_limits<signed_t> s_limits;
    typedef std::numeric_limits<unsigned_t> uns_limits;

    BOOST_TEST_EQ(lexical_cast<unsigned_t>((uns_limits::max)()), (uns_limits::max)());
    BOOST_TEST_EQ(lexical_cast<unsigned_t>((uns_limits::min)()), (uns_limits::min)());

    BOOST_TEST_EQ(lexical_cast<signed_t>((s_limits::max)()), (s_limits::max)());
    BOOST_TEST_EQ(lexical_cast<signed_t>((uns_limits::min)()), static_cast<signed_t>((uns_limits::min)()));

    BOOST_TEST_EQ(lexical_cast<unsigned_t>((s_limits::max)()), static_cast<unsigned_t>((s_limits::max)()));
    BOOST_TEST_EQ(lexical_cast<unsigned_t>((s_limits::min)()), static_cast<unsigned_t>((s_limits::min)()));
}

void test_integral_conversions_on_min_max()
{
    test_integral_conversions_on_min_max_impl<int>();
    test_integral_conversions_on_min_max_impl<short>();

#ifdef _MSC_VER
    test_integral_conversions_on_min_max_impl<long int>();

#if defined(BOOST_HAS_LONG_LONG)
    test_integral_conversions_on_min_max_impl<boost::long_long_type>();
#elif defined(BOOST_HAS_MS_INT64)
    test_integral_conversions_on_min_max_impl<__int64>();
#endif

#ifdef BOOST_LCAST_TEST_128
    test_integral_conversions_on_min_max_impl<boost::int128_type>();
#endif
#endif

}

void test_negative_integral() {
    // From https://github.com/boostorg/lexical_cast/issues/45
    BOOST_TEST_EQ(boost::lexical_cast<int>("-6575543"), -6575543);

    BOOST_TEST_EQ(boost::lexical_cast<int>(-6575543), -6575543);

    BOOST_TEST_EQ(boost::lexical_cast<int>("+6575543"), +6575543);
    BOOST_TEST_EQ(boost::lexical_cast<int>(6575543), 6575543);

    if (sizeof(short) == 2 && CHAR_BIT == 8) {
        BOOST_TEST_EQ(boost::lexical_cast<short>("-32768"), -32768);
        BOOST_TEST_EQ(boost::lexical_cast<short>(-32768), -32768);
        BOOST_TEST_EQ(boost::lexical_cast<unsigned short>("-32768"), 32768);
        BOOST_TEST_EQ(boost::lexical_cast<unsigned short>(-32768), 32768);

        BOOST_TEST_EQ(boost::lexical_cast<unsigned short>(65535), 65535);

        BOOST_TEST_THROWS(boost::lexical_cast<unsigned short>(-65536), bad_lexical_cast);
        BOOST_TEST_EQ(boost::lexical_cast<unsigned short>(-65535), 1);
        BOOST_TEST_EQ(boost::lexical_cast<unsigned short>(-65534), 2);

        BOOST_TEST_THROWS(boost::lexical_cast<short>(65535), bad_lexical_cast);
        BOOST_TEST_THROWS(boost::lexical_cast<short>(-65536), bad_lexical_cast);
        BOOST_TEST_THROWS(boost::lexical_cast<short>(-65535), bad_lexical_cast);
    }
}

int main()
{
    test_conversion_from_to_short();
    test_conversion_from_to_ushort();
    test_conversion_from_to_int();
    test_conversion_from_to_uint();
    test_conversion_from_to_long();
    test_conversion_from_to_ulong();
    test_conversion_from_to_intmax_t();
    test_conversion_from_to_uintmax_t();
#ifdef LCAST_TEST_LONGLONG
    test_conversion_from_to_longlong();
    test_conversion_from_to_ulonglong();
#endif
#ifdef BOOST_LCAST_TEST_128
    test_conversion_from_to_int128();
    test_conversion_from_to_uint128();
#endif
    test_integral_conversions_on_min_max();

    test_negative_integral();

    return boost::report_errors();
}

