#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Thread.h>

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

#if BDN_HAVE_THREADS

TEST_CASE("test.failsInOtherThreads", "[!shouldfail]")
{
	SECTION("exceptionPropagatedToMainThread")
	{
		std::future<void> result = Thread::exec( [](){ REQUIRE(false); } );

		result.get();
	}

	SECTION("exceptionNotPropagatedToMainThread")
	{
		std::future<void> result = Thread::exec( [](){ REQUIRE(false); } );
		result.wait();
	}
}

#endif

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


TEST_CASE("test.shouldFailWithFailOnTopLevel", "[!shouldfail]")
{
    REQUIRE(false);
}

TEST_CASE("test.shouldFailWithFailInSection", "[!shouldfail]")
{
	SECTION("failSection")
	{
        REQUIRE(false);
	}
}

TEST_CASE("test.uncaughtExceptionBugWorkaround", "[test]")
{
    // std::uncaught_exception is used by the test system to determine
    // if a section aborted with an exception.
    // However, some C++ standard library implementations have a buggy
    // implementation that can result in the flag to remain set under certain
    // conditions, even if there is not actually an uncaught exception.
    // We test here that this bug either does not happen, or that after the bug
    // occurs the test either aborts or the flag is reset by some method.

    SECTION("throwCatch")
    {
        // the bug we know of should NOT trigger here
        try
        {
            throw InvalidArgumentError("hello");
        }
        catch(...)
        {
        }

        REQUIRE( !std::uncaught_exception() );
    }

    SECTION("requireThrows")
    {
        // the bug we know of should NOT trigger here
        REQUIRE_THROWS( throw InvalidArgumentError("hello") );

        REQUIRE( !std::uncaught_exception() );
    }

    SECTION("storeReThrow")
    {
        // this is where the bug MIGHT trigger
        std::exception_ptr p;

        try
        {
            throw InvalidArgumentError("hello");
        }
        catch(...)
        {
            p = std::current_exception();
        }

        REQUIRE( !std::uncaught_exception() );

        try
        {
            std::rethrow_exception(p);
        }
        catch(...)
        {
        }

        if( std::uncaught_exception() )
        {
            // yep, we have the bug. This is not a failure yet - but
            // we check in the next section that the flag is not set anymore
            // (IF the test continues).
            int x=0;
            x++;
        }
    }

    SECTION("afterStoreRethrow")
    {
        // even if the bug was triggered above, the flag should now be unset again!
        REQUIRE( !std::uncaught_exception() );
    }
}



