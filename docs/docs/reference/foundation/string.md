# String

Convenience functions and types for working with `std::string` in Boden.

## Types

    using String = std::string;
    using StringView = std::string_view;

## C++20 Polyfills

* **inline bool starts_with(const String &haystack, const String &needle)**

* **inline bool ends_with(const String &haystack, const String &needle)**

## Convenience Functions

* **String fromUtf32(const std::u32string &utf32String)**

* **String fromUtf16(const std::u16string &utf16String)**

* **std::tuple<StringView, StringView> split(const StringView &view, String::value_type token)**

* **void ltrim(std::string &s)**

* **void rtrim(std::string &s)**

* **void trim(std::string &s)**

* **std::string ltrim_copy(std::string s)**

* **std::string rtrim_copy(std::string s)**

* **std::string trim_copy(std::string s)**
