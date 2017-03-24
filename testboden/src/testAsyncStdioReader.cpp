#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/AsyncStdioReader.h>

using namespace bdn;

template<typename CharType>
class AsyncStdioReaderTextContext : public Base
{
public:
    AsyncStdioReaderTextContext()
    {
        _stream << "line 1" << std::endl << "line 2" << std::endl << "line 3";

        _pReader = newObj<AsyncStdioReader<CharType> >(&_stream);
    }

    void nextStep()
    {        
        _stepCounter++;

        if(_stepCounter<3)
        {
            P<IAsyncOp<String> > pOp = _pReader->readLine();
            pOp->onDone() += weakMethod(this, &AsyncStdioReaderTextContext<CharType>::readDone );
        }
    }
    
    void readDone(IAsyncOp<String>* pOp)
    {
        _results.push_back(pOp->getResult() );
        nextStep();
    }

    void testTimeEnded()
    {
        REQUIRE( _stepCounter==4 );
        REQUIRE( _results.size()==3 );        

        REQUIRE( _results[0]=="line 1" );
        REQUIRE( _results[1]=="line 2" );
        REQUIRE( _results[2]=="line 3" );
    }

    std::basic_stringstream<CharType>   _stream;
    P< AsyncStdioReader<CharType> >     _pReader;

    int                 _stepCounter = 0;
    std::vector<String> _results;
};

template<typename CharType>
void testAsyncStdioReader()
{
    P< AsyncStdioReaderTextContext<CharType> > pContext = newObj< AsyncStdioReaderTextContext<CharType> >();

    pContext->nextStep();

	CONTINUE_SECTION_AFTER_SECONDS( 3, pContext )
    {
        pContext->testTimeEnded();
    };
}

TEST_CASE("AsyncStdioReader")
{
    SECTION("char")
    {
        testAsyncStdioReader<char>();
    }

    SECTION("wchar_t")
    {
        testAsyncStdioReader<wchar_t>();
    }

    SECTION("char16_t")
    {
        testAsyncStdioReader<char16_t>();
    }

    SECTION("char32_t")
    {
        testAsyncStdioReader<char32_t>();
    }    
}




