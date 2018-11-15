#include <bdn/init.h>
#include <bdn/test.h>

#include <iostream>

// GCC 4.8 does not have the standard codecvt header.
#if !defined(__GNUC__) || __GNUC__ >= 5
#include <codecvt>
#endif

using namespace bdn;

String getStreamString(std::ostringstream &stream) { return String::fromLocaleEncoding(stream.str(), stream.getloc()); }

String getStreamString(std::wostringstream &stream) { return String(stream.str()); }

template <class StreamType, class StringType> void writeStringToStream(StreamType &stream, StringType in)
{
    stream << in;
}

template <> void writeStringToStream<std::ostringstream, std::string>(std::ostringstream &stream, std::string in)
{
    // this is a special case. The stream will assume that a std::string is in
    // its locale encoding, but ours is in UTF-8. We need to pass it in in
    // locale encoding.
    stream << String(in).toLocaleEncoding(stream.getloc());
}

template <> void writeStringToStream<std::ostringstream, const char *>(std::ostringstream &stream, const char *in)
{
    // this is a special case. The stream will assume that a const char* is in
    // its locale encoding, but ours is in UTF-8. We need to pass it in in
    // locale encoding.
    stream << String(in).toLocaleEncoding(stream.getloc());
}

template <class StreamType, class StringType> inline void testStreamStringOutput(bool useUtf8Locale)
{
    String inObj(U"he\U00002345llo");
    StringType in = (StringType)inObj;

    StreamType stream;

    if (useUtf8Locale) {
        stream.imbue(deriveUtf8Locale(std::locale::classic()));
    }

    writeStringToStream(stream, in);

    String result = getStreamString(stream);

    // we expect an exact result if we write the same chars
    // that the string uses internally.
    // The only exception is if the locale does not use utf8 encoding
    // and the stream uses char internally. Then the data is written 1:1 to the
    // stream, but when we read it back it will be interpreted according to the
    // locale. Note that when we write char strings to a wide char stream then
    // each char is individually widened so we also do not get the correct
    // result.
    bool expectExactResult = typeid(typename StreamType::char_type) == typeid(in[0]);
    if (!useUtf8Locale && typeid(typename StreamType::char_type) == typeid(char))
        expectExactResult = false;

    // we also always expect an exact result if the input type is String
    // and the stream type is either utf-8 char or wchar_t
    if (typeid(StringType) == typeid(String) &&
        (typeid(typename StreamType::char_type) != typeid(char) || useUtf8Locale)) {
        expectExactResult = true;
    }

    if (expectExactResult) {
        // must match exactly
        REQUIRE(result == inObj);
    } else {
        REQUIRE(result.startsWith("he"));
        REQUIRE(result.endsWith("llo"));
    }
}

template <class StreamType, class StringType> inline void testStreamStringOutput()
{
    SECTION("utf8Encoding")
    testStreamStringOutput<StreamType, StringType>(true);

    SECTION("defaultEncoding")
    testStreamStringOutput<StreamType, StringType>(false);
}

template <class StreamType> inline void testStreamOutput()
{
    SECTION("String")
    testStreamStringOutput<StreamType, String>();

    // const char* is supported by all stream types, no matter what their
    // internal character encoding is
    SECTION("const char*")
    testStreamStringOutput<StreamType, const char *>();

    SECTION("std::basic_string<CharT>")
    testStreamStringOutput<StreamType, std::basic_string<typename StreamType::char_type>>();

    SECTION("const CharT*")
    testStreamStringOutput<StreamType, const typename StreamType::char_type *>();
}

TEST_CASE("std stream string output")
{
    SECTION("ostringstream")
    testStreamOutput<std::ostringstream>();

    SECTION("wostringstream")
    testStreamOutput<std::wostringstream>();
}
