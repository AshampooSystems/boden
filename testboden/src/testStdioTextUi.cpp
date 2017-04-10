#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/StdioTextUi.h>

using namespace bdn;

template<typename CharType>
class TestStdioTextUiFixture : public Base
{
public:
    TestStdioTextUiFixture()
    {
        _inStream
            << String("first line") << std::endl
            << String("second line") << std::endl
            << String("third line");

        _pUi = newObj< StdioTextUi<CharType> >( &_inStream, &_outStream, &_errStream );
    }

    void doTest()
    {
        P<TestStdioTextUiFixture> pThis = this;

        SECTION("readLine")
        {
            SECTION("sequential")
            {
                P<IAsyncOp<String>> pOp = _pUi->readLine();

                pOp->onDone() += [pOp, pThis]( P<IAsyncOp<String>> pParamOp )
                {
                    String result = pOp->getResult();
                    REQUIRE( result=="first line" );
                    result = pParamOp->getResult();
                    REQUIRE( result=="first line" );

                    P<IAsyncOp<String>> pOp2 = pThis->_pUi->readLine();

                    pOp2->onDone() += [pOp2, pThis]( P<IAsyncOp<String>> pParamOp)
                    {
                        String result = pOp2->getResult();
                        REQUIRE( result=="second line" );
                        result = pParamOp->getResult();
                        REQUIRE( result=="second line" );

                        P<IAsyncOp<String>> pOp3 = pThis->_pUi->readLine();

                        pOp3->onDone() += [pOp3, pThis](P<IAsyncOp<String>> pParamOp)
                        {
                            String result = pOp3->getResult();
                            REQUIRE( result=="third line" );
                            result = pParamOp->getResult();
                            REQUIRE( result=="third line" );

                            pThis->_finished = true;
                        };
                    };
                };

                scheduleFinishTest();
            }

            SECTION("parallel")
            {
                P<IAsyncOp<String>> pOp = _pUi->readLine();
                P<IAsyncOp<String>> pOp2 = _pUi->readLine();
                P<IAsyncOp<String>> pOp3 = _pUi->readLine();

                pOp->onDone() += [pOp, pThis](P<IAsyncOp<String>> pParamOp)
                {
                    String result = pOp->getResult();
                    REQUIRE( result=="first line" );
                    result = pParamOp->getResult();
                    REQUIRE( result=="first line" );

                    REQUIRE( pThis->_finishedOpCounter==0);
                    pThis->_finishedOpCounter++;
                };

                pOp2->onDone() += [pOp2, pThis](P<IAsyncOp<String>> pParamOp)
                {
                    String result2 = pOp2->getResult();
                    REQUIRE( result2=="second line" );
                    result2 = pParamOp->getResult();
                    REQUIRE( result2=="second line" );

                    REQUIRE( pThis->_finishedOpCounter==1);
                    pThis->_finishedOpCounter++;
                };

                pOp3->onDone() += [pOp3, pThis](P<IAsyncOp<String>> pParamOp)
                {
                    String result3 = pOp3->getResult();
                    REQUIRE( result3=="third line" );
                    result3 = pParamOp->getResult();
                    REQUIRE( result3=="third line" );

                    REQUIRE( pThis->_finishedOpCounter==2);
                    pThis->_finishedOpCounter++;
                    pThis->_finished = true;
                };

                scheduleFinishTest();
            }
        }


        SECTION("writeLine")
        {
            testWrite( strongMethod((ITextUi*)_pUi.getPtr(), &ITextUi::writeLine), "\n", &_outStream );
        }

        SECTION("write")
        {
            testWrite( strongMethod((ITextUi*)_pUi.getPtr(), &ITextUi::write), "", &_outStream );
        }

        SECTION("writeErrorLine")
        {
            testWrite( strongMethod((ITextUi*)_pUi.getPtr(), &ITextUi::writeErrorLine), "\n", &_errStream );
        }

        SECTION("writeError")
        {
            testWrite( strongMethod((ITextUi*)_pUi.getPtr(), &ITextUi::writeError), "", &_errStream );
        }
    }

    void testWrite(std::function< P<IAsyncOp<void>>(String) > writeFunc, String expectedWriteSuffix, std::basic_stringstream<CharType>* pStream)
    {
        P<TestStdioTextUiFixture> pThis = this;

        SECTION("sequential")
        {
            P<IAsyncOp<void>> pOp = writeFunc("first");

            pOp->onDone() += [pOp, pThis, writeFunc, pStream, expectedWriteSuffix]( P<IAsyncOp<void>> pParamOp )
            {
                REQUIRE( String(pStream->str())=="first"+expectedWriteSuffix );
                pOp->getResult();
                pParamOp->getResult();
                REQUIRE( String(pStream->str())=="first"+expectedWriteSuffix );

                P<IAsyncOp<void>> pOp2 = writeFunc("second");

                pOp2->onDone() += [pOp2, pThis, writeFunc, pStream, expectedWriteSuffix]( P<IAsyncOp<void>> pParamOp)
                {
                    REQUIRE( String(pStream->str())=="first"+expectedWriteSuffix+"second"+expectedWriteSuffix );
                    pOp2->getResult();
                    pParamOp->getResult();
                    REQUIRE( String(pStream->str())=="first"+expectedWriteSuffix+"second"+expectedWriteSuffix );

                    P<IAsyncOp<void>> pOp3 = writeFunc("third");

                    pOp3->onDone() += [pOp3, pThis, writeFunc, pStream, expectedWriteSuffix](P<IAsyncOp<void>> pParamOp)
                    {
                        REQUIRE( String(pStream->str())=="first"+expectedWriteSuffix+"second"+expectedWriteSuffix+"third"+expectedWriteSuffix );
                        pOp3->getResult();
                        pParamOp->getResult();
                        REQUIRE( String(pStream->str())=="first"+expectedWriteSuffix+"second"+expectedWriteSuffix+"third"+expectedWriteSuffix );

                        pThis->_finished = true;
                    };
                };
            };

            scheduleFinishTest();
        }

        SECTION("parallel")
        {
            P<IAsyncOp<void>> pOp = writeFunc("first");
            P<IAsyncOp<void>> pOp2 = writeFunc("second");
            P<IAsyncOp<void>> pOp3 = writeFunc("third");

            pOp->onDone() += [pOp, pThis, pStream](P<IAsyncOp<void>> pParamOp)
            {
                pOp->getResult();
                pParamOp->getResult();

                REQUIRE( pThis->_finishedOpCounter==0);
                pThis->_finishedOpCounter++;                    
            };

            pOp2->onDone() += [pOp2, pThis, pStream](P<IAsyncOp<void>> pParamOp)
            {
                pOp2->getResult();
                pParamOp->getResult();

                REQUIRE( pThis->_finishedOpCounter==1);
                pThis->_finishedOpCounter++;
            };

            pOp3->onDone() += [pOp3, pThis, pStream, expectedWriteSuffix](P<IAsyncOp<void>> pParamOp)
            {
                REQUIRE( pThis->_finishedOpCounter==2);

                REQUIRE( String(pStream->str())=="first"+expectedWriteSuffix+"second"+expectedWriteSuffix+"third"+expectedWriteSuffix );
                pOp3->getResult();
                pParamOp->getResult();
                REQUIRE( String(pStream->str())=="first"+expectedWriteSuffix+"second"+expectedWriteSuffix+"third"+expectedWriteSuffix );

                pThis->_finishedOpCounter++;
                pThis->_finished = true;
            };

            scheduleFinishTest();
        }
    }

    void scheduleFinishTest()
    {
        CONTINUE_SECTION_AFTER_SECONDS_WITH(0.2, strongMethod(this, &TestStdioTextUiFixture::finishTest));
    }
    
    void finishTest()
    {
        _finishCheckCounter++;

        if(!_finished)
        {
            REQUIRE( _finishCheckCounter<3 );

            scheduleFinishTest();
        }
    }

private:
    std::basic_stringstream<CharType>   _inStream;
    std::basic_stringstream<CharType>   _outStream;
    std::basic_stringstream<CharType>   _errStream;
    P<StdioTextUi<CharType> >           _pUi;

    int                                 _finishedOpCounter = 0;
    int                                 _finishCheckCounter = 0;
    bool                                _finished = false;
};


template<typename CharType>
void testStdioTextUi()
{
    P<TestStdioTextUiFixture<CharType> > pFixture = newObj<TestStdioTextUiFixture<CharType> >();

    pFixture->doTest();
}

TEST_CASE("StdioTextUi")
{
    SECTION("char")
    {
        testStdioTextUi<char>();
    }

    SECTION("wchar_t")
    {
        testStdioTextUi<wchar_t>();
    }
}




