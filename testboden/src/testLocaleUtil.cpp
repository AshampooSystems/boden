#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/localeUtil.h>

using namespace bdn;

TEST_CASE("localeUtil")
{
    SECTION("CodecVt typedefs")
    {
        SECTION("CodecVtUtf8Utf16")
        {
            // we only test the size for the intern type because it might not always be char16_t
            REQUIRE( sizeof(CodecVtUtf8Utf16::intern_type)==2 );
            REQUIRE( typeid( CodecVtUtf8Utf16::extern_type ) == typeid(char) );
            REQUIRE( typeid( CodecVtUtf8Utf16::state_type ) == typeid(std::mbstate_t) );

            REQUIRE( typeid( CodecVtUtf8Utf16 ) == typeid( std::codecvt_utf8_utf16<CodecVtUtf8Utf16::intern_type> ) );
        }

        SECTION("CodecVtUtf8Utf32")
        {
            // we only test the size for the intern type because it might not always be char32_t
            REQUIRE( sizeof(CodecVtUtf8Utf32::intern_type)==4 );
            REQUIRE( typeid( CodecVtUtf8Utf32::extern_type ) == typeid(char) );
            REQUIRE( typeid( CodecVtUtf8Utf32::state_type ) == typeid(std::mbstate_t) );

            REQUIRE( typeid( CodecVtUtf8Utf32 ) == typeid( std::codecvt_utf8<CodecVtUtf8Utf32::intern_type> ) );
        }

        SECTION("CodecVtUtf8Ucs2")
        {
            // we only test the size for the intern type because it might not always be char16_t
            REQUIRE( sizeof(CodecVtUtf8Ucs2::intern_type)==2 );
            REQUIRE( typeid( CodecVtUtf8Ucs2::extern_type ) == typeid(char) );
            REQUIRE( typeid( CodecVtUtf8Ucs2::state_type ) == typeid(std::mbstate_t) );

            REQUIRE( typeid( CodecVtUtf8Ucs2 ) == typeid( std::codecvt_utf8<CodecVtUtf8Ucs2::intern_type> ) );
        }

        SECTION("CodecVtUtf8Ucs4")
        {
            // we only test the size for the intern type because it might not always be char32_t
            REQUIRE( sizeof(CodecVtUtf8Ucs4::intern_type)==4 );
            REQUIRE( typeid( CodecVtUtf8Ucs4::extern_type ) == typeid(char) );
            REQUIRE( typeid( CodecVtUtf8Ucs4::state_type ) == typeid(std::mbstate_t) );

            REQUIRE( typeid( CodecVtUtf8Ucs4 ) == typeid( std::codecvt_utf8<CodecVtUtf8Ucs4::intern_type> ) );
        }


        SECTION("CodecVtChar16Char")
        {
            // we only test the size for the intern type because it might not always be char16_t
            REQUIRE( sizeof(CodecVtChar16Char::intern_type)==2 );
            REQUIRE( typeid( CodecVtChar16Char::extern_type ) == typeid(char) );
            REQUIRE( typeid( CodecVtChar16Char::state_type ) == typeid(std::mbstate_t) );

            REQUIRE( typeid( CodecVtChar16Char ) == typeid( std::codecvt<CodecVtChar16Char::intern_type, char, std::mbstate_t> ) );
        }

        SECTION("CodecVtChar32Char")
        {
            // we only test the size for the intern type because it might not always be char32_t
            REQUIRE( sizeof(CodecVtChar32Char::intern_type)==4 );
            REQUIRE( typeid( CodecVtChar32Char::extern_type ) == typeid(char) );
            REQUIRE( typeid( CodecVtChar32Char::state_type ) == typeid(std::mbstate_t) );

            REQUIRE( typeid( CodecVtChar32Char ) == typeid( std::codecvt<CodecVtChar32Char::intern_type, char, std::mbstate_t> ) );
        }
    }

    SECTION("deriveUtf8Locale")
    {
        // we know that the classic locale is NOT utf-8, so we can use that
        // for testing
        std::locale origLoc = std::locale::classic();
        std::locale loc = deriveUtf8Locale( origLoc );

        // verify that the locale uses utf-8 codecs
        REQUIRE( typeid( std::use_facet< std::codecvt<wchar_t,char,mbstate_t> >(loc) ) == typeid( std::codecvt_utf8<wchar_t> ) );
        REQUIRE( typeid( std::use_facet< CodecVtChar16Char >(loc) ) == typeid( CodecVtUtf8Utf16 ) );
        REQUIRE( typeid( std::use_facet< CodecVtChar32Char >(loc) ) == typeid( CodecVtUtf8Utf32 ) );

        // verify that the other settings are still the same
        REQUIRE( typeid( std::use_facet< std::moneypunct<char> >(loc) ) == typeid( std::use_facet< std::moneypunct<char> >(origLoc) )  );
        REQUIRE( std::use_facet< std::moneypunct<char> >(loc).decimal_point() == std::use_facet< std::moneypunct<char> >(origLoc).decimal_point()  );
        
    }

	SECTION("isUtf8Locale")
	{
        SECTION("classic")
        {
            // the classic "C" locale always uses ascii encoding
            REQUIRE( !isUtf8Locale( std::locale::classic() ) );
        }

        SECTION("global")
        {
            // the global locale may have been changed, so we cannot be certain 
            // which encoding is uses. So here we only test that there is no crash
            isUtf8Locale( std::locale() );
        }

        SECTION("native")
        {
            // the "native" locale is the one that is the default for the system.
            // We also do not know if this is Utf8 or not, so we only test that this
            // does not crash.
            isUtf8Locale( std::locale("") );
        }

        SECTION("derived utf8")
        {
            std::locale loc = deriveUtf8Locale( std::locale::classic() );

            REQUIRE( isUtf8Locale(loc) );
        }

        SECTION("custom utf8")
        {
            // we derive our own types from the standard library ones and set them in the locale.

            class MyUtf8Wchar : public std::codecvt_utf8<wchar_t>
            {
            };

            class MyUtf8Utf16 : public CodecVtUtf8Utf16
            {
            };

            class MyUtf8Utf32 : public CodecVtUtf8Utf32
            {
            };


            std::locale loc = std::locale ( std::locale::classic(), new MyUtf8Wchar );
            loc = std::locale( loc, new MyUtf8Utf16 );
            loc = std::locale( loc, new MyUtf8Utf32 );

            REQUIRE( isUtf8Locale(loc) );
        }
    }
}

