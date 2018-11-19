#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/StdioTextUi.h>

using namespace bdn;

template <typename CharType> class TestStdioTextUiFixture : public Base
{
  public:
    TestStdioTextUiFixture()
    {
        _inStream << String("first line") << std::endl << String("second line") << std::endl << String("third line");

        _ui = newObj<StdioTextUi<CharType>>(&_inStream, &_outStream, &_errStream);
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
        P<TestStdioTextUiFixture> self = this;

        SECTION("readLine")
        {
            SECTION("sequential")
            {
                P<IAsyncOp<String>> op = _ui->readLine();

                P<IAsyncOp<String>> captureOp = holdOpReference ? op : nullptr;

                op->onDone() += [captureOp, this, holdOpReference](P<IAsyncOp<String>> paramOp) {
                    String result = paramOp->getResult();
                    REQUIRE(result == "first line");
                    if (captureOp != nullptr) {
                        result = captureOp->getResult();
                        REQUIRE(result == "first line");
                    }

                    P<IAsyncOp<String>> op2 = this->_ui->readLine();

                    P<IAsyncOp<String>> captureOp2 = holdOpReference ? op2 : nullptr;

                    op2->onDone() += [captureOp2, this, holdOpReference](P<IAsyncOp<String>> paramOp) {
                        String result = paramOp->getResult();
                        REQUIRE(result == "second line");
                        if (captureOp2 != nullptr) {
                            result = captureOp2->getResult();
                            REQUIRE(result == "second line");
                        }

                        P<IAsyncOp<String>> op3 = this->_ui->readLine();

                        P<IAsyncOp<String>> captureOp3 = holdOpReference ? op3 : nullptr;

                        op3->onDone() += [captureOp3, this](P<IAsyncOp<String>> paramOp) {
                            String result = paramOp->getResult();
                            REQUIRE(result == "third line");
                            if (captureOp3 != nullptr) {
                                result = captureOp3->getResult();
                                REQUIRE(result == "third line");
                            }

                            this->_finished = true;
                        };
                    };
                };

                scheduleFinishTest();
            }

            SECTION("parallel")
            {
                P<IAsyncOp<String>> op = _ui->readLine();
                P<IAsyncOp<String>> op2 = _ui->readLine();
                P<IAsyncOp<String>> op3 = _ui->readLine();

                P<IAsyncOp<String>> captureOp = holdOpReference ? op : nullptr;
                P<IAsyncOp<String>> captureOp2 = holdOpReference ? op2 : nullptr;
                P<IAsyncOp<String>> captureOp3 = holdOpReference ? op3 : nullptr;

                op->onDone() += [captureOp, this](P<IAsyncOp<String>> paramOp) {
                    String result = paramOp->getResult();
                    REQUIRE(result == "first line");
                    if (captureOp != nullptr) {
                        result = captureOp->getResult();
                        REQUIRE(result == "first line");
                    }

                    REQUIRE(this->_finishedOpCounter == 0);
                    this->_finishedOpCounter++;
                };

                op2->onDone() += [captureOp2, this](P<IAsyncOp<String>> paramOp) {
                    String result2 = paramOp->getResult();
                    REQUIRE(result2 == "second line");
                    if (captureOp2 != nullptr) {
                        result2 = captureOp2->getResult();
                        REQUIRE(result2 == "second line");
                    }

                    REQUIRE(this->_finishedOpCounter == 1);
                    this->_finishedOpCounter++;
                };

                op3->onDone() += [captureOp3, this](P<IAsyncOp<String>> paramOp) {
                    String result3 = paramOp->getResult();
                    REQUIRE(result3 == "third line");
                    if (captureOp3 != nullptr) {
                        result3 = captureOp3->getResult();
                        REQUIRE(result3 == "third line");
                    }

                    REQUIRE(this->_finishedOpCounter == 2);
                    this->_finishedOpCounter++;
                    this->_finished = true;
                };

                scheduleFinishTest();
            }
        }

        SECTION("output.writeLine")
        testWrite(strongMethod((ITextSink *)_ui->output().getPtr(), &ITextSink::writeLine), "\n", &_outStream);

        SECTION("write")
        testWrite(strongMethod((ITextSink *)_ui->output().getPtr(), &ITextSink::write), "", &_outStream);

        SECTION("writeErrorLine")
        testWrite(strongMethod((ITextSink *)_ui->statusOrProblem().getPtr(), &ITextSink::writeLine), "\n", &_errStream);

        SECTION("writeError")
        testWrite(strongMethod((ITextSink *)_ui->statusOrProblem().getPtr(), &ITextSink::write), "", &_errStream);
    }

    void testWrite(std::function<void(String)> writeFunc, String expectedWriteSuffix,
                   std::basic_stringstream<CharType> *stream)
    {
        P<TestStdioTextUiFixture> self = this;

        SECTION("sequential")
        {
            writeFunc("first");

            REQUIRE(String(stream->str()) == "first" + expectedWriteSuffix);

            writeFunc("second");

            REQUIRE(String(stream->str()) == "first" + expectedWriteSuffix + "second" + expectedWriteSuffix);

            writeFunc("third");
            REQUIRE(String(stream->str()) ==
                    "first" + expectedWriteSuffix + "second" + expectedWriteSuffix + "third" + expectedWriteSuffix);

            this->_finished = true;

            scheduleFinishTest();
        }

#if BDN_HAVE_THREADS
        SECTION("multithreaded")
        {
            P<Signal> signal = newObj<Signal>();

            std::future<void> thread1Result = Thread::exec([writeFunc, signal]() {
                signal->wait();
                writeFunc(",first");
            });

            std::future<void> thread2Result = Thread::exec([writeFunc, signal]() {
                signal->wait();
                writeFunc(",second");
            });

            std::future<void> thread3Result = Thread::exec([writeFunc, signal]() {
                signal->wait();
                writeFunc(",third");
            });

            // wait a little to ensure that all three threads are waiting on
            // out signal.
            Thread::sleepMillis(1000);

            // set the signal to start all three operations at the same time
            signal->set();

            // wait for all threads to finish
            thread1Result.get();
            thread2Result.get();
            thread3Result.get();

            String result(stream->str());

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
        CONTINUE_SECTION_AFTER_RUN_SECONDS_WITH(0.2, strongMethod(this, &TestStdioTextUiFixture::finishTest));
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
    P<StdioTextUi<CharType>> _ui;

    int _finishedOpCounter = 0;
    int _finishCheckCounter = 0;
    bool _finished = false;
};

template <typename CharType> void testStdioTextUi()
{
    P<TestStdioTextUiFixture<CharType>> fixture = newObj<TestStdioTextUiFixture<CharType>>();

    fixture->doTest();
}

TEST_CASE("StdioTextUi")
{
    SECTION("char") { testStdioTextUi<char>(); }

    SECTION("wchar_t") { testStdioTextUi<wchar_t>(); }
}
