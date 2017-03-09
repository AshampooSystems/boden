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



class NonBaseMethodPTestHelper
{
public:
    
    
    int ii(int a)
    {
        _lastCalled = "int(int)";
        
        return a*a;
    }
    
    String _lastCalled;
};


template<class MethodType>
static void verifyMethodValid(const MethodType& m, bool expectedValid )
{
    REQUIRE( static_cast<bool>(m) == expectedValid );
    REQUIRE( (m==nullptr) == !expectedValid );
    REQUIRE( (m!=nullptr) == expectedValid );
}

TEST_CASE("bindMethod")
{

    MethodPTestHelper helper;
    
    REQUIRE( helper.getRefCount() == 1);
    
    
    SECTION("int()")
    {
        {
            auto m = bindMethod(&helper, &MethodPTestHelper::i);
            
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
            auto m = bindMethod(&helper, &MethodPTestHelper::ii);
            
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
            auto m = bindMethod((MethodPTestHelperBase*)&helper, &MethodPTestHelperBase::ii);
            
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
            auto m = bindMethod((IMethodPTestHelper*)&helper, &IMethodPTestHelper::ii);
            
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
            auto m = bindMethod(&helper, &MethodPTestHelper::vi);
            
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
            auto m = bindMethod(&helper, &MethodPTestHelper::sss);
            
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
        
            auto m = bindMethod(pHelper, &MethodPTestHelper::sss);
            
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
        BoundMethod_<MethodPTestHelper, void (MethodPTestHelper::*)(int)> m;
        
        verifyMethodValid(m, false);
        
        // calling the bindMethod should have no effect
        REQUIRE_THROWS_AS( m(1), std::bad_function_call );
    }
    
    SECTION("object=null")
    {
        auto m = bindMethod((MethodPTestHelper*)nullptr, &MethodPTestHelper::ii);
        
        verifyMethodValid(m, false);
        
        REQUIRE_THROWS_AS( m(1), std::bad_function_call );
    }
    
    SECTION("bindMethod=null")
    {
        void (MethodPTestHelper::* nullFunc)(int);
        
        nullFunc = nullptr;
    
        auto m = bindMethod(&helper, nullFunc);
        
        verifyMethodValid(m, false);
        
        REQUIRE_THROWS_AS( m(1), std::bad_function_call );
    }
    
    SECTION("bindMethod and object null")
    {
        void (MethodPTestHelper::* nullFunc)(int);
        nullFunc = nullptr;
        
        auto m = bindMethod((MethodPTestHelper*)nullptr, nullFunc);
        
        verifyMethodValid(m, false);
        
        REQUIRE_THROWS_AS( m(1), std::bad_function_call );
    }


    SECTION("copyConstructor")
    {
        SECTION("defaultInit")
        {
            BoundMethod_<MethodPTestHelper, void (MethodPTestHelper::*)(int)> m;
            
            BoundMethod_<MethodPTestHelper, void (MethodPTestHelper::*)(int)> m2(m);
            
            verifyMethodValid(m2, false);
            
            // calling the bindMethod should have no effect
            REQUIRE_THROWS_AS( m2(1), std::bad_function_call );
        }
        
        SECTION("ok")
        {
            {
                auto m = bindMethod(&helper, &MethodPTestHelper::ii);
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
            BoundMethod_<MethodPTestHelper, void (MethodPTestHelper::*)(int)> m;
            
            BoundMethod_<MethodPTestHelper, void (MethodPTestHelper::*)(int)> m2;
            
            m2 = m;
            
            verifyMethodValid(m2, false);
            
            // calling the bindMethod should have no effect
            REQUIRE_THROWS_AS( m2(1), std::bad_function_call );
        }
        
        SECTION("ok")
        {
            {
                auto m = bindMethod(&helper, &MethodPTestHelper::ii);
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




TEST_CASE("weakBindMethod")
{
    
    MethodPTestHelper helper;
    
    REQUIRE( helper.getRefCount() == 1);
    
    
    SECTION("int()")
    {
        {
            auto m = weakBindMethod(&helper, &MethodPTestHelper::i);
            
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
            auto m = weakBindMethod(&helper, &MethodPTestHelper::ii);
            
            // refcount should not have increased
            REQUIRE( helper.getRefCount() == 1);
            verifyMethodValid(m, true);
            
            int val = m(3);
            REQUIRE( val==9 );
            
            REQUIRE( helper._lastCalled=="int(int)" );
        }
        
        REQUIRE( helper.getRefCount()==1 );
    }
    
    SECTION("int(int) non-base")
    {
        NonBaseMethodPTestHelper nonBaseHelper;
        
        {
            auto m = weakBindMethod(&nonBaseHelper, &NonBaseMethodPTestHelper::ii);
            
            verifyMethodValid(m, true);
            
            int val = m(3);
            REQUIRE( val==9 );
            
            REQUIRE( nonBaseHelper._lastCalled=="int(int)" );
        }
    }
    
    SECTION("int(int) virtual base")
    {
        {
            auto m = weakBindMethod((MethodPTestHelperBase*)&helper, &MethodPTestHelperBase::ii);
            
            REQUIRE( helper.getRefCount() == 1);
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
            auto m = weakBindMethod((IMethodPTestHelper*)&helper, &IMethodPTestHelper::ii);
            
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
            auto m = weakBindMethod(&helper, &MethodPTestHelper::vi);
            
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
            auto m = weakBindMethod(&helper, &MethodPTestHelper::sss);
            
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
            
            auto m = weakBindMethod(pHelper, &MethodPTestHelper::sss);
            
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
        WeakBoundMethod_<MethodPTestHelper, void (MethodPTestHelper::*)(int)> m;
        
        verifyMethodValid(m, false);
        
        // calling the bindMethod should have no effect
        REQUIRE_THROWS_AS( m(1), std::bad_function_call );
    }
    
    SECTION("object=null")
    {
        auto m = weakBindMethod((MethodPTestHelper*)nullptr, &MethodPTestHelper::ii);
        
        verifyMethodValid(m, false);
        
        REQUIRE_THROWS_AS( m(1), std::bad_function_call );
    }
    
    SECTION("bindMethod=null")
    {
        void (MethodPTestHelper::* nullFunc)(int);
        
        nullFunc = nullptr;
        
        auto m = weakBindMethod(&helper, nullFunc);
        
        verifyMethodValid(m, false);
        
        REQUIRE_THROWS_AS( m(1), std::bad_function_call );
    }
    
    SECTION("bindMethod and object null")
    {
        void (MethodPTestHelper::* nullFunc)(int);
        nullFunc = nullptr;
        
        auto m = weakBindMethod((MethodPTestHelper*)nullptr, nullFunc);
        
        verifyMethodValid(m, false);
        
        REQUIRE_THROWS_AS( m(1), std::bad_function_call );
    }
    
    
    SECTION("copyConstructor")
    {
        SECTION("defaultInit")
        {
            WeakBoundMethod_<MethodPTestHelper, void (MethodPTestHelper::*)(int)> m;
            
            WeakBoundMethod_<MethodPTestHelper, void (MethodPTestHelper::*)(int)> m2(m);
            
            verifyMethodValid(m2, false);
            
            // calling the bindMethod should have no effect
            REQUIRE_THROWS_AS( m2(1), std::bad_function_call );
        }
        
        SECTION("ok")
        {
            {
                auto m = weakBindMethod(&helper, &MethodPTestHelper::ii);
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
            WeakBoundMethod_<MethodPTestHelper, void (MethodPTestHelper::*)(int)> m;
            
            WeakBoundMethod_<MethodPTestHelper, void (MethodPTestHelper::*)(int)> m2;
            
            m2 = m;
            
            verifyMethodValid(m2, false);
            
            // calling the bindMethod should have no effect
            REQUIRE_THROWS_AS( m2(1), std::bad_function_call );
        }
        
        SECTION("ok")
        {
            {
                auto m = weakBindMethod(&helper, &MethodPTestHelper::ii);
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
    
    
}







