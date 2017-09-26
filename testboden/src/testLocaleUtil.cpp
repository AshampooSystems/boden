#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/localeUtil.h>

using namespace bdn;


/** Wraps another codec to hide its type (used for testing).*/
template<class InnerCodecType>
class CodecVtWrapper_ : public std::codecvt<typename InnerCodecType::intern_type, typename InnerCodecType::extern_type, typename InnerCodecType::state_type>
{
public:

private:

    typedef typename InnerCodecType::intern_type MyInternType;
    typedef typename InnerCodecType::extern_type MyExternType;
    typedef typename InnerCodecType::state_type MyStateType;

protected:

    typename std::codecvt<typename InnerCodecType::intern_type, typename InnerCodecType::extern_type, typename InnerCodecType::state_type>::result
        do_out( MyStateType& state,
            const MyInternType* from,
            const MyInternType* from_end,
            const MyInternType*& from_next,
            MyExternType* to,
            MyExternType* to_end,
            MyExternType*& to_next ) const override
    {
        return _innerCodec.out(state, from, from_end, from_next, to, to_end, to_next);
    }

    typename std::codecvt<typename InnerCodecType::intern_type, typename InnerCodecType::extern_type, typename InnerCodecType::state_type>::result
        do_in( MyStateType& state,
            const MyExternType* from,
            const MyExternType* from_end,
            const MyExternType*& from_next,
            MyInternType* to,
            MyInternType* to_end,
            MyInternType*& to_next ) const override
    {
        return _innerCodec.in(state, from, from_end, from_next, to, to_end, to_next);
    }

    typename std::codecvt<typename InnerCodecType::intern_type, typename InnerCodecType::extern_type, typename InnerCodecType::state_type>::result
        do_unshift( MyStateType& state,
                           MyExternType* to,
                           MyExternType* to_end,
                           MyExternType*& to_next) const override
    {
        return _innerCodec.unshift(state, to, to_end, to_next);
    }
        
    int do_encoding() const noexcept override
    {
        return _innerCodec.encoding();
    }

    bool do_always_noconv() const noexcept override
    {
        return _innerCodec.always_noconv();
    }

    int do_length( MyStateType& state,
                       const MyExternType* from,
                       const MyExternType* from_end,
                       std::size_t max ) const override
    {
        return _innerCodec.length(state, from, from_end, max);
    }
    
    int do_max_length() const noexcept override
    {
        return _innerCodec.max_length();
    }
        

private:
    
    InnerCodecType _innerCodec;
};


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
        const std::codecvt<wchar_t,char,mbstate_t>& locWideCodec = std::use_facet< std::codecvt<wchar_t,char,mbstate_t> >(loc);

        REQUIRE( typeid( locWideCodec ) == typeid( std::codecvt_utf8<wchar_t> ) );

        
#if defined(_MSC_VER) && _MSC_VER>=1900 && _MSC_VER<=1901  // Visual Studio 2015 and 2017
        // the necessary specializations for std::use_facet for char16_t and char32_t are missing
        // in Visual Studio 2015 and 2017 (even though these are standard facets defined by the standard).
        // Note that there is also no use_facet specialization for the alternative codecvt implementations
        // for unsigned int and unsigned short.
        // So we cannot test this if we have these compiler versions.

        // So, do nothing here.
#else
        const std::codecvt<char16_t,char,mbstate_t>& locUtf16Codec = std::use_facet< std::codecvt<char16_t,char,mbstate_t> >(loc);
        REQUIRE( typeid( locUtf16Codec  ) == typeid( CodecVtUtf8Utf16 ) );

        const std::codecvt<char32_t,char,mbstate_t>& locUtf32Codec = std::use_facet< std::codecvt<char32_t,char,mbstate_t> >(loc);
        REQUIRE( typeid( locUtf32Codec ) == typeid( CodecVtUtf8Utf32 ) );
#endif

        // verify that the other settings are still the same
        const std::moneypunct<char>& locMoneyPunct = std::use_facet< std::moneypunct<char> >(loc);
        const std::moneypunct<char>& origLocMoneyPunct = std::use_facet< std::moneypunct<char> >(origLoc);
        REQUIRE( typeid( locMoneyPunct ) == typeid( origLocMoneyPunct )  );

        REQUIRE( locMoneyPunct.decimal_point() == origLocMoneyPunct.decimal_point()  );
        
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

        SECTION("custom utf8 derived from codecvt_utf8")
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

        SECTION("custom utf8 not derived from codecvt_utf8")
        {
            // we derive our own types from the standard library ones and set them in the locale.
                       
            std::locale loc = std::locale ( std::locale::classic(), new CodecVtWrapper_< std::codecvt_utf8<wchar_t> > );
            loc = std::locale( loc, new CodecVtWrapper_< CodecVtUtf8Utf16 > );
            loc = std::locale( loc, new CodecVtWrapper_< CodecVtUtf8Utf32 > );

            REQUIRE( isUtf8Locale(loc) );
        }
    }
}

