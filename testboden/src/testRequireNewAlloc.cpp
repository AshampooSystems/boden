#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/RequireNewAlloc.h>

using namespace bdn;

class TestDummy : public RequireNewAlloc<Base, TestDummy>
{
public:
	TestDummy()
	{		
	}

	TestDummy(double x)
	{		
	}
};

TEST_CASE("RequireNewAlloc")
{
	SECTION("NoParams")
	{
		SECTION("nonNew")
			REQUIRE_THROWS_AS( TestDummy test;, ProgrammingError );

		SECTION("new")
			newObj<TestDummy>();
	}

	SECTION("withParams")
	{
		SECTION("nonNew")
			REQUIRE_THROWS_AS( TestDummy test(2.3);, ProgrammingError );

		SECTION("new")
			newObj<TestDummy>(2.3);
	}

	SECTION("errorMessageContainsCorrectClassName")
	{
		String errorMessage;
		try
		{
			TestDummy test;
		}
		catch(ProgrammingError& e)
		{
			errorMessage = e.what();			
		}

		REQUIRE( errorMessage.contains("TestDummy") );
	}


}


