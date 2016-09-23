#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Margin.h>

using namespace bdn;

TEST_CASE("Margin")
{
	SECTION("defaultConstruct")
	{
		Margin a;

		REQUIRE( a.top == 0);
		REQUIRE( a.right == 0);
		REQUIRE( a.bottom == 0);
		REQUIRE( a.left == 0);
	}

	SECTION("constructAllIndividually")
	{
		Margin a(1,2,3,4);

		REQUIRE( a.top == 1);
		REQUIRE( a.right == 2);
		REQUIRE( a.bottom == 3);
		REQUIRE( a.left == 4);
	}

	SECTION("constructSingleVal")
	{
		Margin a(3);

		REQUIRE( a.top == 3);
		REQUIRE( a.right == 3);
		REQUIRE( a.bottom == 3);
		REQUIRE( a.left == 3);
	}

	SECTION("constructTwoVals")
	{
		Margin a(3, 4);

		REQUIRE( a.top == 3);
		REQUIRE( a.right == 4);
		REQUIRE( a.bottom == 3);
		REQUIRE( a.left == 4);
	}

	SECTION("equality")
	{
		Margin a(1,2,3,4);
		
		checkEquality( Margin(), Margin(), true );
		checkEquality( a, Margin(), false );
		checkEquality( a, Margin(1,2,3,4), true );
		checkEquality( a, Margin(10,2,3,4), false );
		checkEquality( a, Margin(1,20,3,4), false );
		checkEquality( a, Margin(1,2,30,4), false );
		checkEquality( a, Margin(1,2,3,40), false );
	}
    
    SECTION("+-Margin")
    {
        Margin a(1,2,3,4);
        
        SECTION("+")
        {
            REQUIRE( a+Margin(11,22,33,44) == Margin(12,24,36,48) );
            REQUIRE( a == Margin(1,2,3,4) );
        }
        
        SECTION("-")
        {
            REQUIRE( a-Margin(11,22,33,44) == Margin(-10,-20,-30,-40) );
            REQUIRE( a == Margin(1,2,3,4) );
        }
        
        SECTION("+neg")
        {
            REQUIRE( a+Margin(-11,-22,-33,-44) == Margin(-10,-20,-30,-40) );
            REQUIRE( a == Margin(1,2,3,4) );
        }
        
        SECTION("-neg")
        {
            REQUIRE( a-Margin(-11,-22,-33,-44) == Margin(12,24,36,48) );
            REQUIRE( a == Margin(1,2,3,4) );
        }
        
        SECTION("+=")
        {
            a+=Margin(11,22,33,44);
            REQUIRE( a == Margin(12,24,36,48) );
        }
        
        SECTION("-=")
        {
            a-=Margin(11,22,33,44);
            REQUIRE( a == Margin(-10,-20,-30,-40) );
        }
        
        SECTION("+=neg")
        {
            a+=Margin(-11,-22,-33,-44);
            REQUIRE( a == Margin(-10,-20,-30,-40) );
        }
        
        SECTION("-=neg")
        {
            a-=Margin(-11,-22,-33,-44);
            REQUIRE( a == Margin(12,24,36,48) );
        }
    }

    
    
    SECTION("comparison operators")
    {
        Margin a(10, 20, 30, 40);

        for(int component=0; component<4; component++)
        {
            Margin b(a);

            REQUIRE( ! (a<b) );
            REQUIRE( ! (a>b) );
            REQUIRE( (a<=b) );
            REQUIRE( (a>=b) );

            double* pComp;
            if(component==0)
                pComp = &b.top;
            else if(component==1)
                pComp = &b.right;
            else if(component==2)
                pComp = &b.bottom;
            else
                pComp = &b.left;

            (*pComp) ++;

            REQUIRE( ! (a<b) );
            REQUIRE( ! (a>b) );
            REQUIRE( (a<=b) );
            REQUIRE( ! (a>=b) );

            (*pComp) -= 2;

            REQUIRE( ! (a<b) );
            REQUIRE( ! (a>b) );
            REQUIRE( ! (a<=b) );
            REQUIRE( (a>=b) );
        }

        {
            Margin b(a);

            b -= Margin(1);

            REQUIRE( ! (a<b) );
            REQUIRE( (a>b) );
            REQUIRE( ! (a<=b) );
            REQUIRE( (a>=b) );
        }

        {
            Margin b(a);

            b += Margin(1);

            REQUIRE( (a<b) );
            REQUIRE( ! (a>b) );
            REQUIRE( (a<=b) );
            REQUIRE( ! (a>=b) );
        }


    }

}


