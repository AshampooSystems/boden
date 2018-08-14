#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/SimpleNotifier.h>
#include <bdn/Array.h>

using namespace bdn;

class SimpleNotifierTestSubscriptionData : public Base
{
public:
    SimpleNotifierTestSubscriptionData( bool* pDeleted )
    : _pDeleted(pDeleted)
    {
    }
    
    ~SimpleNotifierTestSubscriptionData()
    {
        *_pDeleted = true;
    }
    
private:
    bool* _pDeleted;
};

TEST_CASE("SimpleNotifier")
{
    P<SimpleNotifier<String> > pNotifier = newObj<SimpleNotifier<String> >();
    
    SECTION("empty")
    {
        // here we simply verify that no crash happens
        pNotifier->notify("bla");
    }
    
    SECTION("one subscriber")
    {
        Array<String> gotParam;
        
        bool subscriptionDataDeleted=false;
        P<SimpleNotifierTestSubscriptionData> pTestSubscriptionData = newObj<SimpleNotifierTestSubscriptionData>(&subscriptionDataDeleted);
        
        P<INotifierSubscription> pSub = pNotifier->subscribe(
            [&gotParam, pTestSubscriptionData](String param)
            {
                gotParam.add( param );
            } );
        
        pTestSubscriptionData = nullptr;
        // the subscription data should still be alive because it was
        // captured
        REQUIRE( !subscriptionDataDeleted );
        
        SECTION("single notify")
        {
            pNotifier->notify("hello");
            REQUIRE( gotParam == Array<String>{"hello"} );
            REQUIRE( !subscriptionDataDeleted );
        }
        
        SECTION("double notify")
        {
            pNotifier->notify("hello");
            REQUIRE( gotParam == Array<String>{"hello"} );
            pNotifier->notify("world");
            REQUIRE( (gotParam == Array<String>{"hello", "world"}) );
            REQUIRE( !subscriptionDataDeleted );
        }
        
        SECTION("unsubscribe")
        {
            pNotifier->unsubscribe(pSub);
            
            // subscription data should have been deleted
            REQUIRE( subscriptionDataDeleted );
            
            pNotifier->notify("hello");
            REQUIRE( gotParam == Array<String>{}  );
        }
        
        SECTION("unsubscribeAll")
        {
            pNotifier->unsubscribeAll();
            
            // subscription data should have been deleted
            REQUIRE( subscriptionDataDeleted );
            
            pNotifier->notify("hello");
            REQUIRE( gotParam == Array<String>{} );
        }
    }
    
    SECTION("subscribeParamless")
    {
        int callCount=0;
        
        bool subscriptionDataDeleted=false;
        P<SimpleNotifierTestSubscriptionData> pTestSubscriptionData = newObj<SimpleNotifierTestSubscriptionData>(&subscriptionDataDeleted);
        
        P<INotifierSubscription> pSub = pNotifier->subscribeParamless(
            [&callCount, pTestSubscriptionData]()
            {
                callCount++;
            } );
        
        pTestSubscriptionData = nullptr;
        // the subscription data should still be alive because it was
        // captured
        REQUIRE( !subscriptionDataDeleted );
        
        SECTION("single notify")
        {
            pNotifier->notify("hello");
            REQUIRE( callCount == 1);
            REQUIRE( !subscriptionDataDeleted );
        }
        
        SECTION("double notify")
        {
            pNotifier->notify("hello");
            pNotifier->notify("world");
            REQUIRE( callCount == 2 );
            REQUIRE( !subscriptionDataDeleted );
        }
        
        SECTION("unsubscribe")
        {
            pNotifier->unsubscribe(pSub);
            
            // subscription data should have been deleted
            REQUIRE( subscriptionDataDeleted );
            
            pNotifier->notify("hello");
            REQUIRE( callCount == 0);
        }
        
        SECTION("unsubscribeAll")
        {
            pNotifier->unsubscribeAll();
            
            // subscription data should have been deleted
            REQUIRE( subscriptionDataDeleted );
            
            pNotifier->notify("hello");
            REQUIRE( callCount == 0);
        }
    }
    
    SECTION("multiple subscribers")
    {
        Array<String> gotParam1;
        Array<String> gotParam2;
        Array<String> gotParam3;
        
        bool subscriptionData1Deleted=false;
        P<SimpleNotifierTestSubscriptionData> pTestSubscriptionData1 = newObj<SimpleNotifierTestSubscriptionData>(&subscriptionData1Deleted);
        
        bool subscriptionData2Deleted=false;
        P<SimpleNotifierTestSubscriptionData> pTestSubscriptionData2 = newObj<SimpleNotifierTestSubscriptionData>(&subscriptionData2Deleted);
        
        bool subscriptionData3Deleted=false;
        P<SimpleNotifierTestSubscriptionData> pTestSubscriptionData3 = newObj<SimpleNotifierTestSubscriptionData>(&subscriptionData3Deleted);
        
        P<INotifierSubscription> pSub1 = pNotifier->subscribe(
            [&gotParam1, pTestSubscriptionData1](String param)
            {
               gotParam1.add( param );
            } );
        
        P<INotifierSubscription> pSub2 = pNotifier->subscribe(
            [&gotParam2, pTestSubscriptionData2](String param)
            {
               gotParam2.add( param );
            } );
        
        P<INotifierSubscription> pSub3 = pNotifier->subscribe(
            [&gotParam3, pTestSubscriptionData3](String param)
            {
               gotParam3.add( param );
            } );
        
        
        pTestSubscriptionData1 = nullptr;
        pTestSubscriptionData2 = nullptr;
        pTestSubscriptionData3 = nullptr;
        // the subscription data should still be alive because it was
        // captured
        REQUIRE( !subscriptionData1Deleted );
        REQUIRE( !subscriptionData2Deleted );
        REQUIRE( !subscriptionData3Deleted );
        
        SECTION("single notify")
        {
            pNotifier->notify("hello");
            REQUIRE( gotParam1 == Array<String>{"hello"} );
            REQUIRE( gotParam2 == Array<String>{"hello"} );
            REQUIRE( gotParam3 == Array<String>{"hello"} );
            REQUIRE( !subscriptionData1Deleted );
            REQUIRE( !subscriptionData2Deleted );
            REQUIRE( !subscriptionData3Deleted );
        }
        
        SECTION("double notify")
        {
            pNotifier->notify("hello");
            REQUIRE( gotParam1 == Array<String>{"hello"} );
            REQUIRE( gotParam2 == Array<String>{"hello"} );
            REQUIRE( gotParam3 == Array<String>{"hello"} );
            
            pNotifier->notify("world");
            REQUIRE( (gotParam1 == Array<String>{"hello", "world"}) );
            REQUIRE( (gotParam2 == Array<String>{"hello", "world"}) );
            REQUIRE( (gotParam3 == Array<String>{"hello", "world"}) );
            
            REQUIRE( !subscriptionData1Deleted );
            REQUIRE( !subscriptionData2Deleted );
            REQUIRE( !subscriptionData3Deleted );
        }
        
        SECTION("unsubscribe")
        {
            pNotifier->unsubscribe(pSub2);
            
            // subscription data should have been deleted
            REQUIRE( subscriptionData2Deleted );
            // the other two should still be intact
            REQUIRE( !subscriptionData1Deleted );
            REQUIRE( !subscriptionData3Deleted );
            
            pNotifier->notify("hello");
            REQUIRE( (gotParam1 == Array<String>{"hello"}) );
            REQUIRE( (gotParam2 == Array<String>{}) );
            REQUIRE( (gotParam3 == Array<String>{"hello"}) );
        }
        
        SECTION("unsubscribeAll")
        {
            pNotifier->unsubscribeAll();
            
            // subscription data should have been deleted
            REQUIRE( subscriptionData1Deleted );
            REQUIRE( subscriptionData2Deleted );
            REQUIRE( subscriptionData3Deleted );
            
            pNotifier->notify("hello");
            REQUIRE( gotParam1.isEmpty() );
            REQUIRE( gotParam2.isEmpty() );
            REQUIRE( gotParam3.isEmpty() );
        }
    }
    
    
    SECTION("notify from inside notify")
    {
        Array<String> gotParam1;
        Array<String> gotParam2;
        Array<String> gotParam3;
        
        bool subscriptionData1Deleted=false;
        P<SimpleNotifierTestSubscriptionData> pTestSubscriptionData1 = newObj<SimpleNotifierTestSubscriptionData>(&subscriptionData1Deleted);
        
        bool subscriptionData2Deleted=false;
        P<SimpleNotifierTestSubscriptionData> pTestSubscriptionData2 = newObj<SimpleNotifierTestSubscriptionData>(&subscriptionData2Deleted);
        
        bool subscriptionData3Deleted=false;
        P<SimpleNotifierTestSubscriptionData> pTestSubscriptionData3 = newObj<SimpleNotifierTestSubscriptionData>(&subscriptionData3Deleted);
        
        P<INotifierSubscription> pSub1 = pNotifier->subscribe(
            [&gotParam1, pTestSubscriptionData1](String param)
            {
               gotParam1.add( param );
            } );
        
        P<INotifierSubscription> pSub2 = pNotifier->subscribe(
            [&gotParam2, pTestSubscriptionData2, pNotifier](String param)
            {
               gotParam2.add( param );
                
               if(gotParam2.size()==1)
                   pNotifier->notify("world");
            } );
        
        P<INotifierSubscription> pSub3 = pNotifier->subscribe(
            [&gotParam3, pTestSubscriptionData3](String param)
            {
                gotParam3.add( param );
            } );
        
        
        pTestSubscriptionData1 = nullptr;
        pTestSubscriptionData2 = nullptr;
        pTestSubscriptionData3 = nullptr;
        // the subscription data should still be alive because it was
        // captured
        REQUIRE( !subscriptionData1Deleted );
        REQUIRE( !subscriptionData2Deleted );
        REQUIRE( !subscriptionData3Deleted );
        
        
        pNotifier->notify("hello");
        REQUIRE( !subscriptionData1Deleted );
        REQUIRE( !subscriptionData2Deleted );
        REQUIRE( !subscriptionData3Deleted );
        
        REQUIRE( (gotParam1 == Array<String>{"hello", "world"}) );
        REQUIRE( (gotParam2 == Array<String>{"hello", "world"}) );
        // for 3 the order should be reversed, since the second notification
        // is executed during the first notification
        REQUIRE( (gotParam3 == Array<String>{"world", "hello"}) );
        
    }
    
    SECTION("unsubscribe from inside notify")
    {
        Array<String> gotParam1;
        Array<String> gotParam2;
        Array<String> gotParam3;
        
        bool subscriptionData1Deleted=false;
        P<SimpleNotifierTestSubscriptionData> pTestSubscriptionData1 = newObj<SimpleNotifierTestSubscriptionData>(&subscriptionData1Deleted);
        
        bool subscriptionData2Deleted=false;
        P<SimpleNotifierTestSubscriptionData> pTestSubscriptionData2 = newObj<SimpleNotifierTestSubscriptionData>(&subscriptionData2Deleted);
        
        bool subscriptionData3Deleted=false;
        P<SimpleNotifierTestSubscriptionData> pTestSubscriptionData3 = newObj<SimpleNotifierTestSubscriptionData>(&subscriptionData3Deleted);
        
        SECTION("unsub following")
        {
            P<INotifierSubscription> pSub1 = pNotifier->subscribe(
                [&gotParam1, pTestSubscriptionData1](String param)
                {
                   gotParam1.add( param );
                } );
            
            P<INotifierSubscription> pSub3;
            
            P<INotifierSubscription> pSub2 = pNotifier->subscribe(
                [&gotParam2, pTestSubscriptionData2, &pSub3, &pNotifier](String param)
                {
                   gotParam2.add( param );
                    
                   pNotifier->unsubscribe(pSub3);
                } );
            
            pSub3 = pNotifier->subscribe(
                [&gotParam3, pTestSubscriptionData3](String param)
                {
                    gotParam3.add( param );
                } );
            
            
            pTestSubscriptionData1 = nullptr;
            pTestSubscriptionData2 = nullptr;
            pTestSubscriptionData3 = nullptr;
            // the subscription data should still be alive because it was
            // captured
            REQUIRE( !subscriptionData1Deleted );
            REQUIRE( !subscriptionData2Deleted );
            REQUIRE( !subscriptionData3Deleted );
            
            
            pNotifier->notify("hello");
            REQUIRE( !subscriptionData1Deleted );
            REQUIRE( !subscriptionData2Deleted );
            // 3 has been unsubscribed
            REQUIRE( subscriptionData3Deleted );
            
            pNotifier->notify("world");
            REQUIRE( !subscriptionData1Deleted );
            REQUIRE( !subscriptionData2Deleted );
            REQUIRE( subscriptionData3Deleted );
            
            REQUIRE( (gotParam1 == Array<String>{"hello", "world"}) );
            REQUIRE( (gotParam2 == Array<String>{"hello", "world"}) );
            // 3 should not have been called
            REQUIRE( gotParam3.isEmpty() );
        }
                    
        SECTION("unsub current")
        {
            P<INotifierSubscription> pSub1 = pNotifier->subscribe(
                [&gotParam1, pTestSubscriptionData1](String param)
                {
                   gotParam1.add( param );
                } );
            
            P<INotifierSubscription> pSub2;
            
            pSub2 = pNotifier->subscribe(
                [&gotParam2, pTestSubscriptionData2, &pSub2, &pNotifier](String param)
                {
                   gotParam2.add( param );
                    
                   pNotifier->unsubscribe(pSub2);
                } );
            
            P<INotifierSubscription> pSub3 = pNotifier->subscribe(
                [&gotParam3, pTestSubscriptionData3](String param)
                {
                    gotParam3.add( param );
                } );
            
            
            pTestSubscriptionData1 = nullptr;
            pTestSubscriptionData2 = nullptr;
            pTestSubscriptionData3 = nullptr;
            // the subscription data should still be alive because it was
            // captured
            REQUIRE( !subscriptionData1Deleted );
            REQUIRE( !subscriptionData2Deleted );
            REQUIRE( !subscriptionData3Deleted );
            
            
            pNotifier->notify("hello");
            REQUIRE( !subscriptionData1Deleted );
            // 3 should have been unsubscribed
            REQUIRE( subscriptionData2Deleted );
            REQUIRE( !subscriptionData3Deleted );
            
            pNotifier->notify("world");
            REQUIRE( !subscriptionData1Deleted );
            REQUIRE( subscriptionData2Deleted );
            REQUIRE( !subscriptionData3Deleted );
            
            // the unsubscribe should not have had any effect on the initial notification.
            // But the second one should not have been called.
            REQUIRE( (gotParam1 == Array<String>{"hello", "world"}) );
            REQUIRE( (gotParam2 == Array<String>{"hello"}) );
            REQUIRE( (gotParam3 == Array<String>{"hello", "world"}) );
        }
        
        SECTION("unsub all")
        {
            P<INotifierSubscription> pSub1 = pNotifier->subscribe(
                [&gotParam1, pTestSubscriptionData1](String param)
                {
                   gotParam1.add( param );
                } );
            
            P<INotifierSubscription> pSub2 = pNotifier->subscribe(
                [&gotParam2, pTestSubscriptionData2, pNotifier](String param)
                {
                   gotParam2.add( param );
                    
                   pNotifier->unsubscribeAll();
                } );
            
            P<INotifierSubscription> pSub3 = pNotifier->subscribe(
                [&gotParam3, pTestSubscriptionData3](String param)
                {
                    gotParam3.add( param );
                } );
            
            pTestSubscriptionData1 = nullptr;
            pTestSubscriptionData2 = nullptr;
            pTestSubscriptionData3 = nullptr;
            // the subscription data should still be alive because it was
            // captured
            REQUIRE( !subscriptionData1Deleted );
            REQUIRE( !subscriptionData2Deleted );
            REQUIRE( !subscriptionData3Deleted );
            
            
            pNotifier->notify("hello");
            REQUIRE( subscriptionData1Deleted );
            REQUIRE( subscriptionData2Deleted );
            REQUIRE( subscriptionData3Deleted );
            
            pNotifier->notify("world");
            REQUIRE( subscriptionData1Deleted );
            REQUIRE( subscriptionData2Deleted );
            REQUIRE( subscriptionData3Deleted );
            
            REQUIRE( (gotParam1 == Array<String>{"hello"}) );
            REQUIRE( (gotParam2 == Array<String>{"hello"}) );
            REQUIRE( (gotParam3 == Array<String>{}) );
        }
                    
                    
    }
}





