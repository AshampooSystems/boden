#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/MethodP.h>

using namespace bdn;



struct MethodPTestStruct
{
    int         i;
    std::string s;
};


class MethodPTestHelper : public Base
{
public:

    String _lastCalled;

    int i()
    {
        _lastCalled = "int()";
        
        return 42;
    }
    
    int ii(int a)
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
static void verifyMethodNull(const MethodType& m, bool expectedNull )
{
    REQUIRE( m.isNull() == expectedNull );
    REQUIRE( (m==nullptr) == expectedNull );
    REQUIRE( (m!=nullptr) == !expectedNull );
}

TEST_CASE("MethodP")
{

    MethodPTestHelper helper;
    
    REQUIRE( helper.getRefCount() == 1);
    
    
    SECTION("int()")
    {
        {
            auto m = makeMethodP(&helper, &MethodPTestHelper::i);
            
            REQUIRE( helper.getRefCount() == 2);
            verifyMethodNull(m, false);

            int val = m();
            REQUIRE( val==42 );

            REQUIRE( helper._lastCalled=="int()" );
        }
        
        REQUIRE( helper.getRefCount()==1 );
    }
    
    SECTION("int(int)")
    {
        {
            auto m = makeMethodP(&helper, &MethodPTestHelper::ii);
            
            REQUIRE( helper.getRefCount() == 2);
            verifyMethodNull(m, false);
            
            int val = m(3);
            REQUIRE( val==9 );
            
            REQUIRE( helper._lastCalled=="int(int)" );
        }
        
        REQUIRE( helper.getRefCount()==1 );
    }
    
    
    SECTION("void(int)")
    {
        {
            auto m = makeMethodP(&helper, &MethodPTestHelper::vi);
            
            REQUIRE( helper.getRefCount() == 2);
            verifyMethodNull(m, false);
            
            m(3);
            
            REQUIRE( helper._lastCalled=="void(int)" );
        }
        
        REQUIRE( helper.getRefCount()==1 );
    }
    
    SECTION("struct(struct, struct)")
    {
        {
            auto m = makeMethodP(&helper, &MethodPTestHelper::sss);
            
            REQUIRE( helper.getRefCount() == 2);
            verifyMethodNull(m, false);
            
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
        
            auto m = makeMethodP(pHelper, &MethodPTestHelper::sss);
            
            REQUIRE( helper.getRefCount() == 3);
            verifyMethodNull(m, false);
            
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
        MethodP<MethodPTestHelper, void (MethodPTestHelper::*)(int)> m;
        
        verifyMethodNull(m, true);
        
        // calling the method should have no effect
        REQUIRE_THROWS_AS( m(1), ProgrammingError );
    }
    
    SECTION("object=null")
    {
        auto m = makeMethodP((MethodPTestHelper*)nullptr, &MethodPTestHelper::ii);
        
        verifyMethodNull(m, true);
        
        REQUIRE_THROWS_AS( m(1), ProgrammingError );
    }
    
    SECTION("method=null")
    {
        void (MethodPTestHelper::* nullFunc)(int);
        
        nullFunc = nullptr;
    
        auto m = makeMethodP(&helper, nullFunc);
        
        verifyMethodNull(m, true);
        
        REQUIRE_THROWS_AS( m(1), ProgrammingError );
    }
    
    SECTION("method and object null")
    {
        void (MethodPTestHelper::* nullFunc)(int);
        nullFunc = nullptr;
        
        auto m = makeMethodP((MethodPTestHelper*)nullptr, nullFunc);
        
        verifyMethodNull(m, true);
        
        REQUIRE_THROWS_AS( m(1), ProgrammingError );
    }


    SECTION("copyConstructor")
    {
        SECTION("defaultInit")
        {
            MethodP<MethodPTestHelper, void (MethodPTestHelper::*)(int)> m;
            
            MethodP<MethodPTestHelper, void (MethodPTestHelper::*)(int)> m2(m);
            
            verifyMethodNull(m2, true);
            
            // calling the method should have no effect
            REQUIRE_THROWS_AS( m2(1), ProgrammingError );
        }
        
        SECTION("ok")
        {
            {
                auto m = makeMethodP(&helper, &MethodPTestHelper::ii);
                REQUIRE( helper.getRefCount() == 2);
                verifyMethodNull(m, false);
                
                MethodP<MethodPTestHelper, int (MethodPTestHelper::*)(int)> m2(m);
                REQUIRE( helper.getRefCount() == 3);
                verifyMethodNull(m2, false);
                
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
            MethodP<MethodPTestHelper, void (MethodPTestHelper::*)(int)> m;
            
            MethodP<MethodPTestHelper, void (MethodPTestHelper::*)(int)> m2;
            
            m2 = m;
            
            verifyMethodNull(m2, true);
            
            // calling the method should have no effect
            REQUIRE_THROWS_AS( m2(1), ProgrammingError );
        }
        
        SECTION("ok")
        {
            {
                auto m = makeMethodP(&helper, &MethodPTestHelper::ii);
                REQUIRE( helper.getRefCount() == 2);
                verifyMethodNull(m, false);
                
                MethodP<MethodPTestHelper, int (MethodPTestHelper::*)(int)> m2;
                
                m2 = m;
                
                REQUIRE( helper.getRefCount() == 3);
                verifyMethodNull(m2, false);
                
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


    SECTION("convert to std::function")
    {
        SECTION("constructor")
        {
            {
                std::function<int(int)> f( makeMethodP(&helper, &MethodPTestHelper::ii) );
                
                REQUIRE( helper.getRefCount() == 2);
                
                REQUIRE( helper._lastCalled=="" );
                
                int val = f(3);
                REQUIRE( val==9 );
                
                REQUIRE( helper._lastCalled=="int(int)" );
            }
            
            REQUIRE( helper.getRefCount()==1 );
        }
        
        SECTION("assignment")
        {
            {
                std::function<int(int)> f;
                
                f = makeMethodP(&helper, &MethodPTestHelper::ii);
                
                REQUIRE( helper.getRefCount() == 2);
                
                REQUIRE( helper._lastCalled=="" );
                
                int val = f(3);
                REQUIRE( val==9 );
                
                REQUIRE( helper._lastCalled=="int(int)" );
            }
            
            REQUIRE( helper.getRefCount()==1 );
        }
        
            
        
    }

}






