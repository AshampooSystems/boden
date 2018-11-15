#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/TextOutStream.h>
#include <bdn/localeUtil.h>

using namespace bdn;

template <typename STREAM_TYPE> static String _getStreamContents(STREAM_TYPE &stream)
{
    std::basic_stringbuf<char32_t, typename STREAM_TYPE::traits_type> *pBuffer =
        dynamic_cast<std::basic_stringbuf<char32_t, typename STREAM_TYPE::traits_type> *>(stream.rdbuf());

    return pBuffer->str().c_str();
}

template <typename STREAM_TYPE>
static void _verifyContents(STREAM_TYPE &stream, const String &expected, const String *pAlternativeExpected = nullptr,
                            const String *pAlternativeExpected2 = nullptr)
{
    String actual = _getStreamContents(stream);

    std::list<String> expectedList{expected};
    if (pAlternativeExpected != nullptr)
        expectedList.push_back(*pAlternativeExpected);
    if (pAlternativeExpected2 != nullptr)
        expectedList.push_back(*pAlternativeExpected2);

    REQUIRE_IN(actual, expectedList);
}

template <typename INT_TYPE, typename STREAM_TYPE> static void _verifyInteger(STREAM_TYPE &stream)
{
    INT_TYPE value;

    SECTION("min")
    value = Number<INT_TYPE>::minValue();

    SECTION("max")
    value = Number<INT_TYPE>::maxValue();

    stream << value;
    _verifyContents(stream, std::to_string(value));
}

template <typename FLOAT_TYPE, typename STREAM_TYPE> static void _verifyFloatingPoint(STREAM_TYPE &stream)
{
    FLOAT_TYPE value = 123.125;

    stream << value;
    _verifyContents(stream, "123.125");
}

template <typename STRING_TYPE, typename STREAM_TYPE> static void _verifyString(STREAM_TYPE &stream)
{
    String inString = U"he\U00012345llo";
    STRING_TYPE value = (STRING_TYPE)inString;

    SECTION("default locale (utf8)")
    {
        SECTION("lvalue")
        stream << value;

        SECTION("rvalue")
        stream << (STRING_TYPE)inString;

        _verifyContents(stream, inString);
    }

// the following test does not make sense and will not work when
// we enforce UTF-8 encoding
#ifndef BDN_OVERRIDE_LOCALE_ENCODING_UTF8

    SECTION("classic locale (ascii)")
    {
        stream.imbue(std::locale::classic());

        SECTION("lvalue")
        stream << value;

        SECTION("rvalue")
        stream << (STRING_TYPE)inString;

        // the locale only matters if a char string has been pushed in
        if (sizeof(value[0]) == 1) {
            // inString will have exported the value as UTF-8.

            // When it is written to the stream then
            // locale encoding should have been used to decode the string.

            // Since the locale encoding is ASCII, each of the UTF-8 bytes
            // in the middle should have simply been casted up to a char32.
            // Alternatively, the ascii codec may have generated an error,
            // in which case each UTF-8 byte should have been translated
            // to the unicode replacement character U+fffd.

            // MUSL libc is a special case: for some reason its ASCII
            // codec add 0xdf00 to all ascii codes >= 0x80. This is clearly
            // a bug in the standard library codec. We work around it so
            // that our tests also succeed with MUSL.

            String expected1;
            String expected2;
            String expected3;
            for (size_t i = 0; value[i] != 0; i++) {
                unsigned char chr = (unsigned char)value[i];
                expected1 += (char32_t)chr;

                if (chr >= 128) {
                    expected2 += U'\ufffd';
                    expected3 += static_cast<char32_t>(chr) + 0xdf00;
                } else {
                    expected2 += chr;
                    expected3 += chr;
                }
            }

            _verifyContents(stream, expected1, &expected2, &expected3);

        } else
            _verifyContents(stream, inString);
    }

#endif
}

template <typename CHAR_TYPE, typename STREAM_TYPE> static void _verifyCharacterImpl(STREAM_TYPE &stream)
{
    SECTION("no formatting")
    {
        SECTION("ascii")
        {
            stream << (CHAR_TYPE)'h';
            _verifyContents(stream, "h");
        }

        if (sizeof(CHAR_TYPE) >= 2 && sizeof(CHAR_TYPE) < 4) {
            SECTION("unicode")
            {
                stream << (CHAR_TYPE)U'\U00006789';
                _verifyContents(stream, U"\U00006789");
            }
        }

        if (sizeof(CHAR_TYPE) >= 4) {
            SECTION("unicode")
            {
                stream << (CHAR_TYPE)U'\U00012345';
                _verifyContents(stream, U"\U00012345");
            }
        }
    }

    SECTION("left align")
    {
        SECTION("ascii")
        {
            stream << StreamFormat().alignLeft(4) << (CHAR_TYPE)'h';
            _verifyContents(stream, "h   ");
        }

        if (sizeof(CHAR_TYPE) >= 2) {
            SECTION("unicode")
            {
                stream << StreamFormat().alignLeft(4) << (CHAR_TYPE)U'\U00006789';
                _verifyContents(stream, U"\U00006789   ");
            }
        }
    }

    SECTION("right align")
    {
        SECTION("ascii")
        {
            stream << StreamFormat().alignRight(4) << (CHAR_TYPE)'h';
            _verifyContents(stream, "   h");
        }

        if (sizeof(CHAR_TYPE) >= 2) {
            SECTION("unicode")
            {
                stream << StreamFormat().alignRight(4) << (CHAR_TYPE)U'\U00006789';
                _verifyContents(stream, U"   \U00006789");
            }
        }
    }

    SECTION("right align with different fill char")
    {
        SECTION("ascii")
        {
            stream << StreamFormat().alignRight(4, 'm') << (CHAR_TYPE)'h';
            _verifyContents(stream, "mmmh");
        }

        if (sizeof(CHAR_TYPE) >= 2) {
            SECTION("unicode")
            {
                stream << StreamFormat().alignRight(4, 'm') << (CHAR_TYPE)U'\U00006789';
                _verifyContents(stream, U"mmm\U00006789");
            }
        }
    }
}

template <typename CHAR_TYPE, typename STREAM_TYPE> static void _verifyCharacter(STREAM_TYPE &stream)
{
    SECTION("default locale (utf8)")
    _verifyCharacterImpl<CHAR_TYPE, STREAM_TYPE>(stream);

// the following test does not make sense and will not work when
// we enforce UTF-8 encoding
#ifndef BDN_OVERRIDE_LOCALE_ENCODING_UTF8

    SECTION("classic locale (ascii)")
    {
        stream.imbue(std::locale::classic());
        _verifyCharacterImpl<CHAR_TYPE, STREAM_TYPE>(stream);
    }

#endif
}

struct TestTextOutStreamSupportedDummy_
{
};

template <typename CHAR, class TRAITS>
std::basic_ostream<CHAR, TRAITS> &operator<<(std::basic_ostream<CHAR, TRAITS> &stream,
                                             const TestTextOutStreamSupportedDummy_ &x)
{
    return stream << "dummy";
}

template <typename STREAM_TYPE> static void _testTextOutStream()
{
    std::basic_stringbuf<char32_t, typename STREAM_TYPE::traits_type> streamBuffer;
    STREAM_TYPE stream(&streamBuffer);

    SECTION("default locale")
    {
        // the stream should automatically start out with the classic locale,
        // modified to use UTF-8 encoding.
        std::locale loc = stream.getloc();

        REQUIRE(isUtf8Locale(loc));
    }

    // note that the different formatting flags etc. are tested
    // by our test cases for StreamFormat. So there is no need
    // to do it here again. We only verify that the overloads work
    // as expected

    SECTION("integers")
    {
        SECTION("int8_t")
        _verifyInteger<int8_t>(stream);

        SECTION("uint8_t")
        _verifyInteger<uint8_t>(stream);

        SECTION("int16_t")
        _verifyInteger<int16_t>(stream);

        SECTION("uint16_t")
        _verifyInteger<uint16_t>(stream);

        SECTION("int32_t")
        _verifyInteger<int32_t>(stream);

        SECTION("uint32_t")
        _verifyInteger<uint32_t>(stream);

        SECTION("int64_t")
        _verifyInteger<int64_t>(stream);

        SECTION("uint64_t")
        _verifyInteger<uint64_t>(stream);

        SECTION("short")
        _verifyInteger<short>(stream);

        SECTION("unsigned short")
        _verifyInteger<unsigned short>(stream);

        SECTION("int")
        _verifyInteger<int>(stream);

        SECTION("unsigned")
        _verifyInteger<unsigned>(stream);

        SECTION("long")
        _verifyInteger<long>(stream);

        SECTION("unsigned long")
        _verifyInteger<unsigned long>(stream);

        SECTION("long long")
        _verifyInteger<long long>(stream);

        SECTION("unsigned long long")
        _verifyInteger<unsigned long long>(stream);
    }

    SECTION("floating point")
    {
        SECTION("float")
        _verifyFloatingPoint<float>(stream);

        SECTION("double")
        _verifyFloatingPoint<double>(stream);

        SECTION("long double")
        _verifyFloatingPoint<long double>(stream);
    }

    SECTION("string")
    {
        SECTION("String")
        _verifyString<String>(stream);

        SECTION("std::basic_string<char>")
        _verifyString<std::basic_string<char>>(stream);

        SECTION("std::string") // should be the same as basic_string<char> but
                               // we test it anyway
        _verifyString<std::string>(stream);

        SECTION("std::basic_string<wchar_t>")
        _verifyString<std::basic_string<wchar_t>>(stream);

        SECTION("std::wstring") // should be the same as basic_string<wchar_t>
                                // but we test it anyway
        _verifyString<std::wstring>(stream);

        SECTION("basic_string<char16_t>")
        _verifyString<std::basic_string<char16_t>>(stream);

        SECTION("basic_string<char32_t>")
        _verifyString<std::basic_string<char32_t>>(stream);

        SECTION("const char*")
        _verifyString<const char *>(stream);

        SECTION("const wchar_t*")
        _verifyString<const wchar_t *>(stream);

        SECTION("const char16_t*")
        _verifyString<const char16_t *>(stream);

        SECTION("const char32_t*")
        _verifyString<const char32_t *>(stream);
    }

    SECTION("character")
    {
        SECTION("char")
        _verifyCharacter<char>(stream);

        SECTION("wchar_t")
        _verifyCharacter<wchar_t>(stream);

        SECTION("char16_t")
        _verifyCharacter<char16_t>(stream);

        SECTION("char32_t")
        _verifyCharacter<char32_t>(stream);
    }

    SECTION("bool")
    {
        SECTION("true")
        {
            stream << (bool)true;
            _verifyContents(stream, "true");
        }

        SECTION("false")
        {
            stream << (bool)false;
            _verifyContents(stream, "false");
        }
    }

    SECTION("nullptr")
    {
        stream << nullptr;
        _verifyContents(stream, "null");
    }

    SECTION("void*")
    {
        stream << (void *)0x1234;

        std::basic_stringbuf<char32_t, typename STREAM_TYPE::traits_type> *pBuffer =
            dynamic_cast<std::basic_stringbuf<char32_t, typename STREAM_TYPE::traits_type> *>(stream.rdbuf());

        String actual = pBuffer->str().c_str();

        // the format in which pointers are printed depends on the
        // implementation. However, all known implementations print the address
        // in hex, either with or without a leading 0x. Some pad with zeros.

        // It might or might not begin with 0x
        if (actual.startsWith("0x"))
            actual = actual.subString(2);

        // then an unknown number of padding zeros follow
        while (actual.startsWith("0"))
            actual = actual.subString(1);

        // and then the address in hex
        REQUIRE(actual == "1234");
    }

    SECTION("basic_ios")
    {
        // test the stuff that was inherited from std::basic_ios.
        // note that this is necessary because the base class is a template.
        // Thus each function is only compiled when it is used. So we have to
        // use everything to ensure that it compiled with the char32_t template
        // parameter.

        // Note that it is enough to just call each method. We do not have to
        // verify every aspect of the method behaviour.

        SECTION("good")
        REQUIRE(stream.good());

        SECTION("fail")
        REQUIRE(!stream.fail());

        SECTION("bad")
        REQUIRE(!stream.bad());

        SECTION("eof")
        REQUIRE(!stream.eof());

        SECTION("operator!")
        REQUIRE(!(!stream));

        SECTION("operator bool")
        REQUIRE((bool)stream);

        SECTION("rdstate")
        REQUIRE(stream.rdstate() == std::ios_base::goodbit);

        SECTION("setstate")
        stream.setstate(stream.rdstate());

        SECTION("clear")
        stream.clear();

        SECTION("copyfmt")
        stream.copyfmt(stream);

        SECTION("fill")
        stream.fill(stream.fill());

        SECTION("exceptions")
        stream.exceptions(stream.exceptions());

        SECTION("imbue, getloc")
        stream.imbue(stream.getloc());

        SECTION("tie")
        stream.tie(stream.tie());

        SECTION("narrow")
        REQUIRE(stream.narrow(U' ', 0) == ' ');

        SECTION("widen")
        REQUIRE(stream.widen(' ') == U' ');

        // note that the stuff that was inherited from ios_base does not need to
        // be checked because ios_base is not a template. So it works
        // exactly the same for char32_t as for any other character type
    }

    SECTION("writeAnyToStream")
    {

        SECTION("int")
        {
            writeAnyToStream(stream, 1337);
            _verifyContents(stream, "1337");
        }

        SECTION("double")
        {
            writeAnyToStream(stream, 13.125);

            _verifyContents(stream, "13.125");
        }

        SECTION("const char*")
        {
            writeAnyToStream(stream, "hello");
            _verifyContents(stream, "hello");
        }

        SECTION("wstring")
        {
            writeAnyToStream(stream, std::wstring(L"hello"));
            _verifyContents(stream, "hello");
        }

        SECTION("String")
        {
            writeAnyToStream(stream, String("hello"));
            _verifyContents(stream, "hello");
        }

        SECTION("supported object")
        {
            TestTextOutStreamSupportedDummy_ d;

            writeAnyToStream(stream, d);

            _verifyContents(stream, "dummy");
        }

        SECTION("unsupported object")
        {
            struct Dummy
            {
            };

            Dummy d;

            writeAnyToStream(stream, d);

            std::basic_stringbuf<char32_t, UnicodeCharTraits> tempBuffer;
            TextOutStream tempStream(&tempBuffer);

            tempStream << (void *)&d;

            String address = _getStreamContents(tempStream);

            _verifyContents(stream, String("<") + typeid(d).name() + " @ " + address + ">");
        }
    }
}

TEST_CASE("TextOutStream")
{
    // TextOutStream is an alias to basic_ostream<char32_t, UnicodeCharTraits>.
    // We do all tests with each type name, just to verify that both work
    // exactly the same. Note that since they are in different namespaces there
    // is potential for overloading rules to work differently (although they
    // should not).

    SECTION("basic_ostream")
    _testTextOutStream<std::basic_ostream<char32_t, UnicodeCharTraits>>();

    SECTION("TextOutStream")
    _testTextOutStream<TextOutStream>();
}
