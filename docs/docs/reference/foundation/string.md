path: tree/master/framework/ui/include/bdn/
source: String.h

# String

`String` is essentially `std::string` in Boden.

In the Boden Framework, all strings operations are based on UTF-8 `std::string`s. The `String.h` header file is provided to make working with `std::string` more convenient. It is recommended that you use `String` instead of `std::string` in your projects so future changes with regard to the evolving C++ standard will be hassle-free to implement in your apps.

## Types

```c++
namespace bdn {
    using String = std::string;
    using StringView = std::string_view;
}
```
## C++20 Polyfills

* **inline bool starts_with(const String &haystack, const String &needle)**

	Returns whether `haystack` starts with `needle`.

	Implements a polyfill emulating the [`std::basic_string::starts_with()`](https://en.cppreference.com/w/cpp/string/basic_string/starts_with) function which will be part of the C++20 standard.

* **inline bool ends_with(const String &haystack, const String &needle)**

	Returns whether `haystack` ends with `needle`.

	Implements a polyfill emulating the [`std::basic_string::ends_with()`](https://en.cppreference.com/w/cpp/string/basic_string/ends_with) function which will be part of the C++20 standard.

## Conversion Functions

* **String stringFromAny(const std::any& anyString)**

	Converts an std::any to String. Throws std::bad_any_cast if no conversion possible.

	| std::any type  | calls |
	|---|---|
	| String | String(std::any_cast<String\>(any)) |
	| json | (String)std::any_cast<json\>(any) |

## Convenience Functions

* **String fromUtf32(const std::u32string &utf32String)**

	Converts the given UTF-32 string to an UTF-8 string.

* **String fromUtf16(const std::u16string &utf16String)**

	Converts the given UTF-16 string to an UTF-8 string.

* **std::tuple<StringView, StringView> split(const StringView &view, String::value_type token)**

	Splits the given string view using the specified token as a delimiter.

* **void ltrim(std::string &s)**

	Trims whitespace from the start of the given string. The trim operation is performed in place and modifies the given string.

* **void rtrim(std::string &s)**

	Trims whitespace from the end of the given string. The trim operation is performed in place and modifies the given string.

* **void trim(std::string &s)**

	Trims whitespace from the start and end of the given string. The trim operation is performed in place and modifies the given string.

* **std::string ltrim_copy(std::string s)**

	Trims whitespace from the start of the given string. The trim operation does not modify the given string, but returns a trimmed copy instead.

* **std::string rtrim_copy(std::string s)**

	Trims whitespace from the end of the given string. The trim operation does not modify the given string, but returns a trimmed copy instead.

* **std::string trim_copy(std::string s)**

	Trims whitespace from the start and end of the given string. The trim operation does not modify the given string, but returns a trimmed copy instead.
