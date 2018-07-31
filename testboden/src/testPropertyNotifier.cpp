#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/PropertyNotifier.h>
#include <bdn/Array.h>

using namespace bdn;

class PropertyNotifierTestSubscriptionData : public Base
{
public:
    PropertyNotifierTestSubscriptionData( bool* pDeleted )
    : _pDeleted(pDeleted)
    {
    }
    
    ~PropertyNotifierTestSubscriptionData()
    {
        *_pDeleted = true;
    }
    
private:
    bool* _pDeleted;
};

template<typename VALUE_TYPE>
class PropertyNotifierTestAccessor : public Base, BDN_IMPLEMENTS IPropertyReadAccessor<VALUE_TYPE>
{
public:
    
    PropertyNotifierTestAccessor( IPropertyNotifier<VALUE_TYPE>& notifier, const VALUE_TYPE& initialValue )
    : currVal(initialValue)
    , _notifier(notifier)
    {
    }
    
    VALUE_TYPE currVal;
    
    VALUE_TYPE      get() const override
    {
        return currVal;
    }
    
    IPropertyNotifier<VALUE_TYPE>&   changed() const override
    {
        return _notifier;
    }
    
private:
    IPropertyNotifier<VALUE_TYPE>& _notifier;
};

TEST_CASE("PropertyNotifier")
{
    P<PropertyNotifier<String> > pNotifier = newObj<PropertyNotifier<String> >();
    
    SECTION("enforces new allocation")
    {
        BDN_REQUIRE_THROWS_PROGRAMMING_ERROR( PropertyNotifier<int> notifier;  )
    }
    
    SECTION("empty")
    {
        // here we simply verify that no crash happens
        pNotifier->notify( PropertyNotifierTestAccessor<String>(*pNotifier, "bla") );
    }
    
    SECTION("one subscriber")
    {
        Array<String> gotParam;
        
        bool subscriptionDataDeleted=false;
        P<PropertyNotifierTestSubscriptionData> pTestSubscriptionData = newObj<PropertyNotifierTestSubscriptionData>(&subscriptionDataDeleted);
        
        P<INotifierSubControl> pSub = pNotifier->subscribe(
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
            pNotifier->notify( PropertyNotifierTestAccessor<String>(*pNotifier, "hello") );
            REQUIRE( gotParam == Array<String>{"hello"} );
            REQUIRE( !subscriptionDataDeleted );
        }
        
        SECTION("double notify")
        {
            pNotifier->notify( PropertyNotifierTestAccessor<String>(*pNotifier, "hello") );
            REQUIRE( gotParam == Array<String>{"hello"} );
            pNotifier->notify( PropertyNotifierTestAccessor<String>(*pNotifier, "world"));
            REQUIRE( (gotParam == Array<String>{"hello", "world"}) );
            REQUIRE( !subscriptionDataDeleted );
        }
        
        SECTION("unsubscribe")
        {
            pSub->unsubscribe();
            
            // subscription data should have been deleted
            REQUIRE( subscriptionDataDeleted );
            
            pNotifier->notify( PropertyNotifierTestAccessor<String>(*pNotifier, "hello") );
            REQUIRE( gotParam == Array<String>{}  );
        }
        
        SECTION("unsubscribeAll")
        {
            pNotifier->unsubscribeAll();
            
            // subscription data should have been deleted
            REQUIRE( subscriptionDataDeleted );
            
            pNotifier->notify( PropertyNotifierTestAccessor<String>(*pNotifier, "hello") );
            REQUIRE( gotParam == Array<String>{} );
        }
    }
    
    SECTION("subscribeParamless")
    {
        int callCount=0;
        
        bool subscriptionDataDeleted=false;
        P<PropertyNotifierTestSubscriptionData> pTestSubscriptionData = newObj<PropertyNotifierTestSubscriptionData>(&subscriptionDataDeleted);
        
        P<INotifierSubControl> pSub = pNotifier->subscribeParamless(
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
            pNotifier->notify(PropertyNotifierTestAccessor<String>(*pNotifier, "hello"));
            REQUIRE( callCount == 1);
            REQUIRE( !subscriptionDataDeleted );
        }
        
        SECTION("double notify")
        {
            pNotifier->notify(PropertyNotifierTestAccessor<String>(*pNotifier, "hello"));
            pNotifier->notify(PropertyNotifierTestAccessor<String>(*pNotifier, "world"));
            REQUIRE( callCount == 2 );
            REQUIRE( !subscriptionDataDeleted );
        }
        
        SECTION("unsubscribe")
        {
            pSub->unsubscribe();
            
            // subscription data should have been deleted
            REQUIRE( subscriptionDataDeleted );
            
            pNotifier->notify(PropertyNotifierTestAccessor<String>(*pNotifier, "hello"));
            REQUIRE( callCount == 0);
        }
        
        SECTION("unsubscribeAll")
        {
            pNotifier->unsubscribeAll();
            
            // subscription data should have been deleted
            REQUIRE( subscriptionDataDeleted );
            
            pNotifier->notify(PropertyNotifierTestAccessor<String>(*pNotifier, "hello"));
            REQUIRE( callCount == 0);
        }
    }
    
    SECTION("multiple subscribers")
    {
        Array<String> gotParam1;
        Array<String> gotParam2;
        Array<String> gotParam3;
        
        bool subscriptionData1Deleted=false;
        P<PropertyNotifierTestSubscriptionData> pTestSubscriptionData1 = newObj<PropertyNotifierTestSubscriptionData>(&subscriptionData1Deleted);
        
        bool subscriptionData2Deleted=false;
        P<PropertyNotifierTestSubscriptionData> pTestSubscriptionData2 = newObj<PropertyNotifierTestSubscriptionData>(&subscriptionData2Deleted);
        
        bool subscriptionData3Deleted=false;
        P<PropertyNotifierTestSubscriptionData> pTestSubscriptionData3 = newObj<PropertyNotifierTestSubscriptionData>(&subscriptionData3Deleted);
        
        P<INotifierSubControl> pSub1 = pNotifier->subscribe(
            [&gotParam1, pTestSubscriptionData1](String param)
            {
               gotParam1.add( param );
            } );
        
        P<INotifierSubControl> pSub2 = pNotifier->subscribe(
            [&gotParam2, pTestSubscriptionData2](String param)
            {
               gotParam2.add( param );
            } );
        
        P<INotifierSubControl> pSub3 = pNotifier->subscribe(
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
            pNotifier->notify(PropertyNotifierTestAccessor<String>(*pNotifier, "hello"));
            REQUIRE( gotParam1 == Array<String>{"hello"} );
            REQUIRE( gotParam2 == Array<String>{"hello"} );
            REQUIRE( gotParam3 == Array<String>{"hello"} );
            REQUIRE( !subscriptionData1Deleted );
            REQUIRE( !subscriptionData2Deleted );
            REQUIRE( !subscriptionData3Deleted );
        }
        
        SECTION("double notify")
        {
            pNotifier->notify(PropertyNotifierTestAccessor<String>(*pNotifier, "hello"));
            REQUIRE( gotParam1 == Array<String>{"hello"} );
            REQUIRE( gotParam2 == Array<String>{"hello"} );
            REQUIRE( gotParam3 == Array<String>{"hello"} );
            
            pNotifier->notify(PropertyNotifierTestAccessor<String>(*pNotifier, "world"));
            REQUIRE( (gotParam1 == Array<String>{"hello", "world"}) );
            REQUIRE( (gotParam2 == Array<String>{"hello", "world"}) );
            REQUIRE( (gotParam3 == Array<String>{"hello", "world"}) );
            
            REQUIRE( !subscriptionData1Deleted );
            REQUIRE( !subscriptionData2Deleted );
            REQUIRE( !subscriptionData3Deleted );
        }
        
        SECTION("unsubscribe")
        {
            pSub2->unsubscribe();
            
            // subscription data should have been deleted
            REQUIRE( subscriptionData2Deleted );
            // the other two should still be intact
            REQUIRE( !subscriptionData1Deleted );
            REQUIRE( !subscriptionData3Deleted );
            
            pNotifier->notify(PropertyNotifierTestAccessor<String>(*pNotifier, "hello"));
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
            
            pNotifier->notify(PropertyNotifierTestAccessor<String>(*pNotifier, "hello"));
            REQUIRE( gotParam1.isEmpty() );
            REQUIRE( gotParam2.isEmpty() );
            REQUIRE( gotParam3.isEmpty() );
        }
    }
    
    
    SECTION("notify with change in notification callback")
    {
        Array<String> gotParam1;
        Array<String> gotParam2;
        Array<String> gotParam3;
        
        bool subscriptionData1Deleted=false;
        P<PropertyNotifierTestSubscriptionData> pTestSubscriptionData1 = newObj<PropertyNotifierTestSubscriptionData>(&subscriptionData1Deleted);
        
        bool subscriptionData2Deleted=false;
        P<PropertyNotifierTestSubscriptionData> pTestSubscriptionData2 = newObj<PropertyNotifierTestSubscriptionData>(&subscriptionData2Deleted);
        
        bool subscriptionData3Deleted=false;
        P<PropertyNotifierTestSubscriptionData> pTestSubscriptionData3 = newObj<PropertyNotifierTestSubscriptionData>(&subscriptionData3Deleted);
        
        PropertyNotifierTestAccessor<String> accessor(*pNotifier, "");
        
        P<INotifierSubControl> pSub1 = pNotifier->subscribe(
            [&gotParam1, pTestSubscriptionData1](String param)
            {
               gotParam1.add( param );
            } );
        
        P<INotifierSubControl> pSub2 = pNotifier->subscribe(
            [&gotParam2, pTestSubscriptionData2, pNotifier, &accessor](String param)
            {
               gotParam2.add( param );
                
               if(gotParam2.size()==1)
               {
                   accessor.currVal = "world";
                   pNotifier->notify(accessor);
               }
            } );
        
        P<INotifierSubControl> pSub3 = pNotifier->subscribe(
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
        
        accessor.currVal = "hello";
        
        pNotifier->notify(accessor);
        REQUIRE( !subscriptionData1Deleted );
        REQUIRE( !subscriptionData2Deleted );
        REQUIRE( !subscriptionData3Deleted );
        
        REQUIRE( gotParam1 == (Array<String>{"hello", "world"}) );
        REQUIRE( gotParam2 == (Array<String>{"hello", "world"}) );
        // for 3 the order should be called with the new value "world" twice, since both calls
        // happen after the value was changed.
        REQUIRE( gotParam3 == (Array<String>{"world", "world"}) );
        
    }
    
    SECTION("unsubscribe from inside notify")
    {
        Array<String> gotParam1;
        Array<String> gotParam2;
        Array<String> gotParam3;
        
        bool subscriptionData1Deleted=false;
        P<PropertyNotifierTestSubscriptionData> pTestSubscriptionData1 = newObj<PropertyNotifierTestSubscriptionData>(&subscriptionData1Deleted);
        
        bool subscriptionData2Deleted=false;
        P<PropertyNotifierTestSubscriptionData> pTestSubscriptionData2 = newObj<PropertyNotifierTestSubscriptionData>(&subscriptionData2Deleted);
        
        bool subscriptionData3Deleted=false;
        P<PropertyNotifierTestSubscriptionData> pTestSubscriptionData3 = newObj<PropertyNotifierTestSubscriptionData>(&subscriptionData3Deleted);
        
        SECTION("unsub following")
        {
            P<INotifierSubControl> pSub1 = pNotifier->subscribe(
                [&gotParam1, pTestSubscriptionData1](String param)
                {
                   gotParam1.add( param );
                } );
            
            P<INotifierSubControl> pSub3;
            
            P<INotifierSubControl> pSub2 = pNotifier->subscribe(
                [&gotParam2, pTestSubscriptionData2, &pSub3](String param)
                {
                   gotParam2.add( param );
                    
                   pSub3->unsubscribe();
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
            
            
            pNotifier->notify( PropertyNotifierTestAccessor<String>(*pNotifier, "hello") );
            REQUIRE( !subscriptionData1Deleted );
            REQUIRE( !subscriptionData2Deleted );
            // 3 has been unsubscribed
            REQUIRE( subscriptionData3Deleted );
            
            pNotifier->notify( PropertyNotifierTestAccessor<String>(*pNotifier, "world") );
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
            P<INotifierSubControl> pSub1 = pNotifier->subscribe(
                [&gotParam1, pTestSubscriptionData1](String param)
                {
                   gotParam1.add( param );
                } );
            
            P<INotifierSubControl> pSub2;
            
            pSub2 = pNotifier->subscribe(
                [&gotParam2, pTestSubscriptionData2, &pSub2](String param)
                {
                   gotParam2.add( param );
                    
                   pSub2->unsubscribe();
                } );
            
            P<INotifierSubControl> pSub3 = pNotifier->subscribe(
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
            
            
            pNotifier->notify( PropertyNotifierTestAccessor<String>(*pNotifier, "hello") );
            REQUIRE( !subscriptionData1Deleted );
            // 3 should have been unsubscribed
            REQUIRE( subscriptionData2Deleted );
            REQUIRE( !subscriptionData3Deleted );
            
            pNotifier->notify( PropertyNotifierTestAccessor<String>(*pNotifier, "world") );
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
            P<INotifierSubControl> pSub1 = pNotifier->subscribe(
                [&gotParam1, pTestSubscriptionData1](String param)
                {
                   gotParam1.add( param );
                } );
            
            P<INotifierSubControl> pSub2 = pNotifier->subscribe(
                [&gotParam2, pTestSubscriptionData2, pNotifier](String param)
                {
                   gotParam2.add( param );
                    
                   pNotifier->unsubscribeAll();
                } );
            
            P<INotifierSubControl> pSub3 = pNotifier->subscribe(
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
            
            
            pNotifier->notify(PropertyNotifierTestAccessor<String>(*pNotifier, "hello"));
            REQUIRE( subscriptionData1Deleted );
            REQUIRE( subscriptionData2Deleted );
            REQUIRE( subscriptionData3Deleted );
            
            pNotifier->notify(PropertyNotifierTestAccessor<String>(*pNotifier, "world"));
            REQUIRE( subscriptionData1Deleted );
            REQUIRE( subscriptionData2Deleted );
            REQUIRE( subscriptionData3Deleted );
            
            REQUIRE( (gotParam1 == Array<String>{"hello"}) );
            REQUIRE( (gotParam2 == Array<String>{"hello"}) );
            REQUIRE( (gotParam3 == Array<String>{}) );
        }
                    
                    
    }
}





