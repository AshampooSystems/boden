#pragma once

#include <string>
#include <locale>
#include <codecvt>
#include <algorithm>
#include <cctype>

namespace bdn
{
    using String = std::string;
    using StringView = std::string_view;

    namespace cpp20
    {
        inline bool starts_with(const String &haystack, const String &needle)
        {
            return haystack.length() >= needle.length() && haystack.find(needle) == 0;
        }

        inline bool ends_with(const String &haystack, const String &needle)
        {
            if (haystack.size() >= needle.size() &&
                haystack.compare(haystack.size() - needle.size(), needle.size(), needle) == 0) {
                return true;
            } else {
                return false;
            }
        }
    }

    String fromUtf32(const std::u32string &utf32String);

    String fromUtf16(const std::u16string &utf16String);

    // TODO Test
    std::tuple<StringView, StringView> split(const StringView &view, String::value_type token);

    // trim from start (in place)
    void ltrim(std::string &s);

    // trim from end (in place)
    void rtrim(std::string &s);

    // trim from both ends (in place)
    void trim(std::string &s);

    // trim from start (copying)
    std::string ltrim_copy(std::string s);

    // trim from end (copying)
    std::string rtrim_copy(std::string s);

    // trim from both ends (copying)
    std::string trim_copy(std::string s);
}