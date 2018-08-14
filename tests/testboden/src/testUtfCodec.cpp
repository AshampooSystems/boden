#include <bdn/init.h>
#include <bdn/UtfCodec.h>

#include <bdn/test.h>

namespace bdn
{

template<typename CharType, class ExpectedCodecType>
static void verifyUtfCodec()
{
    REQUIRE( typeid( typename UtfCodec<CharType>::EncodedElement ) == typeid(CharType) );
    REQUIRE( typeid( typename UtfCodec<CharType>::template DecodingIterator<CharType*> )
          == typeid( typename ExpectedCodecType::template DecodingIterator<CharType*> ) );
}

TEST_CASE( "UtfCodec", "[string]" )
{
    SECTION("char")
        verifyUtfCodec<char, Utf8Codec>();

    SECTION("char16_t")
        verifyUtfCodec<char16_t, Utf16Codec>();

    SECTION("char32_t")
        verifyUtfCodec<char32_t, Utf32Codec>();

    SECTION("wchar_t")
        verifyUtfCodec<wchar_t, WideCodec>();
}


}


