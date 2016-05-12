#include <bdn/init.h>
#include <bdn/test.h>

#include <thread>
#include <future>

using namespace bdn;


TEST_CASE("test.checkEquality")
{
	class Comparer
	{
	public:
		Comparer(int val)
		{
			_val = val;
		}

		bool invertEqual=false;

		bool operator==(const Comparer& o) const
		{
			bool result = (_val==o._val);
			if(invertEqual)
				return !result;
			else
				return result;
		}

		bool invertNotEqual=false;

		bool operator!=(const Comparer& o) const
		{
			bool result = (_val!=o._val);
			if(invertNotEqual)
				return !result;
			else
				return result;
		}
		
		int _val;
	};

	Comparer a(0);
	Comparer b(0);
	Comparer c(1);

	REQUIRE( checkEquality(a, b, true) );
	REQUIRE( checkEquality(b, a, true) );
	REQUIRE( !checkEquality(a, b, false) );
	REQUIRE( !checkEquality(b, a, false) );

	REQUIRE( checkEquality(a, c, false) );
	REQUIRE( checkEquality(c, a, false) );
	REQUIRE( !checkEquality(a, c, true) );
	REQUIRE( !checkEquality(c, a, true) );
		
	// results are inconsistent => should always get false
	a.invertEqual = true;
	REQUIRE( !checkEquality(a, b, true) );
	REQUIRE( !checkEquality(b, a, true) );
	REQUIRE( !checkEquality(a, b, false) );
	REQUIRE( !checkEquality(b, a, false) );

	REQUIRE( !checkEquality(a, c, false) );
	REQUIRE( !checkEquality(c, a, false) );
	REQUIRE( !checkEquality(a, c, true) );
	REQUIRE( !checkEquality(c, a, true) );
	a.invertEqual = false;

	a.invertNotEqual = true;
	REQUIRE( !checkEquality(a, b, true) );
	REQUIRE( !checkEquality(b, a, true) );
	REQUIRE( !checkEquality(a, b, false) );
	REQUIRE( !checkEquality(b, a, false) );

	REQUIRE( !checkEquality(a, c, false) );
	REQUIRE( !checkEquality(c, a, false) );
	REQUIRE( !checkEquality(a, c, true) );
	REQUIRE( !checkEquality(c, a, true) );
	a.invertNotEqual = false;
	
}


TEST_CASE("test.inNotIn")
{
	std::list<int> container( {1, 10, 20});

	REQUIRE_IN(1, container );
	REQUIRE_IN(10, container );	
	REQUIRE_IN(20, container );

	REQUIRE_NOT_IN(2, container );
	REQUIRE_NOT_IN(11, container );
	REQUIRE_NOT_IN(21, container );
}


TEST_CASE("test.failsInOtherThreads", "[!shouldfail]")
{
	SECTION("exceptionPropagatedToMainThread")
	{
		std::future<void> result = std::async( std::launch::async, [](){ REQUIRE(false); } );

		result.get();
	}

	SECTION("exceptionNotPropagatedToMainThread")
	{
		std::future<void> result = std::async( std::launch::async, [](){ REQUIRE(false); } );
		result.wait();
	}
}


TEST_CASE("test.nestedSectionsNoExtraCalls", "[test]")
{
	SECTION("a")
	{
		bool		aEnteredSubSection = false;
		static int	aCount=0;
		aCount++;
		REQUIRE(aCount<=4);

		SECTION("aa")
		{
			aEnteredSubSection = true;

			bool		aaEnteredSubSection = false;
			static int	aaCount=0;
			aaCount++;
			REQUIRE(aaCount<=2);

			SECTION("aaa")
			{
				static int	aaaCount=0;
				aaaCount++;
				REQUIRE(aaaCount==1);

				aaEnteredSubSection = true;
			}

			SECTION("aab")
			{			
				static int	aabCount=0;
				aabCount++;
				REQUIRE(aabCount==1);

				aaEnteredSubSection = true;
			}

			REQUIRE( aaEnteredSubSection );
		}

		SECTION("ab")
		{
			aEnteredSubSection = true;

			bool		abEnteredSubSection = false;
			static int	abCount=0;
			abCount++;
			REQUIRE(abCount<=2);

			SECTION("aba")
			{
				static int	abaCount=0;
				abaCount++;
				REQUIRE(abaCount==1);

				abEnteredSubSection = true;
			}

			SECTION("abb")
			{			
				static int	abbCount=0;
				abbCount++;
				REQUIRE(abbCount==1);

				abEnteredSubSection = true;
			}

			REQUIRE( abEnteredSubSection );
		}

		REQUIRE( aEnteredSubSection );
	}
	
}



void subTest(bool subSections)
{
	bool		aEnteredSubSection = false;
	
	SECTION("aa")
	{
		aEnteredSubSection = true;

		if(subSections)
		{
			bool		aaEnteredSubSection = false;
			
			SECTION("aaa")
			{
				aaEnteredSubSection = true;
			}

			SECTION("aab")
			{			
				aaEnteredSubSection = true;
			}

			REQUIRE( aaEnteredSubSection );
		}
	}

	SECTION("ab")
	{
		aEnteredSubSection = true;

		if(subSections)
		{
			bool		abEnteredSubSection = false;

			SECTION("aba")
				abEnteredSubSection = true;

			SECTION("abb")
				abEnteredSubSection = true;

			REQUIRE( abEnteredSubSection );
		}
	}

	REQUIRE( aEnteredSubSection );
	
}

TEST_CASE("test.conditionalNestedSectionsNoExtraCalls", "[test]")
{
	SECTION("a")
	{
		subTest(false);
	}

	SECTION("b")
	{
		subTest(true);
	}	
	
}

TEST_CASE("test.conditionalNestedSectionsNoExtraCalls.withoutThenWithSubsections", "[test]")
{
	SECTION("a")
	{
		subTest(false);
	}

	SECTION("b")
	{
		subTest(true);
	}		
}

TEST_CASE("test.conditionalNestedSectionsNoExtraCalls.withThenWithoutSubSections", "[test]")
{
	SECTION("a")
	{
		subTest(true);
	}

	SECTION("b")
	{
		subTest(false);
	}	
	
}