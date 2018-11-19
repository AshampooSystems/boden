#include <bdn/init.h>

#include <bdn/StringBuffer.h>

#include <bdn/test.h>

#include <random>

using namespace bdn;

template <class STRING_BUF>
static void _verifyIteration(STRING_BUF &buf, const String &expected, const String &sectionPrefix)
{
    String::Iterator expectedBegin = expected.begin();
    String::Iterator expectedEnd = expected.end();
    auto beginIt = buf.begin();
    auto endIt = buf.end();

    SECTION(sectionPrefix + "self equality")
    {
        checkEquality(beginIt, buf.begin(), true);
        checkEquality(endIt, buf.end(), true);
    }

    SECTION(sectionPrefix + "forward")
    {
        auto it = beginIt;
        auto expectedIt = expectedBegin;

        while (it != endIt) {
            checkEquality(it, endIt, false);
            checkEquality(expectedIt, expectedEnd, false);

            REQUIRE(*it == *expectedIt);

            ++it;
            ++expectedIt;
        }

        checkEquality(it, endIt, true);
        checkEquality(expectedIt, expectedEnd, true);
    }

    SECTION(sectionPrefix + "backward")
    {
        auto it = endIt;
        auto expectedIt = expectedEnd;

        while (it != beginIt) {
            checkEquality(it, beginIt, false);
            checkEquality(expectedIt, expectedBegin, false);

            --it;
            --expectedIt;

            REQUIRE(*it == *expectedIt);
        }

        checkEquality(it, beginIt, true);
        checkEquality(expectedIt, expectedBegin, true);
    }

    SECTION(sectionPrefix + "random")
    {
        auto it = beginIt;
        auto expectedIt = expectedBegin;

        int forwardCount = 0;
        int backwardCount = 0;

        std::random_device randDevice;
        std::mt19937 randGen(randDevice());
        std::uniform_int_distribution<> randDis(0, 1);

        for (int step = 0; step < 100; step++) {
            bool forward = randDis(randGen) == 0;
            bool copy = randDis(randGen) == 0;

            if (copy) {
                auto other = it;
                it = other;
            }

            if (forward) {
                forwardCount++;

                if (it != endIt) {
                    REQUIRE(expectedIt != expectedEnd);

                    ++it;
                    ++expectedIt;
                }
            } else {
                backwardCount++;

                if (it != beginIt) {
                    REQUIRE(expectedIt != expectedBegin);

                    --it;
                    --expectedIt;
                }
            }

            bool atEnd = (it == endIt);
            bool atBegin = (it == beginIt);

            checkEquality(it, endIt, atEnd);
            checkEquality(expectedIt, expectedEnd, atEnd);

            checkEquality(it, beginIt, atBegin);
            checkEquality(expectedIt, expectedBegin, atBegin);

            if (!atEnd)
                REQUIRE(*it == *expectedIt);
        }

        // sanity check - see if test behaved as expected
        REQUIRE(forwardCount > 0);
        REQUIRE(backwardCount > 0);
    }
}

static void _verifyResultImpl(StringBuffer &buf, const String &expected, const String &sectionPrefix = "")
{
    SECTION(sectionPrefix + "normal iteration")
    _verifyIteration<StringBuffer>(buf, expected, sectionPrefix);

    SECTION(sectionPrefix + "const iteration")
    _verifyIteration<const StringBuffer>(buf, expected, sectionPrefix);

    SECTION(sectionPrefix + "string")
    {
        String actual = buf.toString();
        REQUIRE(actual == expected);
    }
}

static void _verifyResult(StringBuffer &buf, const String &expected)
{
    SECTION("direct")
    _verifyResultImpl(buf, expected);

    SECTION("after move construct")
    {
        auto innerBufferBefore = buf.rdbuf();

        StringBuffer other(std::move(buf));

        // buffer object have been invalidated on the source object
        REQUIRE(buf.rdbuf() == nullptr);

        // the target's buffer should be valid,
        // but NOT the one from the source object.
        REQUIRE(other.rdbuf() != nullptr);
        REQUIRE(other.rdbuf() != innerBufferBefore);

        // now other should have the data.

        // Note that since we call the same function twice in the same section
        // we HAVE to pass a different section name prefix for each call.
        // Otherwise the section tracker will get confused and issue an
        // assertion.
        _verifyResultImpl(other, expected, "target buffer: ");
        _verifyResultImpl(buf, "", "source buffer: ");

        // also verify that we can still write numbers (i.e. stuff whose
        // formatting depends on the locale).
        other << 1234;
        _verifyResultImpl(other, expected + "1234", "target stream after another write: ");
    }

    SECTION("after move assign")
    {
        auto innerBufferBefore = buf.rdbuf();

        StringBuffer other;

        // sanity check: verify that this works before we make the move

        _verifyResultImpl(buf, expected, "pretest source: ");
        _verifyResultImpl(other, "", "pretest target: ");

        auto otherInnerBufferBefore = other.rdbuf();

        other = std::move(buf);

        // buffer object should NOT have been moved
        REQUIRE(buf.rdbuf() == innerBufferBefore);
        REQUIRE(other.rdbuf() == otherInnerBufferBefore);

        // now other should have the data
        _verifyResultImpl(other, expected, "target: ");

        // and the initial buffer should be empty
        _verifyResultImpl(buf, "", "source: ");

        // also verify that we can still write numbers (i.e. stuff whose
        // formatting depends on the locale).
        other << 1234;
        _verifyResultImpl(other, expected + "1234", "target after another write: ");
    }
}

static void _verifyResult(StringBuffer &&buf, const String &expected) { _verifyResult(buf, expected); }

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-function"
#endif

static void _verifyResult(TextOutStream &stream, const String &expected)
{
    _verifyResult(*dynamic_cast<StringBuffer *>(&stream), expected);
}

#ifdef __clang__
#pragma clang diagnostic pop
#endif

std::u32string _readStreamBufferContents(StringBuffer &buf)
{
    std::basic_streambuf<char32_t, UnicodeCharTraits> *buffer = buf.rdbuf();

    char32_t tempBuffer[100];

    std::u32string result;
    std::streamsize charsRead;
    do {
        charsRead = buffer->sgetn(tempBuffer, 100);
        result += std::u32string(tempBuffer, (int)charsRead);
    } while (charsRead == 100);

    return result;
}

static void _verifySeekResult(bool readSeek, StringBuffer &buf,
                              std::basic_streambuf<char32_t, UnicodeCharTraits> *buffer, const String &contents,
                              std::streamoff toIndex, std::streamoff result)
{
    if (toIndex < 0 || (size_t)toIndex > contents.length()) {
        REQUIRE(result == -1);
    } else {
        REQUIRE(result == toIndex);

        if (readSeek) {
            for (int i = 0; i < 3; i++) {
                UnicodeCharTraits::int_type expected;

                if ((size_t)toIndex >= contents.length())
                    expected = UnicodeCharTraits::eof();
                else
                    expected = UnicodeCharTraits::to_int_type(contents[(size_t)toIndex]);

                REQUIRE(buffer->sbumpc() == expected);

                toIndex++;
            }
        } else {
            String expected = contents;

            String::Iterator beginIt = expected.begin() + (ptrdiff_t)toIndex;
            String::Iterator endIt = beginIt;

            for (int i = 0; i < 3; i++) {
                if (endIt != expected.end())
                    ++endIt;

                buffer->sputc('x' + i);
            }

            expected.replace(beginIt, endIt, "xyz");

            String actual = buf.toString();

            REQUIRE(actual == expected);
        }
    }
}

static void _verifyGenericSeek(StringBuffer &buf, const String &contents, std::ios_base::openmode which, bool readSeek)
{
    std::basic_streambuf<char32_t, UnicodeCharTraits> *buffer = buf.rdbuf();

    size_t length = contents.length();

    struct SeekPositionData_
    {
        std::streamoff index;
        String desc;
    };

    List<SeekPositionData_> posDataList{{{-1, "begin-1"},
                                         {0, "begin"},
                                         {1, "begin+1"},
                                         {(std::streamoff)contents.length() / 2 - 1, "middle-1"},
                                         {(std::streamoff)contents.length() / 2, "middle"},
                                         {(std::streamoff)contents.length() / 2 + 1, "middle+1"},
                                         {(std::streamoff)contents.length() - 1, "end-1"},
                                         {(std::streamoff)contents.length(), "end"},
                                         {(std::streamoff)contents.length() + 1, "end+1"}}};

    for (SeekPositionData_ &fromPosData : posDataList) {
        if (fromPosData.index < 0 || (size_t)fromPosData.index > contents.length())
            continue;

        SECTION("from " + fromPosData.desc)
        {
            std::streampos fromPos = buffer->pubseekpos(fromPosData.index, which);
            REQUIRE(fromPos == fromPosData.index);

            for (SeekPositionData_ &toPosData : posDataList) {
                SECTION("to " + toPosData.desc)
                {
                    SECTION("anchor=begin")
                    {
                        SECTION("seekoff")
                        {
                            _verifySeekResult(readSeek, buf, buffer, contents, toPosData.index,
                                              buffer->pubseekoff(toPosData.index, std::ios_base::beg, which));
                        }

                        SECTION("seekpos")
                        {
                            _verifySeekResult(readSeek, buf, buffer, contents, toPosData.index,
                                              buffer->pubseekpos(toPosData.index, which));
                        }
                    }

                    // anchor=cur is not supported if in and out positions are
                    // changed at the same time
                    if (which != (std::ios_base::in | std::ios_base::out)) {
                        SECTION("anchor=cur")
                        {
                            _verifySeekResult(
                                readSeek, buf, buffer, contents, toPosData.index,
                                buffer->pubseekoff(toPosData.index - fromPosData.index, std::ios_base::cur, which));
                        }
                    }

                    SECTION("anchor=end")
                    {
                        _verifySeekResult(
                            readSeek, buf, buffer, contents, toPosData.index,
                            buffer->pubseekoff(toPosData.index - contents.length(), std::ios_base::end, which));
                    }
                }
            }
        }
    }
}

static void _verifyReadSeek(StringBuffer &buf, const String &contents, std::ios_base::openmode which)
{
    _verifyGenericSeek(buf, contents, which, true);

    if (which == std::ios_base::in) {
        std::basic_streambuf<char32_t, UnicodeCharTraits> *buffer = buf.rdbuf();
        size_t length = contents.length();

        SECTION("sequenced seek")
        {
            std::streamoff pos = buffer->pubseekoff(0, std::ios_base::end, which);
            REQUIRE(pos == length);

            REQUIRE(buffer->sgetc() == UnicodeCharTraits::eof());
            REQUIRE(buffer->sbumpc() == UnicodeCharTraits::eof());

            pos = buffer->pubseekoff(-2, std::ios_base::end, which);
            REQUIRE(pos == length - 2);

            REQUIRE(buffer->sbumpc() == contents[length - 2]);
            REQUIRE(buffer->sbumpc() == contents[length - 1]);

            // anchor=cur is not supported if in and out positions are
            // changed at the same time
            if (which != (std::ios_base::in | std::ios_base::out))
                pos = buffer->pubseekoff(-(long)(length / 2) - 1, std::ios_base::cur, which);
            else
                pos = buffer->pubseekoff(-(long)(length / 2) - 1, std::ios_base::end, which);
            REQUIRE(pos == length - length / 2 - 1);

            REQUIRE(buffer->sbumpc() == contents[length - length / 2 - 1]);
            REQUIRE(buffer->sbumpc() == contents[length - length / 2]);
            REQUIRE(buffer->sbumpc() == contents[length - length / 2 + 1]);

            pos = buffer->pubseekoff(-(long)(length / 2), std::ios_base::end, which);
            REQUIRE(pos == length - length / 2);

            REQUIRE(buffer->sbumpc() == contents[length - length / 2]);
            REQUIRE(buffer->sbumpc() == contents[length - length / 2 + 1]);

            pos = buffer->pubseekoff(0, std::ios_base::beg, which);
            REQUIRE(pos == 0);

            REQUIRE(buffer->sbumpc() == contents[0]);
            REQUIRE(buffer->sbumpc() == contents[1]);

            pos = buffer->pubseekoff(length / 2, std::ios_base::beg, which);
            REQUIRE(pos == length / 2);

            REQUIRE(buffer->sbumpc() == contents[length / 2]);
            REQUIRE(buffer->sbumpc() == contents[length / 2 + 1]);
        }
    }
}

static void _verifyWriteSeek(StringBuffer &buf, const String &contents, std::ios_base::openmode which)
{
    _verifyGenericSeek(buf, contents, which, false);
}

static void _verifySeek(StringBuffer &buf, const String &contents)
{
    SECTION("read")
    {
        SECTION("which = in")
        _verifyReadSeek(buf, contents, std::ios_base::in);

        SECTION("which = in | out")
        _verifyReadSeek(buf, contents, std::ios_base::in | std::ios_base::out);
    }

    SECTION("write")
    {
        SECTION("which = out")
        _verifyWriteSeek(buf, contents, std::ios_base::out);

        SECTION("which = in | out")
        _verifyReadSeek(buf, contents, std::ios_base::in | std::ios_base::out);
    }
}

class WithStreamOperator_
{
  public:
};

template <typename CHAR_TYPE, class CHAR_TRAITS>
std::basic_ostream<CHAR_TYPE, CHAR_TRAITS> &operator<<(std::basic_ostream<CHAR_TYPE, CHAR_TRAITS> &stream,
                                                       const WithStreamOperator_ &o)
{
    return stream << "ok";
}

class WithStreamOperatorInBaseClass_ : public WithStreamOperator_
{
};

class WithStreamOperatorOverloaded_
{
  public:
};

template <typename CHAR_TYPE, class CHAR_TRAITS>
std::basic_ostream<CHAR_TYPE, CHAR_TRAITS> &operator<<(std::basic_ostream<CHAR_TYPE, CHAR_TRAITS> &stream,
                                                       WithStreamOperatorOverloaded_ &o)
{
    return stream << "non const";
}

template <typename CHAR_TYPE, class CHAR_TRAITS>
std::basic_ostream<CHAR_TYPE, CHAR_TRAITS> &operator<<(std::basic_ostream<CHAR_TYPE, CHAR_TRAITS> &stream,
                                                       const WithStreamOperatorOverloaded_ &o)
{
    return stream << "const";
}

class NoStreamOperator_
{
};

template <typename VALUE_TYPE>
void _verifyStringify(VALUE_TYPE value, const String &expectedString, bool expectedIsPrefixOnly = false)
{
    String actualString;

    SECTION("toString")
    actualString = toString<VALUE_TYPE>(std::forward<VALUE_TYPE>(value));

    SECTION("anyToString")
    actualString = anyToString<VALUE_TYPE>(std::forward<VALUE_TYPE>(value));

    if (expectedIsPrefixOnly)
        REQUIRE(actualString.startsWith(expectedString));
    else
        REQUIRE(actualString == expectedString);
}

template <typename VALUE_TYPE> void _verifyStringifyUnsupportedObject(VALUE_TYPE value, bool prefixOnly = false)
{
    String address = toString((const void *)&value);

    String expectedPrefix = String("<") + typeid(typename std::decay<VALUE_TYPE>::type).name() + " @ ";
    String expectedWithAddress = expectedPrefix + address + ">";

    // note that we xannot test toString here, since that would result
    // in a compiler error. So we can only check anyToString

    SECTION("anyToString")
    {
        String actualString = anyToString(std::forward<VALUE_TYPE>(value));

        if (prefixOnly)
            REQUIRE(actualString.startsWith(expectedPrefix));
        else
            REQUIRE(actualString == expectedWithAddress);
    }
}

template <bool supported> struct VerifyStringifyObject_
{
    template <class VALUE_TYPE> static void verify(VALUE_TYPE value, const String &expectedResult)
    {
        _verifyStringify<VALUE_TYPE>(value, expectedResult);
    }
};

template <> struct VerifyStringifyObject_<false>
{
    template <class VALUE_TYPE> static void verify(VALUE_TYPE value, const String &expectedResult)
    {
        _verifyStringifyUnsupportedObject<VALUE_TYPE>(value, !std::is_reference<VALUE_TYPE>::value);
    }
};

template <class CLS, bool CONST_SUPPORTED, bool NON_CONST_SUPPORTED>
void _verifyStringifyObject(const String &expectedConstResult, const String &expectedNonConstResult)
{
    CLS obj;

    SECTION("value")
    VerifyStringifyObject_<CONST_SUPPORTED>::template verify<CLS>(obj, expectedConstResult);

    SECTION("reference")
    VerifyStringifyObject_<NON_CONST_SUPPORTED>::template verify<CLS &>(obj, expectedNonConstResult);

    SECTION("const value")
    VerifyStringifyObject_<CONST_SUPPORTED>::template verify<const CLS>(obj, expectedConstResult);

    SECTION("const reference")
    VerifyStringifyObject_<CONST_SUPPORTED>::template verify<const CLS &>(obj, expectedConstResult);
}

void _testStringifyObject()
{
    SECTION("with << operator")
    _verifyStringifyObject<WithStreamOperator_, true, true>("ok", "ok");

    SECTION("with << operator for base class")
    _verifyStringifyObject<WithStreamOperatorInBaseClass_, true, true>("ok", "ok");

    SECTION("overloaded << operator")
    _verifyStringifyObject<WithStreamOperatorOverloaded_, true, true>("const", "non const");

    SECTION("no << operator ")
    _verifyStringifyObject<NoStreamOperator_, false, false>("", "");
}

template <typename CHAR_TYPE> void _verifyStringifyChar()
{
    typename std::decay<CHAR_TYPE>::type chr = 'X';

    SECTION("value")
    _verifyStringify<CHAR_TYPE>(chr, "X");

    SECTION("const value")
    _verifyStringify<const CHAR_TYPE>(chr, "X");

    SECTION("reference")
    _verifyStringify<CHAR_TYPE &>(chr, "X");

    SECTION("const reference")
    _verifyStringify<const CHAR_TYPE &>(chr, "X");
}

void _testStringifyChars()
{
    SECTION("char")
    _verifyStringifyChar<char>();

    SECTION("wchar_t")
    _verifyStringifyChar<wchar_t>();

    SECTION("char16_t")
    _verifyStringifyChar<char16_t>();

    SECTION("char32_t")
    _verifyStringifyChar<char32_t>();
}

template <typename INT_TYPE> void _verifyStringifyIntWithExactType()
{
    typename std::decay<INT_TYPE>::type val;

    SECTION("0")
    {
        val = 0;
        _verifyStringify<INT_TYPE>(val, "0");
    }

    SECTION("max")
    {
        val = Number<decltype(val)>::maxValue();

        String expected = std::to_string(val);
        _verifyStringify<INT_TYPE>(val, expected);
    }

    SECTION("min")
    {
        val = Number<decltype(val)>::minValue();

        String expected = std::to_string(val);
        _verifyStringify<INT_TYPE>(val, expected);
    }
}

template <typename INT_TYPE> void _verifyStringifyInt()
{
    SECTION("value")
    _verifyStringifyIntWithExactType<INT_TYPE>();

    SECTION("const value")
    _verifyStringifyIntWithExactType<const INT_TYPE>();

    SECTION("reference")
    _verifyStringifyIntWithExactType<INT_TYPE &>();

    SECTION("const reference")
    _verifyStringifyIntWithExactType<const INT_TYPE &>();
}

void _testStringifyIntegers()
{
    SECTION("int8_t")
    _verifyStringifyInt<int8_t>();

    SECTION("uint8_t")
    _verifyStringifyInt<uint8_t>();

    SECTION("int16_t")
    _verifyStringifyInt<int16_t>();

    SECTION("uint16_t")
    _verifyStringifyInt<uint16_t>();

    SECTION("int32_t")
    _verifyStringifyInt<int32_t>();

    SECTION("uint32_t")
    _verifyStringifyInt<uint32_t>();

    SECTION("int64_t")
    _verifyStringifyInt<int64_t>();

    SECTION("uint64_t")
    _verifyStringifyInt<uint64_t>();

    SECTION("int")
    _verifyStringifyInt<int>();

    SECTION("unsigned")
    _verifyStringifyInt<unsigned>();

    SECTION("long")
    _verifyStringifyInt<long>();

    SECTION("unsigned long")
    _verifyStringifyInt<unsigned long>();

    SECTION("long long")
    _verifyStringifyInt<long long>();

    SECTION("unsigned long")
    _verifyStringifyInt<unsigned long long>();

    SECTION("intptr_t")
    _verifyStringifyInt<intptr_t>();

    SECTION("uintptr_t")
    _verifyStringifyInt<uintptr_t>();

    SECTION("size_t")
    _verifyStringifyInt<size_t>();
}

template <typename BOOL_TYPE> void _verifyStringifyBoolWithExactType()
{
    bool val;

    SECTION("true")
    {
        val = true;
        _verifyStringify<BOOL_TYPE>(val, "true");
    }

    SECTION("false")
    {
        val = false;
        _verifyStringify<BOOL_TYPE>(val, "false");
    }
}

void _testStringifyBool()
{
    SECTION("value")
    _verifyStringifyBoolWithExactType<bool>();

    SECTION("const value")
    _verifyStringifyBoolWithExactType<const bool>();

    SECTION("reference")
    _verifyStringifyBoolWithExactType<bool &>();

    SECTION("const reference")
    _verifyStringifyBoolWithExactType<const bool &>();
}

void _testStringifyNullptr()
{
    std::nullptr_t p = nullptr;

    SECTION("value")
    _verifyStringify<std::nullptr_t>(p, "null");

    SECTION("const value")
    _verifyStringify<const std::nullptr_t>(p, "null");

    SECTION("reference")
    _verifyStringify<std::nullptr_t &>(p, "null");

    SECTION("const reference")
    _verifyStringify<const std::nullptr_t &>(p, "null");
}

template <typename FLOAT_TYPE> void _verifyStringifyFloatingPointVariants(FLOAT_TYPE val, const String &expected)
{
    SECTION("value")
    _verifyStringify<FLOAT_TYPE>(val, expected);

    SECTION("const value")
    _verifyStringify<const FLOAT_TYPE>(val, expected);

    SECTION("reference")
    _verifyStringify<FLOAT_TYPE &>(val, expected);

    SECTION("const reference")
    _verifyStringify<const FLOAT_TYPE &>(val, expected);
}

template <typename FLOAT_TYPE> void _verifyStringifyFloatingPoint()
{
    SECTION("0")
    _verifyStringifyFloatingPointVariants<FLOAT_TYPE>(0, "0");

    SECTION("1")
    _verifyStringifyFloatingPointVariants<FLOAT_TYPE>(1, "1");

    SECTION("-1")
    _verifyStringifyFloatingPointVariants<FLOAT_TYPE>(-1, "-1");

    SECTION("0.5")
    _verifyStringifyFloatingPointVariants<FLOAT_TYPE>(0.5, "0.5");

    SECTION("-0.5")
    _verifyStringifyFloatingPointVariants<FLOAT_TYPE>(-0.5, "-0.5");

    SECTION("0.125")
    _verifyStringifyFloatingPointVariants<FLOAT_TYPE>(0.125, "0.125");

    SECTION("-0.125")
    _verifyStringifyFloatingPointVariants<FLOAT_TYPE>(-0.125, "-0.125");

    SECTION("max")
    {
        FLOAT_TYPE val = Number<FLOAT_TYPE>::maxValue();

        std::stringstream strm;
        strm.imbue(std::locale::classic());
        strm << val;

        String expected = strm.str();
        // sanity check
        REQUIRE(expected.length() >= 8);
        REQUIRE(expected.contains("."));
        REQUIRE(expected.contains("e"));

        _verifyStringifyFloatingPointVariants<FLOAT_TYPE>(val, expected);
    }

    SECTION("min")
    {
        FLOAT_TYPE val = Number<FLOAT_TYPE>::minValue();

        std::stringstream strm;
        strm.imbue(std::locale::classic());
        strm << val;

        String expected = strm.str();
        // sanity check
        REQUIRE(expected.length() >= 8);
        REQUIRE(expected.contains("."));
        REQUIRE(expected.contains("e"));
        REQUIRE(expected.startsWith("-"));

        _verifyStringifyFloatingPointVariants<FLOAT_TYPE>(val, expected);
    }
}

void _testStringifyFloatingPoint()
{
    SECTION("float")
    _verifyStringifyFloatingPoint<float>();

    SECTION("double")
    _verifyStringifyFloatingPoint<double>();

    SECTION("long double")
    _verifyStringifyFloatingPoint<long double>();
}

template <typename STRING_TYPE> STRING_TYPE makeStringOfType(const String &val)
{
    return static_cast<STRING_TYPE>(val);
}

template <> char *makeStringOfType<char *>(const String &val)
{
    const char *s = makeStringOfType<const char *>(val);
    return const_cast<char *>(s);
}

template <> wchar_t *makeStringOfType<wchar_t *>(const String &val)
{
    const wchar_t *s = makeStringOfType<const wchar_t *>(val);
    return const_cast<wchar_t *>(s);
}

template <> char16_t *makeStringOfType<char16_t *>(const String &val)
{
    const char16_t *s = makeStringOfType<const char16_t *>(val);
    return const_cast<char16_t *>(s);
}

template <> char32_t *makeStringOfType<char32_t *>(const String &val)
{
    const char32_t *s = makeStringOfType<const char32_t *>(val);
    return const_cast<char32_t *>(s);
}

template <> StringImpl<Utf8StringData> makeStringOfType<StringImpl<Utf8StringData>>(const String &val)
{
    return StringImpl<Utf8StringData>(val.begin(), val.end());
}

template <> StringImpl<Utf16StringData> makeStringOfType<StringImpl<Utf16StringData>>(const String &val)
{
    return StringImpl<Utf16StringData>(val.begin(), val.end());
}

template <> StringImpl<Utf32StringData> makeStringOfType<StringImpl<Utf32StringData>>(const String &val)
{
    return StringImpl<Utf32StringData>(val.begin(), val.end());
}

template <> StringImpl<WideStringData> makeStringOfType<StringImpl<WideStringData>>(const String &val)
{
    return StringImpl<WideStringData>(val.begin(), val.end());
}

template <class STRING_TYPE> void _verifyStringifyStringValue(const String &val)
{
    STRING_TYPE s = makeStringOfType<STRING_TYPE>(val);

    SECTION("value")
    _verifyStringify<STRING_TYPE>(s, val);

    SECTION("const value")
    _verifyStringify<const STRING_TYPE>(s, val);

    SECTION("reference")
    _verifyStringify<STRING_TYPE &>(s, val);

    SECTION("const reference")
    _verifyStringify<const STRING_TYPE &>(s, val);
}

template <class STRING_CLASS> void _verifyStringifyString()
{
    SECTION("empty")
    _verifyStringifyStringValue<STRING_CLASS>("");

    SECTION("ascii")
    _verifyStringifyStringValue<STRING_CLASS>("hello");

    SECTION("unicode")
    _verifyStringifyStringValue<STRING_CLASS>(U"hel\U00012345lo");
}

void _testStringifyString()
{
    SECTION("String")
    _verifyStringifyString<String>();

    // note: this one is actually the same as String, since String is an
    // alias for StringImpl<NativeStringData>. But we add an explicit
    // test for it anyway, in case the alias changes in the future
    SECTION("StringImpl<NativeStringData>")
    _verifyStringifyString<StringImpl<NativeStringData>>();

    SECTION("StringImpl<Utf8StringData>")
    _verifyStringifyString<StringImpl<Utf8StringData>>();

    SECTION("StringImpl<Utf16StringData>")
    _verifyStringifyString<StringImpl<Utf16StringData>>();

    SECTION("StringImpl<Utf32StringData>")
    _verifyStringifyString<StringImpl<Utf32StringData>>();

    SECTION("StringImpl<WideStringData>")
    _verifyStringifyString<StringImpl<WideStringData>>();

    SECTION("std::string")
    _verifyStringifyString<std::string>();

    SECTION("std::wstring")
    _verifyStringifyString<std::wstring>();

    SECTION("std::u16string")
    _verifyStringifyString<std::u16string>();

    SECTION("std::u32string")
    _verifyStringifyString<std::u32string>();

    SECTION("const char*")
    _verifyStringifyString<const char *>();

    SECTION("const wchar_t*")
    _verifyStringifyString<const wchar_t *>();

    SECTION("const char16_t*")
    _verifyStringifyString<const char16_t *>();

    SECTION("const char32_t*")
    _verifyStringifyString<const char32_t *>();

    SECTION("char*")
    _verifyStringifyString<char *>();

    SECTION("wchar_t*")
    _verifyStringifyString<wchar_t *>();

    SECTION("char16_t*")
    _verifyStringifyString<char16_t *>();

    SECTION("char32_t*")
    _verifyStringifyString<char32_t *>();
}

String getExpectedPointerString(const void *p)
{
    // how pointer addresses are printed is implementation dependent.
    // it is defined by num_put of the locale.
    // So we use std::wstringstream to get that value.

    std::wstringstream stream;
    stream << p;
    std::wstring expected = stream.str();

    REQUIRE(!expected.empty());

    return expected;
}

template <typename POINTER_TYPE> void _verifyStringifyPointerVariants(POINTER_TYPE val)
{
    String expected = getExpectedPointerString(val);

    SECTION("value")
    _verifyStringify<POINTER_TYPE>(val, expected);

    SECTION("const value")
    _verifyStringify<const POINTER_TYPE>(val, expected);

    SECTION("reference")
    _verifyStringify<POINTER_TYPE &>(val, expected);

    SECTION("const reference")
    _verifyStringify<const POINTER_TYPE &>(val, expected);
}

template <typename POINTER_TYPE> void _verifyStringifyPointer()
{
    SECTION("null")
    _verifyStringifyPointerVariants<POINTER_TYPE>(nullptr);

    SECTION("non-null")
    _verifyStringifyPointerVariants<POINTER_TYPE>((POINTER_TYPE)0x123456);

    SECTION("all bits used")
    _verifyStringifyPointerVariants<POINTER_TYPE>((POINTER_TYPE)((intptr_t)-1));
}

void _testStringifyPointer()
{
    SECTION("void*")
    _verifyStringifyPointer<void *>();

    SECTION("const void*")
    _verifyStringifyPointer<const void *>();

    SECTION("uint8_t*")
    _verifyStringifyPointer<uint8_t *>();

    SECTION("const uint8_t*")
    _verifyStringifyPointer<const uint8_t *>();

    SECTION("String*")
    _verifyStringifyPointer<const String *>();
}

static void _testStringify()
{
    SECTION("chars")
    _testStringifyChars();

    SECTION("integers")
    _testStringifyIntegers();

    SECTION("bool")
    _testStringifyBool();

    SECTION("nullptr")
    _testStringifyNullptr();

    SECTION("floating point")
    _testStringifyFloatingPoint();

    SECTION("string")
    _testStringifyString();

    SECTION("pointer")
    _testStringifyPointer();

    SECTION("object")
    _testStringifyObject();
}

TEST_CASE("StringBuffer")
{
    StringBuffer buf;

    SECTION("initial")
    _verifyResult(buf, "");

    SECTION("single char")
    {
        buf.put('x');

        _verifyResult(buf, "x");
    }

    SECTION("const char*")
    {
        SECTION("normal")
        {
            buf << "hello world";
            _verifyResult(buf, "hello world");
        }

        SECTION("ad hoc")
        _verifyResult(StringBuffer() << "hello world", "hello world");
    }

    SECTION("const wchar_t*")
    {
        SECTION("normal")
        {
            buf << L"hello world";
            _verifyResult(buf, "hello world");
        }

        SECTION("ad hoc")
        _verifyResult(StringBuffer() << L"hello world", "hello world");
    }

    SECTION("const char16_t*")
    {
        SECTION("normal")
        {
            buf << u"hello world";
            _verifyResult(buf, "hello world");
        }

        SECTION("ad hoc")
        _verifyResult(StringBuffer() << u"hello world", "hello world");
    }

    SECTION("const char32_t*")
    {
        SECTION("normal")
        {
            buf << U"hello world";
            _verifyResult(buf, "hello world");
        }

        SECTION("ad hoc")
        _verifyResult(StringBuffer() << U"hello world", "hello world");
    }

    SECTION("std::string")
    {
        SECTION("normal")
        {
            buf << std::string("hello world");
            _verifyResult(buf, "hello world");
        }

        SECTION("ad hoc")
        {
            SECTION("normal argument")
            {
                std::string s("hello world");

                _verifyResult(StringBuffer() << s, "hello world");
            }

            SECTION("ad hoc argument")
            _verifyResult(StringBuffer() << std::string("hello world"), "hello world");
        }
    }

    SECTION("std::wstring")
    {
        SECTION("normal")
        {
            buf << std::wstring(L"hello world");
            _verifyResult(buf, "hello world");
        }

        SECTION("ad hoc")
        _verifyResult(StringBuffer() << std::wstring(L"hello world"), "hello world");
    }

    SECTION("std::u16string")
    {
        SECTION("normal")
        {
            buf << std::u16string(u"hello world");
            _verifyResult(buf, "hello world");
        }

        SECTION("ad hoc")
        _verifyResult(StringBuffer() << std::u16string(u"hello world"), "hello world");
    }

    SECTION("std::u32string")
    {
        SECTION("normal")
        {
            buf << std::u32string(U"hello world");
            _verifyResult(buf, "hello world");
        }

        SECTION("ad hoc")
        _verifyResult(StringBuffer() << std::u32string(U"hello world"), "hello world");
    }

    SECTION("formatted number")
    {
        SECTION("normal")
        {
            buf << StreamFormat().zeroPad(8) << 42;
            _verifyResult(buf, "00000042");
        }

        // XXX StreamFormat does not currently work with ad hoc
        // streams. See https://systems.atlassian.net/browse/BDN-57
        // SECTION("ad hoc")
        // 	_verifyResult(StringBuffer() << StreamFormat().zeroPad(8) << 42,
        // "00000042" );
    }

    SECTION("multiple puts")
    {
        SECTION("normal")
        {
            buf << "hello " << StreamFormat().zeroPad(4) << 17 << " world";

            _verifyResult(buf, "hello 0017 world");
        }

        // XXX StreamFormat does not currently work with ad hoc
        // streams. See https://systems.atlassian.net/browse/BDN-57
        // SECTION("ad hoc")
        //	_verifyResult(StringBuffer() << "hello " <<
        // StreamFormat().zeroPad(4) << 17 << " world", "hello 0017 world");
    }

    String chunkSizeString;
    for (int i = 0; i < 128; i++)
        chunkSizeString += '0' + (i % 10);
    REQUIRE(chunkSizeString.length() == 128);

    SECTION("write exactly to chunk end")
    {
        buf << chunkSizeString;
        _verifyResult(buf, chunkSizeString);
    }

    SECTION("write over chunk end")
    {
        buf << "x" << chunkSizeString;
        _verifyResult(buf, "x" + chunkSizeString);
    }

    SECTION("write exactly to chunk end, then write again")
    {
        buf << chunkSizeString << "x";
        _verifyResult(buf, chunkSizeString + "x");
    }

    SECTION("write to 1 before chunk end, then write to chunk end")
    {
        buf << chunkSizeString.subString(0, 127) << chunkSizeString[127];
        _verifyResult(buf, chunkSizeString);
    }

    SECTION("write to 1 before chunk end, then write over chunk end")
    {
        buf << chunkSizeString.subString(0, 127) << "xx";
        _verifyResult(buf, chunkSizeString.subString(0, 127) + "xx");
    }

    SECTION("write multiple chunks")
    {
        String expected;

        for (int i = 0; i < 10; i++) {
            buf << i << chunkSizeString;
            expected += std::to_string(i) + chunkSizeString;
        }

        _verifyResult(buf, expected);
    }

    SECTION("small write then read")
    {
        // this checks if the stream buffer calls sync in underflow
        buf << "x";

        std::u32string result = _readStreamBufferContents(buf);

        REQUIRE(result == U"x");
    }

    SECTION("write then read lots of data")
    {
        String expected;

        for (int i = 0; i < 100; i++)
            expected += chunkSizeString;

        buf << expected;
        String result = _readStreamBufferContents(buf);

        REQUIRE(result == expected);
    }

    SECTION("write read write read")
    {
        buf << "helloworld";
        String result = _readStreamBufferContents(buf);
        REQUIRE(result == "helloworld");

        buf << chunkSizeString;

        String result2 = _readStreamBufferContents(buf);
        REQUIRE(result2 == chunkSizeString);
    }

    SECTION("stream buffer supports seek")
    {

        SECTION("small data")
        {
            buf << "helloworld";

            _verifySeek(buf, "helloworld");
        }

        SECTION("big data")
        {
            buf << chunkSizeString << chunkSizeString;

            _verifySeek(buf, chunkSizeString + chunkSizeString);
        }
    }

    SECTION("implicit conversion to string")
    {
        SECTION("ad hoc created")
        {
            String result = StringBuffer() << 1234 << 567;
            REQUIRE(result == "1234567");
        }

        SECTION("normal")
        {
            StringBuffer buf;
            buf << 1234 << 567;
            String result = buf;

            REQUIRE(result == "1234567");
        }
    }

    SECTION("global stringify functions")
    _testStringify();
}
