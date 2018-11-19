#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/AsyncStdioReader.h>
#include <bdn/Array.h>

using namespace bdn;

template <typename CharType> class AsyncStdioReader_OneAtTheTimeTestContext : public Base
{
  public:
    AsyncStdioReader_OneAtTheTimeTestContext()
    {
        _stream << "line 1" << std::endl << "line 2" << std::endl << "line 3";

        _reader = newObj<AsyncStdioReader<CharType>>(&_stream);
    }

    void startTest()
    {
        nextStep();

        CONTINUE_SECTION_AFTER_RUN_SECONDS_WITH(
            0.5, strongMethod(this, &AsyncStdioReader_OneAtTheTimeTestContext::continueTest));
    }

    void nextStep()
    {
        if (_nextStep < 3) {
            _nextStep++;

            P<IAsyncOp<String>> op = _reader->readLine();
            op->onDone() += weakMethod(this, &AsyncStdioReader_OneAtTheTimeTestContext<CharType>::readDone);
        }
    }

    void readDone(IAsyncOp<String> *op)
    {
        Mutex::Lock lock(_mutex);

        _results.push_back(op->getResult());
        nextStep();
    }

    void continueTest()
    {
        Mutex::Lock lock(_mutex);

        if (_results.size() == 3) {
            REQUIRE(_nextStep == 3);
            REQUIRE(_results.size() == 3);

            REQUIRE(_results[0] == "line 1");
            REQUIRE(_results[1] == "line 2");
            REQUIRE(_results[2] == "line 3");
        } else {
            REQUIRE(_timeoutCounter < 10);

            _timeoutCounter++;

            CONTINUE_SECTION_AFTER_RUN_SECONDS_WITH(
                0.5, strongMethod(this, &AsyncStdioReader_OneAtTheTimeTestContext::continueTest));
        }
    }

    std::basic_stringstream<CharType> _stream;
    P<AsyncStdioReader<CharType>> _reader;

    int _nextStep = 0;
    Array<String> _results;

    int _timeoutCounter = 0;

    Mutex _mutex;
};

template <typename CharType> class AsyncStdioReader_AllAtOnceTestContext : public Base
{
  public:
    AsyncStdioReader_AllAtOnceTestContext()
    {
        for (int i = 0; i < 100; i++) {
            if (i > 0)
                _stream << std::endl;
            _stream << "line " << i;
        }

        _reader = newObj<AsyncStdioReader<CharType>>(&_stream);
    }

    void startTest()
    {
        for (int i = 0; i < 100; i++) {
            P<IAsyncOp<String>> op = _reader->readLine();
            op->onDone() += weakMethod(this, &AsyncStdioReader_AllAtOnceTestContext<CharType>::readDone);
        }

        CONTINUE_SECTION_AFTER_RUN_SECONDS_WITH(
            0.5, strongMethod(this, &AsyncStdioReader_AllAtOnceTestContext::continueTest));
    }

    void readDone(IAsyncOp<String> *op)
    {
        Mutex::Lock lock(_mutex);

        _results.push_back(op->getResult());
    }

    void continueTest()
    {
        Mutex::Lock lock(_mutex);

        if (_results.size() >= 100) {
            REQUIRE(_results.size() == 100);

            for (int i = 0; i < 100; i++) {
                REQUIRE(_results[i] == "line " + std::to_string(i));
            }
        } else {
            REQUIRE(_timeoutCounter < 10);

            _timeoutCounter++;

            CONTINUE_SECTION_AFTER_RUN_SECONDS_WITH(
                0.5, strongMethod(this, &AsyncStdioReader_AllAtOnceTestContext::continueTest));
        }
    }

    std::basic_stringstream<CharType> _stream;
    P<AsyncStdioReader<CharType>> _reader;

    Array<String> _results;

    int _timeoutCounter = 0;

    Mutex _mutex;
};

template <typename CharType> void testAsyncStdioReader()
{
    SECTION("one at a time")
    {
        P<AsyncStdioReader_OneAtTheTimeTestContext<CharType>> context =
            newObj<AsyncStdioReader_OneAtTheTimeTestContext<CharType>>();

        context->startTest();
    }

    SECTION("all at once")
    {
        P<AsyncStdioReader_AllAtOnceTestContext<CharType>> context =
            newObj<AsyncStdioReader_AllAtOnceTestContext<CharType>>();

        context->startTest();
    }
}

TEST_CASE("AsyncStdioReader")
{
    SECTION("char") { testAsyncStdioReader<char>(); }

    SECTION("wchar_t") { testAsyncStdioReader<wchar_t>(); }
}
