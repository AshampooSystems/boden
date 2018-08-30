#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/IUiProvider.h>

#include <bdn/TextSinkStdStreamBuf.h>
#include <bdn/test/MockTextSink.h>

using namespace bdn;


template<typename CHAR_TYPE>
class RedirectStdStream
{
public:
    RedirectStdStream(
                  std::basic_ostream<CHAR_TYPE>& stream,
                  std::basic_streambuf<CHAR_TYPE>& buffer)
    : _stream(stream)
    {
        _pOldBuffer = _stream.rdbuf(&buffer);
    }
    
    ~RedirectStdStream()
    {
        _stream.rdbuf( _pOldBuffer );
    }
    
private:
    std::basic_ostream<CHAR_TYPE>&   _stream;
    std::basic_streambuf<CHAR_TYPE>* _pOldBuffer;
};

template<typename CHAR_TYPE>
void testDefaultUiProvider( std::basic_ostream<CHAR_TYPE>& stdOutStream, std::basic_ostream<CHAR_TYPE>& stdErrStream )
{
    SECTION("textUi")
    {
        P<ITextUi> pUi = getDefaultUiProvider()->getTextUi();
        
        SECTION("stdio connection")
        {
            bdn::test::MockTextSink outSink;
            bdn::test::MockTextSink errSink;
            
            TextSinkStdStreamBuf<CHAR_TYPE> outBuffer(&outSink);
            TextSinkStdStreamBuf<CHAR_TYPE> errBuffer(&errSink);
            
            {
                RedirectStdStream<CHAR_TYPE> redirectOut( stdOutStream, outBuffer);
                RedirectStdStream<CHAR_TYPE> redirectErr( stdErrStream, errBuffer);
            
                // IMPORTANT: we should not start any new sections
                // here, nor make any assertions while the streams
                // are redirected. Otherwise output from the test framework
                // could end up in our test buffer instead of the intended
                // stream.
                pUi->output()->writeLine("for out");
                pUi->statusOrProblem()->writeLine("for err");
                
                stdOutStream.flush();
                stdErrStream.flush();
            }
            
            // the cout/cerr redirection has been undone.
            // So now we can use functionality from the test framework again.
            
            String out;
            String err;
            
            for(auto& s: outSink.getWrittenChunks() )
                out += s;
            
            for(auto& s: errSink.getWrittenChunks() )
                err += s;
            
            REQUIRE( out == "for out\n" );
            REQUIRE( err == "for err\n" );
        }
    }
}



TEST_CASE("defaultUiProvider")
{

#if BDN_PLATFORM_FAMILY_WINDOWS

    // on Windows we use wcout and wcerr, since then we
    // do no have encoding problems.
    testDefaultUiProvider<wchar_t>(std::wcout, std::wcerr);

#else
    // on other platforms the normal std::cout and std::cerr are used
    testDefaultUiProvider<char>(std::cout, std::cerr);
#endif
}






