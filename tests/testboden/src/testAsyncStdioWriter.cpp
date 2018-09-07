#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/AsyncStdioWriter.h>

using namespace bdn;

template <typename CharType>
class AsyncStdioWriter_OneAtTheTimeTestContext : public Base
{
  public:
    AsyncStdioWriter_OneAtTheTimeTestContext(bool writeLine)
    {
        _pWriter = newObj<AsyncStdioWriter<CharType>>(&_stream);
        _writeLine = writeLine;
    }

    void startTest()
    {
        nextStep();

        CONTINUE_SECTION_AFTER_RUN_SECONDS_WITH(
            0.5,
            strongMethod(
                this, &AsyncStdioWriter_OneAtTheTimeTestContext::continueTest));
    }

    void nextStep()
    {
        if (_nextStep < 3) {
            String textToWrite = "line " + std::to_string(_nextStep);

            _nextStep++;

            P<IAsyncOp<void>> pOp;
            if (_writeLine)
                pOp = _pWriter->writeLine(textToWrite);
            else
                pOp = _pWriter->write(textToWrite);
            pOp->onDone() += weakMethod(
                this,
                &AsyncStdioWriter_OneAtTheTimeTestContext<CharType>::writeDone);
        }
    }

    void writeDone(IAsyncOp<void> *pOp)
    {
        Mutex::Lock lock(_mutex);

        // check for errors
        pOp->getResult();

        nextStep();
    }

    void continueTest()
    {
        Mutex::Lock lock(_mutex);

        if (_nextStep == 3) {
            _stream.flush();

            String result = _stream.str();

            String expectedResult;
            for (int i = 0; i < 3; i++) {
                expectedResult += "line " + std::to_string(i);
                if (_writeLine)
                    expectedResult += "\n";
            }

            REQUIRE(result == expectedResult);
        } else {
            REQUIRE(_timeoutCounter < 10);

            _timeoutCounter++;

            CONTINUE_SECTION_AFTER_RUN_SECONDS_WITH(
                0.5,
                strongMethod(
                    this,
                    &AsyncStdioWriter_OneAtTheTimeTestContext::continueTest));
        }
    }

    std::basic_stringstream<CharType> _stream;
    P<AsyncStdioWriter<CharType>> _pWriter;

    int _nextStep = 0;

    int _timeoutCounter = 0;

    Mutex _mutex;
    bool _writeLine;
};

template <typename CharType>
class AsyncStdioWriter_AllAtOnceTestContext : public Base
{
  public:
    AsyncStdioWriter_AllAtOnceTestContext(bool writeLine)
    {
        _pWriter = newObj<AsyncStdioWriter<CharType>>(&_stream);

        _writeLine = writeLine;
    }

    void startTest()
    {
        for (int i = 0; i < 100; i++) {
            String textToWrite = "line " + std::to_string(i);

            P<IAsyncOp<void>> pOp;
            if (_writeLine)
                pOp = _pWriter->writeLine(textToWrite);
            else
                pOp = _pWriter->write(textToWrite);

            pOp->onDone() += weakMethod(
                this,
                &AsyncStdioWriter_AllAtOnceTestContext<CharType>::writeDone);
        }

        CONTINUE_SECTION_AFTER_RUN_SECONDS_WITH(
            0.5,
            strongMethod(this,
                         &AsyncStdioWriter_AllAtOnceTestContext::continueTest));
    }

    void writeDone(IAsyncOp<void> *pOp)
    {
        Mutex::Lock lock(_mutex);

        _doneCount++;
    }

    void continueTest()
    {
        Mutex::Lock lock(_mutex);

        if (_doneCount >= 100) {
            REQUIRE(_doneCount == 100);

            _stream.flush();

            String result = _stream.str();

            String expectedResult;
            for (int i = 0; i < 100; i++) {
                expectedResult += "line " + std::to_string(i);
                if (_writeLine)
                    expectedResult += "\n";
            }

            REQUIRE(result == expectedResult);
        } else {
            REQUIRE(_timeoutCounter < 30);

            _timeoutCounter++;

            CONTINUE_SECTION_AFTER_RUN_SECONDS_WITH(
                0.5, strongMethod(
                         this,
                         &AsyncStdioWriter_AllAtOnceTestContext::continueTest));
        }
    }

    std::basic_stringstream<CharType> _stream;
    P<AsyncStdioWriter<CharType>> _pWriter;

    int _doneCount = 0;

    int _timeoutCounter = 0;

    Mutex _mutex;
    bool _writeLine;
};

template <typename CharType> void verifyAsyncStdioWriter(bool writeLine)
{
    SECTION("one at a time")
    {
        P<AsyncStdioWriter_OneAtTheTimeTestContext<CharType>> pContext =
            newObj<AsyncStdioWriter_OneAtTheTimeTestContext<CharType>>(
                writeLine);

        pContext->startTest();
    }

    SECTION("all at once")
    {
        P<AsyncStdioWriter_AllAtOnceTestContext<CharType>> pContext =
            newObj<AsyncStdioWriter_AllAtOnceTestContext<CharType>>(writeLine);

        pContext->startTest();
    }
}

template <typename CharType> void testAsyncStdioWriter()
{
    SECTION("write")
    verifyAsyncStdioWriter<CharType>(false);

    SECTION("writeLine")
    verifyAsyncStdioWriter<CharType>(true);
}

TEST_CASE("AsyncStdioWriter")
{
    SECTION("char") { testAsyncStdioWriter<char>(); }

    SECTION("wchar_t") { testAsyncStdioWriter<wchar_t>(); }
}
