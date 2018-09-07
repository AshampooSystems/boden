#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/StdioTextUi.h>

using namespace bdn;

template <typename CharType> class TestStdioTextUiFixture : public Base
{
  public:
    TestStdioTextUiFixture()
    {
        _inStream << String("first line") << std::endl
                  << String("second line") << std::endl
                  << String("third line");

        _pUi =
            newObj<StdioTextUi<CharType>>(&_inStream, &_outStream, &_errStream);
    }

    void doTest()
    {
        SECTION("hold op reference")
        doTest(true);

        SECTION("don't hold op reference")
        doTest(false);
    }

    void doTest(bool holdOpReference)
    {
        P<TestStdioTextUiFixture> pThis = this;

        SECTION("readLine")
        {
            SECTION("sequential")
            {
                P<IAsyncOp<String>> pOp = _pUi->readLine();

                P<IAsyncOp<String>> pCaptureOp =
                    holdOpReference ? pOp : nullptr;

                pOp->onDone() += [pCaptureOp, pThis, holdOpReference](
                                     P<IAsyncOp<String>> pParamOp) {
                    String result = pParamOp->getResult();
                    REQUIRE(result == "first line");
                    if (pCaptureOp != nullptr) {
                        result = pCaptureOp->getResult();
                        REQUIRE(result == "first line");
                    }

                    P<IAsyncOp<String>> pOp2 = pThis->_pUi->readLine();

                    P<IAsyncOp<String>> pCaptureOp2 =
                        holdOpReference ? pOp2 : nullptr;

                    pOp2->onDone() += [pCaptureOp2, pThis, holdOpReference](
                                          P<IAsyncOp<String>> pParamOp) {
                        String result = pParamOp->getResult();
                        REQUIRE(result == "second line");
                        if (pCaptureOp2 != nullptr) {
                            result = pCaptureOp2->getResult();
                            REQUIRE(result == "second line");
                        }

                        P<IAsyncOp<String>> pOp3 = pThis->_pUi->readLine();

                        P<IAsyncOp<String>> pCaptureOp3 =
                            holdOpReference ? pOp3 : nullptr;

                        pOp3->onDone() +=
                            [pCaptureOp3, pThis](P<IAsyncOp<String>> pParamOp) {
                                String result = pParamOp->getResult();
                                REQUIRE(result == "third line");
                                if (pCaptureOp3 != nullptr) {
                                    result = pCaptureOp3->getResult();
                                    REQUIRE(result == "third line");
                                }

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

                P<IAsyncOp<String>> pCaptureOp =
                    holdOpReference ? pOp : nullptr;
                P<IAsyncOp<String>> pCaptureOp2 =
                    holdOpReference ? pOp2 : nullptr;
                P<IAsyncOp<String>> pCaptureOp3 =
                    holdOpReference ? pOp3 : nullptr;

                pOp->onDone() +=
                    [pCaptureOp, pThis](P<IAsyncOp<String>> pParamOp) {
                        String result = pParamOp->getResult();
                        REQUIRE(result == "first line");
                        if (pCaptureOp != nullptr) {
                            result = pCaptureOp->getResult();
                            REQUIRE(result == "first line");
                        }

                        REQUIRE(pThis->_finishedOpCounter == 0);
                        pThis->_finishedOpCounter++;
                    };

                pOp2->onDone() +=
                    [pCaptureOp2, pThis](P<IAsyncOp<String>> pParamOp) {
                        String result2 = pParamOp->getResult();
                        REQUIRE(result2 == "second line");
                        if (pCaptureOp2 != nullptr) {
                            result2 = pCaptureOp2->getResult();
                            REQUIRE(result2 == "second line");
                        }

                        REQUIRE(pThis->_finishedOpCounter == 1);
                        pThis->_finishedOpCounter++;
                    };

                pOp3->onDone() +=
                    [pCaptureOp3, pThis](P<IAsyncOp<String>> pParamOp) {
                        String result3 = pParamOp->getResult();
                        REQUIRE(result3 == "third line");
                        if (pCaptureOp3 != nullptr) {
                            result3 = pCaptureOp3->getResult();
                            REQUIRE(result3 == "third line");
                        }

                        REQUIRE(pThis->_finishedOpCounter == 2);
                        pThis->_finishedOpCounter++;
                        pThis->_finished = true;
                    };

                scheduleFinishTest();
            }
        }

        SECTION("output.writeLine")
        testWrite(strongMethod((ITextSink *)_pUi->output().getPtr(),
                               &ITextSink::writeLine),
                  "\n", &_outStream);

        SECTION("write")
        testWrite(strongMethod((ITextSink *)_pUi->output().getPtr(),
                               &ITextSink::write),
                  "", &_outStream);

        SECTION("writeErrorLine")
        testWrite(strongMethod((ITextSink *)_pUi->statusOrProblem().getPtr(),
                               &ITextSink::writeLine),
                  "\n", &_errStream);

        SECTION("writeError")
        testWrite(strongMethod((ITextSink *)_pUi->statusOrProblem().getPtr(),
                               &ITextSink::write),
                  "", &_errStream);
    }

    void testWrite(std::function<void(String)> writeFunc,
                   String expectedWriteSuffix,
                   std::basic_stringstream<CharType> *pStream)
    {
        P<TestStdioTextUiFixture> pThis = this;

        SECTION("sequential")
        {
            writeFunc("first");

            REQUIRE(String(pStream->str()) == "first" + expectedWriteSuffix);

            writeFunc("second");

            REQUIRE(String(pStream->str()) == "first" + expectedWriteSuffix +
                                                  "second" +
                                                  expectedWriteSuffix);

            writeFunc("third");
            REQUIRE(String(pStream->str()) ==
                    "first" + expectedWriteSuffix + "second" +
                        expectedWriteSuffix + "third" + expectedWriteSuffix);

            pThis->_finished = true;

            scheduleFinishTest();
        }

#if BDN_HAVE_THREADS
        SECTION("multithreaded")
        {
            P<Signal> pSignal = newObj<Signal>();

            std::future<void> thread1Result =
                Thread::exec([writeFunc, pSignal]() {
                    pSignal->wait();
                    writeFunc(",first");
                });

            std::future<void> thread2Result =
                Thread::exec([writeFunc, pSignal]() {
                    pSignal->wait();
                    writeFunc(",second");
                });

            std::future<void> thread3Result =
                Thread::exec([writeFunc, pSignal]() {
                    pSignal->wait();
                    writeFunc(",third");
                });

            // wait a little to ensure that all three threads are waiting on
            // out signal.
            Thread::sleepMillis(1000);

            // set the signal to start all three operations at the same time
            pSignal->set();

            // wait for all threads to finish
            thread1Result.get();
            thread2Result.get();
            thread3Result.get();

            String result(pStream->str());

            REQUIRE(result.startsWith(","));
            result = result.subString(1);

            bool gotFirst = false;
            bool gotSecond = false;
            bool gotThird = false;

            char32_t lastSep = 1;

            while (!result.isEmpty()) {
                String token = result.splitOffToken(",", true, &lastSep);
                if (token == "first" + expectedWriteSuffix) {
                    REQUIRE(!gotFirst);
                    gotFirst = true;
                }
                if (token == "second" + expectedWriteSuffix) {
                    REQUIRE(!gotSecond);
                    gotSecond = true;
                }

                if (token == "third" + expectedWriteSuffix) {
                    REQUIRE(!gotThird);
                    gotThird = true;
                }
            }

            REQUIRE(gotFirst);
            REQUIRE(gotSecond);
            REQUIRE(gotThird);

            REQUIRE(lastSep == 0);
        }
#endif
    }

    void scheduleFinishTest()
    {
        CONTINUE_SECTION_AFTER_RUN_SECONDS_WITH(
            0.2, strongMethod(this, &TestStdioTextUiFixture::finishTest));
    }

    void finishTest()
    {
        _finishCheckCounter++;

        if (!_finished) {
            REQUIRE(_finishCheckCounter < 3);

            scheduleFinishTest();
        }
    }

  private:
    std::basic_stringstream<CharType> _inStream;
    std::basic_stringstream<CharType> _outStream;
    std::basic_stringstream<CharType> _errStream;
    P<StdioTextUi<CharType>> _pUi;

    int _finishedOpCounter = 0;
    int _finishCheckCounter = 0;
    bool _finished = false;
};

template <typename CharType> void testStdioTextUi()
{
    P<TestStdioTextUiFixture<CharType>> pFixture =
        newObj<TestStdioTextUiFixture<CharType>>();

    pFixture->doTest();
}

TEST_CASE("StdioTextUi")
{
    SECTION("char") { testStdioTextUi<char>(); }

    SECTION("wchar_t") { testStdioTextUi<wchar_t>(); }
}
