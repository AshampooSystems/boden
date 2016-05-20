#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/cast.h>


using namespace bdn;

class A : public Base
{
	
};

class SubA : public A
{
	
};

class SubSubA : public SubA
{
	
};

class B
{
	
};



void verifyConst(void* p, bool constExpected)
{
	REQUIRE(!constExpected);
}

void verifyConst(const void* p, bool constExpected)
{
	REQUIRE(constExpected);
}


template<class InPtr>
void testTryCast(InPtr pSubA, bool constExpected)
{
	REQUIRE( tryCast<SubA>(pSubA)==pSubA );
	REQUIRE( tryCast<A>(pSubA)==pSubA );
	REQUIRE( tryCast<B>(pSubA)==nullptr );		
	REQUIRE( tryCast<SubSubA>(pSubA)==nullptr );
		
	verifyConst( tryCast<SubA>(pSubA), constExpected );
	verifyConst( tryCast<A>(pSubA), constExpected );
	verifyConst( tryCast<B>(pSubA), constExpected );
	verifyConst( tryCast<SubSubA>(pSubA), constExpected );
}


TEST_CASE("tryCast")
{
	SECTION("null")
	{
		A* pA = nullptr;

		REQUIRE( tryCast<A>(pA)==nullptr );
		REQUIRE( tryCast<B>(pA)==nullptr );
		REQUIRE( tryCast<SubA>(pA)==nullptr );
	}

	SECTION("non-null")
	{
		SubA subA;

		SECTION("non-const")
			testTryCast<SubA*>(&subA, false);		
		
		SECTION("const")
			testTryCast<const SubA*>(&subA, true);

		SECTION("P")
		{
			P<SubA> pSubA = &subA;

			testTryCast< P<SubA> >(pSubA, false);		
		}

		SECTION("PC")
		{
			P<const SubA> pSubA = &subA;

			testTryCast< P<const SubA> >(pSubA, true);		
		}		

	}	
}



template<class InPtr>
void testCast(InPtr pSubA, bool constExpected)
{
	
	REQUIRE( cast<SubA>(pSubA)==pSubA );
	REQUIRE( cast<A>(pSubA)==pSubA );
	REQUIRE_THROWS_AS( cast<B>(pSubA), CastError );		
	REQUIRE_THROWS_AS( cast<SubSubA>(pSubA), CastError );

	verifyConst( cast<SubA>(pSubA), constExpected);
	verifyConst( cast<A>(pSubA), constExpected );
}

TEST_CASE("cast")
{
	SECTION("null")
	{
		A* pA = nullptr;

		REQUIRE( cast<A>(pA)==nullptr );
		REQUIRE( cast<B>(pA)==nullptr );
		REQUIRE( cast<SubA>(pA)==nullptr );
	}

	SECTION("non-null")
	{
		SubA subA;

		SECTION("non-const")
			testCast<SubA*>(&subA, false);		
		
		SECTION("const")
			testCast<const SubA*>(&subA, true);

		SECTION("P")
		{
			P<SubA> pSubA = &subA;

			testCast< P<SubA> >(pSubA, false);		
		}

		SECTION("PC")
		{
			P<const SubA> pSubA = &subA;

			testCast< P<const SubA> >(pSubA, true);		
		}		
	}	
}

