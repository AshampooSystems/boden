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


class TestAsyncOpData : public Base
{
public:

    int done1CallCount = 0;
    int done2CallCount = 0;
    int done3CallCount = 0;

};

void verifyAsyncOpRunnable(bool withDoneListeners)
{
    P<TestAsyncOpRunnable>  pRunnable = newObj<TestAsyncOpRunnable>();

    P<TestAsyncOpData>      pTestData = newObj<TestAsyncOpData>();

    if(withDoneListeners)
    {
        pRunnable->onDone() +=
            [pTestData](IAsyncOp<String>* pOp)
            {
                pTestData->done1CallCount++;
            };
        pRunnable->onDone() +=
            [pTestData](IAsyncOp<String>* pOp)
            {
                pTestData->done2CallCount++;
            };
    }

    SECTION("no exception")
    {		
        REQUIRE_THROWS_AS( pRunnable->getResult(), UnfinishedError );
        REQUIRE( !pRunnable->isDone() );

        Thread thread( pRunnable );

        REQUIRE(pTestData->done1CallCount==0);
        REQUIRE(pTestData->done2CallCount==0);

        try
        {
            REQUIRE( pRunnable->_startedSignal.wait(5000) );
            REQUIRE(pTestData->done1CallCount==0);
            REQUIRE(pTestData->done2CallCount==0);

            
            REQUIRE_THROWS_AS( pRunnable->getResult(), UnfinishedError );
            REQUIRE(pTestData->done1CallCount==0);
            REQUIRE(pTestData->done2CallCount==0);

            pRunnable->_proceedSignal.set();

            thread.join( Thread::ExceptionThrow );

            // the done listeners should still not have been called. The notifications
            // are processed asynchronously
            REQUIRE(pTestData->done1CallCount==0);
            REQUIRE(pTestData->done2CallCount==0);

            CONTINUE_SECTION_WHEN_IDLE(pRunnable, pTestData, withDoneListeners )
            {
                REQUIRE(pTestData->done1CallCount == (withDoneListeners ? 1 : 0) );
                REQUIRE(pTestData->done2CallCount == (withDoneListeners ? 1 : 0) );

                String result = pRunnable->getResult();

                REQUIRE( result=="hello" );            
            };
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

        CONTINUE_SECTION_WHEN_IDLE( pRunnable, pTestData, withDoneListeners)
        {
            REQUIRE(pTestData->done1CallCount == (withDoneListeners ? 1 : 0) );
            REQUIRE(pTestData->done2CallCount == (withDoneListeners ? 1 : 0) );
        
            REQUIRE( pRunnable->isDone() );        
            REQUIRE_THROWS_AS( pRunnable->getResult(), TestAsyncOpError);
            REQUIRE( pRunnable->isDone() );
        };
    }


    SECTION("abort")
    {
        SECTION("before start")
        {
            pRunnable->signalStop();

            // done listeners should NOT have been called immediately. The notifier
            // must call them asynchronously to ensure that no mutexes or other resources
            // are locked when the subscribed functions are called.
            REQUIRE(pTestData->done1CallCount == 0 );
            REQUIRE(pTestData->done2CallCount == 0 );

            REQUIRE( pRunnable->isDone() );
            REQUIRE_THROWS_AS( pRunnable->getResult(), AbortedError );

            // but done listener notification should have been scheduled
            CONTINUE_SECTION_WHEN_IDLE( pRunnable, pTestData, withDoneListeners)
            {
                REQUIRE(pTestData->done1CallCount == (withDoneListeners ? 1 : 0) );
                REQUIRE(pTestData->done2CallCount == (withDoneListeners ? 1 : 0) );

                REQUIRE( pRunnable->isDone() );
                REQUIRE_THROWS_AS( pRunnable->getResult(), AbortedError );

                pRunnable->_proceedSignal.set();

                Thread thread( pRunnable );

                thread.join( Thread::ExceptionThrow );

                // doOp() should NOT have been called
                REQUIRE( pRunnable->_startedSignal.isSet() == false );
        
                // should be done and throw an AbortedError
                REQUIRE( pRunnable->isDone() );

                CONTINUE_SECTION_WHEN_IDLE( pRunnable, pTestData, withDoneListeners)
                {
                    // still same call count
                    REQUIRE(pTestData->done1CallCount == (withDoneListeners ? 1 : 0) );
                    REQUIRE(pTestData->done2CallCount == (withDoneListeners ? 1 : 0) );

                    REQUIRE_THROWS_AS( pRunnable->getResult(), AbortedError );
                };
            };
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

            // notifications should be called asynchronously
            REQUIRE(pTestData->done1CallCount == 0);
            REQUIRE(pTestData->done2CallCount == 0 );           

            CONTINUE_SECTION_WHEN_IDLE(pRunnable, pTestData, withDoneListeners)
            {            
                REQUIRE(pTestData->done1CallCount == (withDoneListeners ? 1 : 0) );
                REQUIRE(pTestData->done2CallCount == (withDoneListeners ? 1 : 0) );           
            };
        }
    }

    if(withDoneListeners)
    {
        SECTION("done listener added directly after finish")
        {		
            Thread thread( pRunnable );

            pRunnable->_proceedSignal.set();

            thread.join( Thread::ExceptionThrow );

            String result = pRunnable->getResult();

            REQUIRE( result=="hello" );           

            pRunnable->onDone() +=
                [pTestData](IAsyncOp<String>* pOp)
                {
                    REQUIRE( pOp->getResult()=="hello" );

                    pTestData->done3CallCount++;
                };

            // should not be called immediately
            REQUIRE(pTestData->done3CallCount==0);

            CONTINUE_SECTION_WHEN_IDLE(pTestData)
            {
                // now it should have been called
                REQUIRE(pTestData->done3CallCount==1);
            };
        }

        SECTION("done listener added after initial notifications")
        {		
            Thread thread( pRunnable );

            pRunnable->_proceedSignal.set();

            thread.join( Thread::ExceptionThrow );

            String result = pRunnable->getResult();

            REQUIRE( result=="hello" );           

            // let all pending events be processed
            CONTINUE_SECTION_WHEN_IDLE(pTestData, pRunnable)
            {
                // then add a new listener
                pRunnable->onDone() +=
                    [pTestData](IAsyncOp<String>* pOp)
                    {
                        REQUIRE( pOp->getResult()=="hello" );

                        pTestData->done3CallCount++;
                    };

                // should not be called immediately
                REQUIRE(pTestData->done3CallCount==0);

                CONTINUE_SECTION_WHEN_IDLE(pTestData)
                {
                    // now it should have been called
                    REQUIRE(pTestData->done3CallCount==1);
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


