#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/AsyncOpRunnable.h>
#include <bdn/Signal.h>

using namespace bdn;


class TestAsyncOpError : public std::exception
{
};

class TestAsyncOpRunnable : public AsyncOpRunnable<String>
{
public:

    Signal _proceedSignal;
    Signal _startedSignal;
    bool   _throwException = false;

protected:
    String doOp() override
    {
        _startedSignal.set();
        _proceedSignal.wait();

        if(_throwException)
            throw TestAsyncOpError();

        return "hello";
    }
};


void verifyAsyncOpRunnable(bool withDoneListeners)
{
    P<TestAsyncOpRunnable> pRunnable = newObj<TestAsyncOpRunnable>();

    int done1CallCount = 0;
    int done2CallCount = 0;

    if(withDoneListeners)
    {
        pRunnable->onDone() +=
            [&done1CallCount](IAsyncOp<String>* pOp)
            {
                done1CallCount++;
            };
        pRunnable->onDone() +=
            [&done2CallCount](IAsyncOp<String>* pOp)
            {
                done2CallCount++;
            };
    }

    SECTION("no exception")
    {		
        REQUIRE_THROWS_AS( pRunnable->getResult(), UnfinishedError );
        REQUIRE( !pRunnable->isDone() );

        Thread thread( pRunnable );

        REQUIRE(done1CallCount==0);
        REQUIRE(done2CallCount==0);

        try
        {
            REQUIRE( pRunnable->_startedSignal.wait(5000) );
            REQUIRE(done1CallCount==0);
            REQUIRE(done2CallCount==0);

            
            REQUIRE_THROWS_AS( pRunnable->getResult(), UnfinishedError );
            REQUIRE(done1CallCount==0);
            REQUIRE(done2CallCount==0);

            pRunnable->_proceedSignal.set();

            thread.join( Thread::ExceptionThrow );

            REQUIRE(done1CallCount == (withDoneListeners ? 1 : 0) );
            REQUIRE(done2CallCount == (withDoneListeners ? 1 : 0) );

            String result = pRunnable->getResult();

            REQUIRE( result=="hello" );            
        }
        catch(...)
        {
            pRunnable->_proceedSignal.set();
            throw;
        }
    }

    SECTION("exception")
    {
        pRunnable->_throwException = true;
        pRunnable->_proceedSignal.set();

        Thread thread( pRunnable );

        thread.join( Thread::ExceptionThrow );

        REQUIRE(done1CallCount == (withDoneListeners ? 1 : 0) );
        REQUIRE(done2CallCount == (withDoneListeners ? 1 : 0) );
        
        REQUIRE( pRunnable->isDone() );        
        REQUIRE_THROWS_AS( pRunnable->getResult(), TestAsyncOpError);
        REQUIRE( pRunnable->isDone() );
    }


    SECTION("abort")
    {
        SECTION("before start")
        {
            pRunnable->signalStop();
            
            // done listeners should have been notified immediately
            REQUIRE(done1CallCount == (withDoneListeners ? 1 : 0));
            REQUIRE(done2CallCount == (withDoneListeners ? 1 : 0) );

            REQUIRE( pRunnable->isDone() );
            REQUIRE_THROWS_AS( pRunnable->getResult(), AbortedError );

            pRunnable->_proceedSignal.set();

            Thread thread( pRunnable );

            thread.join( Thread::ExceptionThrow );

            // doOp() should NOT have been called
            REQUIRE( pRunnable->_startedSignal.isSet() == false );
        
            // should be done and throw an AbortedError
            REQUIRE( pRunnable->isDone() );

            REQUIRE(done1CallCount == (withDoneListeners ? 1 : 0) );
            REQUIRE(done2CallCount == (withDoneListeners ? 1 : 0) );

            REQUIRE_THROWS_AS( pRunnable->getResult(), AbortedError );
        }

        SECTION("after start")
        {
            Thread thread( pRunnable );

            // runnable should have started, but wait for proceedSignal
            REQUIRE( pRunnable->_startedSignal.wait(5000) );
            REQUIRE( !pRunnable->isDone() );

            // now signal stop
            pRunnable->signalStop();

            // let runnable continue
            pRunnable->_proceedSignal.set();

            thread.join( Thread::ExceptionThrow );

            // should have finished successfully (i.e. signalStop should not have had any effect)
            REQUIRE( pRunnable->isDone() );
            REQUIRE( pRunnable->getResult()=="hello");
            
            REQUIRE(done1CallCount == (withDoneListeners ? 1 : 0) );
            REQUIRE(done2CallCount == (withDoneListeners ? 1 : 0) );           
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

