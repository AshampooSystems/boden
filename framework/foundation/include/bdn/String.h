#pragma once

#include <algorithm>
#include <any>
#include <cctype>
#include <chrono>
#include <codecvt>
#include <locale>
#include <regex>
#include <sstream>
#include <string>

namespace bdn
{
    namespace cpp20
    {
        inline bool starts_with(const std::string &haystack, const std::string &needle)
        {
            return haystack.length() >= needle.length() && haystack.find(needle) == 0;
        }

        inline bool ends_with(const std::string &haystack, const std::string &needle)
        {
            return haystack.size() >= needle.size() &&
                   haystack.compare(haystack.size() - needle.size(), needle.size(), needle) == 0;
        }
    }

    std::string stringFromAny(const std::any &anyString);

    std::string fromUtf32(const std::u32string &utf32String);

    std::string fromUtf16(const std::u16string &utf16String);

    // TODO Test
    std::tuple<std::string_view, std::string_view> split(const std::string_view &view, std::string::value_type token);

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

    template <class _Rep, class _Period> std::string duration_to_string(std::chrono::duration<_Rep, _Period> duration);

    template <class _Rep = double, class _Period = std::ratio<1>>
    std::chrono::duration<_Rep, _Period> string_to_duration(const std::string &s)
    {
        std::regex re("([0-9\\.]+)[\\s]*(ns|us|\xC2\xB5s|ms|s|min|h)");
        std::smatch base_match;
        std::regex_match(s, base_match, re);

        if (!base_match.empty()) {
            std::string value = base_match[1];
            std::string unit = base_match[2];

            std::istringstream stream(value);
            long double v;
            stream >> v;

            if (!stream.fail()) {
                if (unit == "ns") {
                    return std::chrono::duration_cast<std::chrono::duration<_Rep, _Period>>(
                        std::chrono::duration<double, std::nano>(v));
                }
                if (unit == "us" || unit == "\xC2\xB5s") {
                    return std::chrono::duration_cast<std::chrono::duration<_Rep, _Period>>(
                        std::chrono::duration<double, std::micro>(v));
                }
                if (unit == "ms") {
                    return std::chrono::duration_cast<std::chrono::duration<_Rep, _Period>>(
                        std::chrono::duration<double, std::milli>(v));
                }
                if (unit == "s") {
                    return std::chrono::duration_cast<std::chrono::duration<_Rep, _Period>>(
                        std::chrono::duration<double>(v));
                }
                if (unit == "min") {
                    return std::chrono::duration_cast<std::chrono::duration<_Rep, _Period>>(
                        std::chrono::duration<double, std::ratio<60>>(v));
                }
                if (unit == "h") {
                    return std::chrono::duration_cast<std::chrono::duration<_Rep, _Period>>(
                        std::chrono::duration<double, std::ratio<3600>>(v));
                }
            }
        }

        throw std::invalid_argument("Couldn't convert string to duration");
    }

    namespace path // Replace with std::filesystem once its more commonly available
    {
        inline std::pair<std::string, std::string> split(const std::string &path)
        {
            auto lastSeperatorPos = path.find_last_of('/');
            if (lastSeperatorPos == std::string::npos) {
                return std::make_pair("", path);
            }

            return std::make_pair(path.substr(0, lastSeperatorPos), path.substr(lastSeperatorPos + 1));
        }

        inline std::pair<std::string, std::string> splitExt(const std::string &filename)
        {
            auto extpos = filename.find_last_of('.');
            if (extpos == std::string::npos || extpos == 0) {
                return std::make_pair(filename, "");
            }

            return std::make_pair(filename.substr(0, extpos), filename.substr(extpos + 1));
        }
    }
}
