#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/func.h>

using namespace bdn;



struct MethodPTestStruct
{
    int         i;
    std::string s;
};


class MethodPTestHelperBase : public Base
{
public:

    virtual int ii(int a)
    {
        _lastCalled = "base int(int)";
        
        return a;
    }
    
    String _lastCalled;
    
};

class IMethodPTestHelper : BDN_IMPLEMENTS IBase
{
public:
    
    virtual int ii(int a)=0;
    
};

class MethodPTestHelper : public MethodPTestHelperBase, BDN_IMPLEMENTS IMethodPTestHelper
{
public:

    int i()
    {
        _lastCalled = "int()";
        
        return 42;
    }
    
    int ii(int a) override
    {
        _lastCalled = "int(int)";
        
        return a*a;
    }
    
    void vi(int a)
    {
        _lastCalled = "void(int)";
    }
    
    MethodPTestStruct sss(const MethodPTestStruct& a, const MethodPTestStruct& b)
    {
        _lastCalled = "struct(struct,struct)";
        
        return MethodPTestStruct{a.i+b.i, a.s+b.s};
    }
};




template<class MethodType>
static void verifyMethodValid(const MethodType& m, bool expectedValid )
{
    REQUIRE( static_cast<bool>(m) == expectedValid );
    REQUIRE( (m==nullptr) == !expectedValid );
    REQUIRE( (m!=nullptr) == expectedValid );
}

TEST_CASE("strongMethod")
{
    MethodPTestHelper helper;
    
    REQUIRE( helper.getRefCount() == 1);
    
    
    SECTION("int()")
    {
        {
            auto m = strongMethod(&helper, &MethodPTestHelper::i);
            
            REQUIRE( helper.getRefCount() == 2);
            verifyMethodValid(m, true);

            int val = m();
            REQUIRE( val==42 );

            REQUIRE( helper._lastCalled=="int()" );
        }
        
        REQUIRE( helper.getRefCount()==1 );
    }
    
    SECTION("int(int)")
    {
        {
            auto m = strongMethod(&helper, &MethodPTestHelper::ii);
            
            REQUIRE( helper.getRefCount() == 2);
            verifyMethodValid(m, true);
            
            int val = m(3);
            REQUIRE( val==9 );
            
            REQUIRE( helper._lastCalled=="int(int)" );
        }
        
        REQUIRE( helper.getRefCount()==1 );
    }
    
    SECTION("int(int) virtual base")
    {
        {
            auto m = strongMethod((MethodPTestHelperBase*)&helper, &MethodPTestHelperBase::ii);
            
            REQUIRE( helper.getRefCount() == 2);
            verifyMethodValid(m, true);
            
            int val = m(3);
            REQUIRE( val==9 );
            
            REQUIRE( helper._lastCalled=="int(int)" );
        }
        
        REQUIRE( helper.getRefCount()==1 );
    }
    
    SECTION("int(int) virtual interface")
    {
        {
            auto m = strongMethod((IMethodPTestHelper*)&helper, &IMethodPTestHelper::ii);
            
            REQUIRE( helper.getRefCount() == 2);
            verifyMethodValid(m, true);
            
            int val = m(3);
            REQUIRE( val==9 );
            
            REQUIRE( helper._lastCalled=="int(int)" );
        }
        
        REQUIRE( helper.getRefCount()==1 );
    }
       
    
    SECTION("void(int)")
    {
        {
            auto m = strongMethod(&helper, &MethodPTestHelper::vi);
            
            REQUIRE( helper.getRefCount() == 2);
            verifyMethodValid(m, true);
            
            m(3);
            
            REQUIRE( helper._lastCalled=="void(int)" );
        }
        
        REQUIRE( helper.getRefCount()==1 );
    }
    
    SECTION("struct(struct, struct)")
    {
        {
            auto m = strongMethod(&helper, &MethodPTestHelper::sss);
            
            REQUIRE( helper.getRefCount() == 2);
            verifyMethodValid(m, true);
            
            MethodPTestStruct a{12, "ab"};
            MethodPTestStruct b{34, "cd"};
            
            MethodPTestStruct r = m(a, b);
            
            REQUIRE( r.i==12+34 );
            REQUIRE( r.s=="abcd" );
            
            
            REQUIRE( helper._lastCalled=="struct(struct,struct)" );
        }
        
        REQUIRE( helper.getRefCount()==1 );
    }
    
    SECTION("struct(struct, struct) from P")
    {
        {
            P<MethodPTestHelper> pHelper = &helper;
        
            auto m = strongMethod(pHelper, &MethodPTestHelper::sss);
            
            REQUIRE( helper.getRefCount() == 3);
            verifyMethodValid(m, true);
            
            MethodPTestStruct a{12, "ab"};
            MethodPTestStruct b{34, "cd"};
            
            MethodPTestStruct r = m(a, b);
            
            REQUIRE( r.i==12+34 );
            REQUIRE( r.s=="abcd" );
            
            
            REQUIRE( helper._lastCalled=="struct(struct,struct)" );
        }
        
        REQUIRE( helper.getRefCount()==1 );
    }

    SECTION("defaultInit")
    {
        StrongMethod_<MethodPTestHelper, void (MethodPTestHelper::*)(int)> m;
        
        verifyMethodValid(m, false);
        
        // calling the strongMethod should have no effect
        REQUIRE_THROWS_AS( m(1), std::bad_function_call );
    }
    
    SECTION("object=null")
    {
        auto m = strongMethod((MethodPTestHelper*)nullptr, &MethodPTestHelper::ii);
        
        verifyMethodValid(m, false);
        
        REQUIRE_THROWS_AS( m(1), std::bad_function_call );
    }
    
    SECTION("method=null")
    {
        void (MethodPTestHelper::* nullFunc)(int);
        
        nullFunc = nullptr;
    
        auto m = strongMethod(&helper, nullFunc);
        
        verifyMethodValid(m, false);
        
        REQUIRE_THROWS_AS( m(1), std::bad_function_call );
    }
    
    SECTION("strongMethod and object null")
    {
        void (MethodPTestHelper::* nullFunc)(int);
        nullFunc = nullptr;
        
        auto m = strongMethod((MethodPTestHelper*)nullptr, nullFunc);
        
        verifyMethodValid(m, false);
        
        REQUIRE_THROWS_AS( m(1), std::bad_function_call );
    }


    SECTION("copyConstructor")
    {
        SECTION("defaultInit")
        {
            StrongMethod_<MethodPTestHelper, void (MethodPTestHelper::*)(int)> m;
            
            StrongMethod_<MethodPTestHelper, void (MethodPTestHelper::*)(int)> m2(m);
            
            verifyMethodValid(m2, false);
            
            // calling the strongMethod should have no effect
            REQUIRE_THROWS_AS( m2(1), std::bad_function_call );
        }
        
        SECTION("ok")
        {
            {
                auto m = strongMethod(&helper, &MethodPTestHelper::ii);
                REQUIRE( helper.getRefCount() == 2);
                verifyMethodValid(m, true);
                
                std::function<int(int)> m2(m);
                REQUIRE( helper.getRefCount() == 3);
                verifyMethodValid(m2, true);
                
                int val = m(3);
                REQUIRE( val==9 );
                
                REQUIRE( helper._lastCalled=="int(int)" );
                helper._lastCalled = "";
                
                val = m2(3);
                REQUIRE( val==9 );
                
                REQUIRE( helper._lastCalled=="int(int)" );
            }
            
            REQUIRE( helper.getRefCount()==1 );
        }
    }


    SECTION("assignment")
    {
        SECTION("defaultInit")
        {
            StrongMethod_<MethodPTestHelper, void (MethodPTestHelper::*)(int)> m;
            
            StrongMethod_<MethodPTestHelper, void (MethodPTestHelper::*)(int)> m2;
            
            m2 = m;
            
            verifyMethodValid(m2, false);
            
            // calling the strongMethod should have no effect
            REQUIRE_THROWS_AS( m2(1), std::bad_function_call );
        }
        
        SECTION("ok")
        {
            {
                auto m = strongMethod(&helper, &MethodPTestHelper::ii);
                REQUIRE( helper.getRefCount() == 2);
                verifyMethodValid(m, true);
                
                std::function<int(int)> m2;
                
                m2 = m;
                
                REQUIRE( helper.getRefCount() == 3);
                verifyMethodValid(m2, true);
                
                int val = m(3);
                REQUIRE( val==9 );
                
                REQUIRE( helper._lastCalled=="int(int)" );
                helper._lastCalled = "";
                
                val = m2(3);
                REQUIRE( val==9 );
                
                REQUIRE( helper._lastCalled=="int(int)" );
            }
            
            REQUIRE( helper.getRefCount()==1 );
        }
    }


}




TEST_CASE("weakMethod")
{
    
    MethodPTestHelper helper;
    
    REQUIRE( helper.getRefCount() == 1);
    
    
    SECTION("int()")
    {
        {
            auto m = weakMethod(&helper, &MethodPTestHelper::i);
            
            // refcount should not have increased
            REQUIRE( helper.getRefCount() == 1);
            verifyMethodValid(m, true);
            
            int val = m();
            REQUIRE( val==42 );
            
            REQUIRE( helper._lastCalled=="int()" );
        }
        
        REQUIRE( helper.getRefCount()==1 );
    }
    
    SECTION("int(int)")
    {
        {
            auto m = weakMethod(&helper, &MethodPTestHelper::ii);
            
            // refcount should not have increased
            REQUIRE( helper.getRefCount() == 1);
            verifyMethodValid(m, true);
            
            int val = m(3);
            REQUIRE( val==9 );
            
            REQUIRE( helper._lastCalled=="int(int)" );
        }
        
        REQUIRE( helper.getRefCount()==1 );
    }
    
    
    SECTION("int(int) virtual base")
    {
        {
            auto m = weakMethod((MethodPTestHelperBase*)&helper, &MethodPTestHelperBase::ii);
            
            REQUIRE( helper.getRefCount() == 1);
            verifyMethodValid(m, true);
            
            int val = m(3);
            REQUIRE( val==9 );
            
            REQUIRE( helper._lastCalled=="int(int)" );
        }
        
        REQUIRE( helper.getRefCount()==1 );
    }
    
    
    
    SECTION("void(int)")
    {
        {
            auto m = weakMethod(&helper, &MethodPTestHelper::vi);
            
            REQUIRE( helper.getRefCount() == 1);
            verifyMethodValid(m, true);
            
            m(3);
            
            REQUIRE( helper._lastCalled=="void(int)" );
        }
        
        REQUIRE( helper.getRefCount()==1 );
    }
    
    SECTION("struct(struct, struct)")
    {
        {
            auto m = weakMethod(&helper, &MethodPTestHelper::sss);
            
            REQUIRE( helper.getRefCount() == 1);
            verifyMethodValid(m, true);
            
            MethodPTestStruct a{12, "ab"};
            MethodPTestStruct b{34, "cd"};
            
            MethodPTestStruct r = m(a, b);
            
            REQUIRE( r.i==12+34 );
            REQUIRE( r.s=="abcd" );
            
            
            REQUIRE( helper._lastCalled=="struct(struct,struct)" );
        }
        
        REQUIRE( helper.getRefCount()==1 );
    }
    
    SECTION("struct(struct, struct) from P")
    {
        {
            P<MethodPTestHelper> pHelper = &helper;
            
            REQUIRE( helper.getRefCount() == 2);
            
            auto m = weakMethod(pHelper, &MethodPTestHelper::sss);
            
            REQUIRE( helper.getRefCount() == 2);
            verifyMethodValid(m, true);
            
            MethodPTestStruct a{12, "ab"};
            MethodPTestStruct b{34, "cd"};
            
            MethodPTestStruct r = m(a, b);
            
            REQUIRE( r.i==12+34 );
            REQUIRE( r.s=="abcd" );
            
            
            REQUIRE( helper._lastCalled=="struct(struct,struct)" );
        }
        
        REQUIRE( helper.getRefCount()==1 );
    }
    
    SECTION("defaultInit")
    {
        WeakMethod_<MethodPTestHelper, void (MethodPTestHelper::*)(int)> m;
        
        verifyMethodValid(m, false);
        
        // calling the strongMethod should have no effect
        REQUIRE_THROWS_AS( m(1), std::bad_function_call );
    }
    
    SECTION("object=null")
    {
        auto m = weakMethod((MethodPTestHelper*)nullptr, &MethodPTestHelper::ii);
        
        verifyMethodValid(m, false);
        
        REQUIRE_THROWS_AS( m(1), std::bad_function_call );
    }
    
    SECTION("method=null")
    {
        void (MethodPTestHelper::* nullFunc)(int);
        
        nullFunc = nullptr;
        
        auto m = weakMethod(&helper, nullFunc);
        
        verifyMethodValid(m, false);
        
        REQUIRE_THROWS_AS( m(1), std::bad_function_call );
    }
    
    SECTION("strongMethod and object null")
    {
        void (MethodPTestHelper::* nullFunc)(int);
        nullFunc = nullptr;
        
        auto m = weakMethod((MethodPTestHelper*)nullptr, nullFunc);
        
        verifyMethodValid(m, false);
        
        REQUIRE_THROWS_AS( m(1), std::bad_function_call );
    }
    
    
    SECTION("copyConstructor")
    {
        SECTION("defaultInit")
        {
            WeakMethod_<MethodPTestHelper, void (MethodPTestHelper::*)(int)> m;
            
            WeakMethod_<MethodPTestHelper, void (MethodPTestHelper::*)(int)> m2(m);
            
            verifyMethodValid(m2, false);
            
            // calling the strongMethod should have no effect
            REQUIRE_THROWS_AS( m2(1), std::bad_function_call );
        }
        
        SECTION("ok")
        {
            {
                auto m = weakMethod(&helper, &MethodPTestHelper::ii);
                REQUIRE( helper.getRefCount() == 1);
                verifyMethodValid(m, true);
                
                std::function<int(int)> m2(m);
                REQUIRE( helper.getRefCount() == 1);
                verifyMethodValid(m2, true);
                
                int val = m(3);
                REQUIRE( val==9 );
                
                REQUIRE( helper._lastCalled=="int(int)" );
                helper._lastCalled = "";
                
                val = m2(3);
                REQUIRE( val==9 );
                
                REQUIRE( helper._lastCalled=="int(int)" );
            }
            
            REQUIRE( helper.getRefCount()==1 );
        }
    }
    
    
    SECTION("assignment")
    {
        SECTION("defaultInit")
        {
            WeakMethod_<MethodPTestHelper, void (MethodPTestHelper::*)(int)> m;
            
            WeakMethod_<MethodPTestHelper, void (MethodPTestHelper::*)(int)> m2;
            
            m2 = m;
            
            verifyMethodValid(m2, false);
            
            // calling the strongMethod should have no effect
            REQUIRE_THROWS_AS( m2(1), std::bad_function_call );
        }
        
        SECTION("ok")
        {
            {
                auto m = weakMethod(&helper, &MethodPTestHelper::ii);
                REQUIRE( helper.getRefCount() == 1);
                verifyMethodValid(m, true);
                
                std::function<int(int)> m2;
                
                m2 = m;
                
                REQUIRE( helper.getRefCount() == 1);
                verifyMethodValid(m2, true);
                
                int val = m(3);
                REQUIRE( val==9 );
                
                REQUIRE( helper._lastCalled=="int(int)" );
                helper._lastCalled = "";
                
                val = m2(3);
                REQUIRE( val==9 );
                
                REQUIRE( helper._lastCalled=="int(int)" );
            }
            
            REQUIRE( helper.getRefCount()==1 );
        }
    }

    SECTION("objectDestroyed")
    {
        std::function<int()> m;

        {
            P<MethodPTestHelper> pHelper = newObj<MethodPTestHelper>();
    
            REQUIRE( pHelper->getRefCount() == 1);    
    
            m = weakMethod(pHelper, &MethodPTestHelper::i);
            
            // refcount should not have increased
            REQUIRE( pHelper->getRefCount() == 1);    
            verifyMethodValid(m, true);
            
            int val = m();
            REQUIRE( val==42 );
            
            REQUIRE( pHelper->_lastCalled=="int()" );
        }

        // should still be valid, since the function object does still have
        // a backing callable object.
        verifyMethodValid(m, true);

        REQUIRE_THROWS_AS( m(), DanglingFunctionError );
    }
    
    
}







