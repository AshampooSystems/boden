#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/win32/util.h>

#include <system_error>

using namespace bdn;
using namespace bdn::win32;

static bool operator==(const POINT& a, const POINT& b)
{
    return (a.x == b.x
            && a.y == b.y);
}

static bool operator==(const SIZE& a, const SIZE& b)
{
    return (a.cx == b.cx
            && a.cy == b.cy);
}

static bool operator==(const RECT& a, const RECT& b)
{
    return (a.left == b.left
        && a.top == b.top
        && a.right == b.right
        && a.bottom == b.bottom);
}

TEST_CASE("win32.util")
{
    SECTION("pointToWin32Point")
    {
        SECTION("rounding")
        {
            REQUIRE( pointToWin32Point( Point(1.49, 2.49), 1 ) == (POINT{1,2}) );
            REQUIRE( pointToWin32Point( Point(1.5, 2.5), 1 ) == (POINT{2,3}) );    
        }

        SECTION("scaling")
        {
            SECTION(">1")
                REQUIRE( pointToWin32Point( Point(2, 3), 2.5 ) == (POINT{5, 8}) );

            SECTION("<1")
                REQUIRE( pointToWin32Point( Point(20, 30), 0.75 ) == (POINT{15, 23}) );
        }
    }


    SECTION("sizeToWin32Size")
    {
        SECTION("rounding")
        {
            REQUIRE( sizeToWin32Size( Size(1.49, 2.49), 1 ) == (SIZE{1,2}) );
            REQUIRE( sizeToWin32Size( Size(1.5, 2.5), 1 ) == (SIZE{2,3}) );    
        }

        SECTION("scaling")
        {
            SECTION(">1")
                REQUIRE( sizeToWin32Size( Size(2, 3), 2.5 ) == (SIZE{5, 8}) );

            SECTION("<1")
                REQUIRE( sizeToWin32Size( Size(20, 30), 0.75 ) == (SIZE{15, 23}) );
        }
    }
    
    SECTION("rectToWin32Rect")
    {
        SECTION("rounding")
        {
            REQUIRE( rectToWin32Rect( Rect(1.49, 2.49, 3.49, 4.49), 1 ) == (RECT{1,2,1+3,2+4}) );
            REQUIRE( rectToWin32Rect( Rect(1.5, 2.5, 3.5, 4.5), 1 ) == (RECT{2,3,2+4,3+5}) );    
        }

        SECTION("scaling")
        {
            SECTION(">1")
                REQUIRE( rectToWin32Rect( Rect(2, 3, 4, 5), 2.5 ) == (RECT{5, 8, 5+10, 8+13}) );

            SECTION("<1")
                REQUIRE( rectToWin32Rect( Rect(20, 30, 40, 50), 0.75 ) == (RECT{15, 23, 15+30, 23+38}) );
        }
    }
    
    SECTION("win32RectToRect")
    {
        SECTION("scaling")
        {
            SECTION(">1")
                REQUIRE( win32RectToRect( RECT{2, 3, 4, 5}, 2.5 ) == Rect(2/2.5, 3/2.5, 4/2.5, 5/2.5) );

            SECTION("<1")
                REQUIRE( win32RectToRect( RECT{2, 3, 4, 5}, 0.75 ) == Rect(2/0.75, 3/0.75, 4/0.75, 5/0.75) );
        }
    }

    SECTION("win32PointToPoint")
    {
        SECTION("scaling")
        {
            SECTION(">1")
                REQUIRE( win32PointToPoint( POINT{2, 3}, 2.5 ) == Point(2/2.5, 3/2.5) );

            SECTION("<1")
                REQUIRE( win32PointToPoint( POINT{2, 3}, 0.75 ) == Point(2/0.75, 3/0.75) );
        }
    }

    SECTION("win32SizeToSize")
    {
        SECTION("scaling")
        {
            SECTION(">1")
                REQUIRE( win32SizeToSize( SIZE{2, 3}, 2.5 ) == Size(2/2.5, 3/2.5) );

            SECTION("<1")
                REQUIRE( win32SizeToSize( SIZE{2, 3}, 0.75 ) == Size(2/0.75, 3/0.75) );
        }
    }

}

