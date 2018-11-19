#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/AsyncOpRunnable.h>
#include <bdn/Signal.h>

using namespace bdn;

#if BDN_HAVE_THREADS

class TestAsyncOpError : public std::exception
{
};

class TestAsyncOpRunnable : public AsyncOpRunnable<String>
{
  public:
    Signal _proceedSignal;
    Signal _startedSignal;
    bool _throwException = false;

  protected:
    String doOp() override
    {
        _startedSignal.set();
        _proceedSignal.wait();

        if (_throwException)
            throw TestAsyncOpError();

        return "hello";
    }
};

class TestAsyncOpData : public Base
{
  public:
    int done1CallCount = 0;
    int done2CallCount = 0;
    int done3CallCount = 0;
};

void verifyAsyncOpRunnable(bool withDoneListeners)
{
    P<TestAsyncOpRunnable> runnable = newObj<TestAsyncOpRunnable>();

    P<TestAsyncOpData> testData = newObj<TestAsyncOpData>();

    if (withDoneListeners) {
        runnable->onDone() += [testData](IAsyncOp<String> *op) { testData->done1CallCount++; };
        runnable->onDone() += [testData](IAsyncOp<String> *op) { testData->done2CallCount++; };
    }

    SECTION("no exception")
    {
        REQUIRE_THROWS_AS(runnable->getResult(), UnfinishedError);
        REQUIRE(!runnable->isDone());

        Thread thread(runnable);

        REQUIRE(testData->done1CallCount == 0);
        REQUIRE(testData->done2CallCount == 0);

        try {
            REQUIRE(runnable->_startedSignal.wait(5000));
            REQUIRE(testData->done1CallCount == 0);
            REQUIRE(testData->done2CallCount == 0);

            REQUIRE_THROWS_AS(runnable->getResult(), UnfinishedError);
            REQUIRE(testData->done1CallCount == 0);
            REQUIRE(testData->done2CallCount == 0);

            runnable->_proceedSignal.set();

            thread.join(Thread::ExceptionThrow);

            // the done listeners should still not have been called. The
            // notifications are processed asynchronously
            REQUIRE(testData->done1CallCount == 0);
            REQUIRE(testData->done2CallCount == 0);

            CONTINUE_SECTION_WHEN_IDLE(runnable, testData, withDoneListeners)
            {
                REQUIRE(testData->done1CallCount == (withDoneListeners ? 1 : 0));
                REQUIRE(testData->done2CallCount == (withDoneListeners ? 1 : 0));

                String result = runnable->getResult();

                REQUIRE(result == "hello");
            };
        }
        catch (...) {
            runnable->_proceedSignal.set();
            throw;
        }
    }

    SECTION("exception")
    {
        runnable->_throwException = true;
        runnable->_proceedSignal.set();

        Thread thread(runnable);

        thread.join(Thread::ExceptionThrow);

        CONTINUE_SECTION_WHEN_IDLE(runnable, testData, withDoneListeners)
        {
            REQUIRE(testData->done1CallCount == (withDoneListeners ? 1 : 0));
            REQUIRE(testData->done2CallCount == (withDoneListeners ? 1 : 0));

            REQUIRE(runnable->isDone());
            REQUIRE_THROWS_AS(runnable->getResult(), TestAsyncOpError);
            REQUIRE(runnable->isDone());
        };
    }

    SECTION("abort")
    {
        SECTION("before start")
        {
            runnable->signalStop();

            // done listeners should NOT have been called immediately. The
            // notifier must call them asynchronously to ensure that no mutexes
            // or other resources are locked when the subscribed functions are
            // called.
            REQUIRE(testData->done1CallCount == 0);
            REQUIRE(testData->done2CallCount == 0);

            REQUIRE(runnable->isDone());
            REQUIRE_THROWS_AS(runnable->getResult(), AbortedError);

            // but done listener notification should have been scheduled
            CONTINUE_SECTION_WHEN_IDLE(runnable, testData, withDoneListeners)
            {
                REQUIRE(testData->done1CallCount == (withDoneListeners ? 1 : 0));
                REQUIRE(testData->done2CallCount == (withDoneListeners ? 1 : 0));

                REQUIRE(runnable->isDone());
                REQUIRE_THROWS_AS(runnable->getResult(), AbortedError);

                runnable->_proceedSignal.set();

                Thread thread(runnable);

                thread.join(Thread::ExceptionThrow);

                // doOp() should NOT have been called
                REQUIRE(runnable->_startedSignal.isSet() == false);

                // should be done and throw an AbortedError
                REQUIRE(runnable->isDone());

                CONTINUE_SECTION_WHEN_IDLE(runnable, testData, withDoneListeners)
                {
                    // still same call count
                    REQUIRE(testData->done1CallCount == (withDoneListeners ? 1 : 0));
                    REQUIRE(testData->done2CallCount == (withDoneListeners ? 1 : 0));

                    REQUIRE_THROWS_AS(runnable->getResult(), AbortedError);
                };
            };
        }

        SECTION("after start")
        {
            Thread thread(runnable);

            // runnable should have started, but wait for proceedSignal
            REQUIRE(runnable->_startedSignal.wait(5000));
            REQUIRE(!runnable->isDone());

            // now signal stop
            runnable->signalStop();

            // let runnable continue
            runnable->_proceedSignal.set();

            thread.join(Thread::ExceptionThrow);

            // should have finished successfully (i.e. signalStop should not
            // have had any effect)
            REQUIRE(runnable->isDone());
            REQUIRE(runnable->getResult() == "hello");

            // notifications should be called asynchronously
            REQUIRE(testData->done1CallCount == 0);
            REQUIRE(testData->done2CallCount == 0);

            CONTINUE_SECTION_WHEN_IDLE(runnable, testData, withDoneListeners)
            {
                REQUIRE(testData->done1CallCount == (withDoneListeners ? 1 : 0));
                REQUIRE(testData->done2CallCount == (withDoneListeners ? 1 : 0));
            };
        }
    }

    if (withDoneListeners) {
        SECTION("done listener added directly after finish")
        {
            Thread thread(runnable);

            runnable->_proceedSignal.set();

            thread.join(Thread::ExceptionThrow);

            String result = runnable->getResult();

            REQUIRE(result == "hello");

            runnable->onDone() += [testData](IAsyncOp<String> *op) {
                REQUIRE(op->getResult() == "hello");

                testData->done3CallCount++;
            };

            // should not be called immediately
            REQUIRE(testData->done3CallCount == 0);

            CONTINUE_SECTION_WHEN_IDLE(testData)
            {
                // now it should have been called
                REQUIRE(testData->done3CallCount == 1);
            };
        }

        SECTION("done listener added after initial notifications")
        {
            Thread thread(runnable);

            runnable->_proceedSignal.set();

            thread.join(Thread::ExceptionThrow);

            String result = runnable->getResult();

            REQUIRE(result == "hello");

            // let all pending events be processed
            CONTINUE_SECTION_WHEN_IDLE(testData, runnable)
            {
                // then add a new listener
                runnable->onDone() += [testData](IAsyncOp<String> *op) {
                    REQUIRE(op->getResult() == "hello");

                    testData->done3CallCount++;
                };

                // should not be called immediately
                REQUIRE(testData->done3CallCount == 0);

                CONTINUE_SECTION_WHEN_IDLE(testData)
                {
                    // now it should have been called
                    REQUIRE(testData->done3CallCount == 1);
                };
            };
        }
    }
}

TEST_CASE("AsyncOpRunnable")
{
    SECTION("without done listener")
    verifyAsyncOpRunnable(false);

    SECTION("with done listener")
    verifyAsyncOpRunnable(true);
}

#endif
