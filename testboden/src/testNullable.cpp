#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Nullable.h>

using namespace bdn;

template<typename ValueType>
static void verifyNull( const Nullable<ValueType>& n)
{
    REQUIRE( n.isNull() );
    
    REQUIRE_THROWS_PROGRAMMING_ERROR( n.get() );
    REQUIRE_THROWS_PROGRAMMING_ERROR( ValueType v; v = n; );
}


template<typename ValueType>
static void verifyNotNull( const Nullable<ValueType>& n, ValueType expectedValue)
{
    REQUIRE( !n.isNull() );
    
    REQUIRE( !n.isNull() );
    REQUIRE( n.get() == expectedValue );
    REQUIRE( ((ValueType)n) == expectedValue );
    
    ValueType value = n;
    REQUIRE( value == expectedValue );
}

template<typename ValueType>
static void testNullable(ValueType exampleValue, ValueType exampleValue2)
{
    SECTION("construct")
    {
        SECTION("default")
        {
            Nullable<ValueType> n;
            
            verifyNull(n);
        }
        
        SECTION("nullptr")
        {
            Nullable<ValueType> n(nullptr);
            
            verifyNull(n);
        }
        
        SECTION("value")
        {
            Nullable<ValueType> n(exampleValue);
            
            verifyNotNull(n, exampleValue);
        }
        
        
        SECTION("movable")
        {
            ValueType v = exampleValue;
            Nullable<ValueType> n( std::move(v) );
            
            verifyNotNull(n, exampleValue);
        }
        
        
        SECTION("Nullable")
        {
            Nullable<ValueType> n1(exampleValue);
            Nullable<ValueType> n2( n1 );
            
            verifyNotNull(n2, exampleValue);
        }

    }
    
    SECTION("assign")
    {
        SECTION("toNull")
        {
            Nullable<ValueType> n;
            
            SECTION("nullptr")
            {
                n = nullptr;
                
                verifyNull(n);
            }
            
            SECTION("value")
            {
                n = exampleValue;
                
                verifyNotNull(n, exampleValue);
            }
            
            SECTION("Nullable-null")
            {
                Nullable<ValueType> n2;
                
                n = n2;
                verifyNull(n);
            }
            
            SECTION("Nullable-notNull")
            {
                Nullable<ValueType> n2(exampleValue);
                
                n = n2;
                verifyNotNull(n, exampleValue);
            }
        }
        
        SECTION("toNotNull")
        {
            Nullable<ValueType> n(exampleValue);
            
            SECTION("nullptr")
            {
                n = nullptr;
                
                verifyNull(n);
            }
            
            SECTION("value")
            {
                n = exampleValue2;
                
                verifyNotNull(n, exampleValue2);
            }
            
            SECTION("Nullable-null")
            {
                Nullable<ValueType> n2;
                
                n = n2;
                verifyNull(n);
            }
            
            SECTION("Nullable-notNull")
            {
                Nullable<ValueType> n2(exampleValue2);
                
                n = n2;
                verifyNotNull(n, exampleValue2);
            }
        }
    }
    
}

TEST_CASE("Nullable")
{
    SECTION("int")
        testNullable<int>(17, 42);
    
    
    SECTION("struct")
    {
        struct SomeStruct
        {
            int x;
            int y;
            
            bool operator==(const SomeStruct& o) const
            {
                return x==o.x && y==o.y;
            }
            
            bool operator!=(const SomeStruct& o) const
            {
                return !operator==(o);
            }
        };
        
        SomeStruct a{42, 67};
        SomeStruct b{11, 202};
        
       
        testNullable<SomeStruct>(a, b);
    }
}



