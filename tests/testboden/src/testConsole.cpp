
#include <bdn/test.h>

#if 0

#include <bdn/Console.h>

using namespace bdn;

TEST_CASE("Console")
{
	std::istringstream in;
	std::ostringstream out;
	std::ostringstream err;

	Console c(&in, &out, &err);

	SECTION("print")
	{
		SECTION("empty")
		{
			c.print("");
			REQUIRE( in.str()=="" );
		}

		SECTION("nonEmpty")
		{
			c.print("hello");
			REQUIRE( in.str()=="hello" );			
		}
		
	}


	
}

#endif // 0
