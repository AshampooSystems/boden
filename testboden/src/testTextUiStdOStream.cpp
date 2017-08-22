#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/TextUiStdOStream.h>

#include <bdn/test/MockTextUi.h>

using namespace bdn;


template<class CharType>
static void testTextUiStdOStream()
{    
    P<bdn::test::MockTextUi> pUi = newObj<bdn::test::MockTextUi>();

    const std::vector<String>& writtenChunks = pUi->getWrittenChunks();

    TextUiStdOStream<CharType> streamObj(pUi);

    // most of the detailed tests for decoding corner cases etc. are covered by the tests
    // for TextUiStdStreamBuf. Here we only have some rough tests that verify
    // that the integration of the streambuf into a stream object works as expected.

    // to be certain that this is the case we use the stream as a generic std::ostream.
    std::basic_ostream<CharType>& stream = streamObj;

    SECTION("several values")
    {
        stream << 42 << " " << 1.22 << std::endl << String("hello world\n");

        // should be buffered initially.
        REQUIRE( writtenChunks.size() == 0);

        stream.flush();

        REQUIRE( writtenChunks.size() == 1);
        REQUIRE( writtenChunks[0] == "42 1.22\nhello world\n" );
    }

    SECTION("autosync when buffer exceeded")
    {
        // the internal buffer size is 64 chars
        for(int i=0; i<8; i++)
            stream << "01234567";

        // should still fit into the buffer
        REQUIRE( writtenChunks.size() == 0);

        // this exceeds the buffer and should cause a flush
        stream << "X";

        REQUIRE( writtenChunks.size() == 1);
        REQUIRE( writtenChunks[0] == "012345670123456701234567012345670123456701234567" );        
    }
}



TEST_CASE("TextUiStdOStream")
{
    SECTION("char")
        testTextUiStdOStream<char>();

    SECTION("wchar_t")
        testTextUiStdOStream<wchar_t>();

    SECTION("char16_t")
        testTextUiStdOStream<char16_t>();

    SECTION("char32_t")
        testTextUiStdOStream<char32_t>();    

}







