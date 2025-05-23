// Copyright Antony Polukhin, 2013-2025.

// Distributed under the Boost Software License, Version 1.0.
// (See the accompanying file LICENSE_1_0.txt
// or a copy at <http://www.boost.org/LICENSE_1_0.txt>.)

#include <boost/lexical_cast.hpp>

#include <array>
#include <string>
#include <cstdio>

#ifdef BOOST_MSVC
#  pragma warning(disable: 4996) // `strerror` is not safe
#endif

//[lexical_cast_log_errno
//`The following example uses numeric data in a string expression:

void log_message(const std::string &);

void log_errno(int yoko)
{
    log_message("Error " + boost::lexical_cast<std::string>(yoko) + ": " + strerror(yoko));
}

//] [/lexical_cast_log_errno]


//[lexical_cast_fixed_buffer
//`The following example converts some number and puts it to file:

void number_to_file(int number, std::FILE* file)
{
    using buf_t = std::array<char, 50>;
    buf_t buffer = boost::lexical_cast<buf_t>(number); // No dynamic memory allocation
    std::fputs(buffer.data(), file);
}

//] [/lexical_cast_fixed_buffer]

//[lexical_cast_substring_conversion
//`The following example takes part of the string and converts it to `int`:

int convert_strings_part(const std::string& s, std::size_t pos, std::size_t n)
{
    return boost::lexical_cast<int>(s.data() + pos, n);
}

//] [/lexical_cast_substring_conversion]

void log_message(const std::string &) {}

int main()
{
    return 0;
}

