#include <bdn/init.h>

#include <bdn/IteratorRangeCombiner.h>

#include <bdn/test.h>

#include <algorithm>

using namespace bdn;


TEST_CASE("IteratorRangeCombiner", "[iterator]")
{
	std::string	s;
	std::string	s2;
	std::string s3;
	std::string expectedResult;
	IteratorRangeCombiner<std::string::iterator>	combiner;

	SECTION("none")
	{
		expectedResult = "";
	}

	SECTION("one empty")
	{
		combiner.addRange(s.begin(), s.end());

		expectedResult = "";
	}

	SECTION("two empty")
	{
		combiner.addRange(s.begin(), s.end());
		combiner.addRange(s2.begin(), s2.end());

		expectedResult = "";
	}

	SECTION("one nonempty")
	{
		s = "hello";

		combiner.addRange(s.begin(), s.end());

		expectedResult = "hello";
	}

	SECTION("two nonempty")
	{
		s = "hello";
		s2 = "world";

		combiner.addRange(s.begin(), s.end());
		combiner.addRange(s2.begin(), s2.end());

		expectedResult = "helloworld";
	}

	SECTION("empty between nonempty")
	{
		s = "hello";
		s2 = "";
		s3 = "world";

		combiner.addRange(s.begin(), s.end());
		combiner.addRange(s2.begin(), s2.end());
		combiner.addRange(s3.begin(), s3.end());

		expectedResult = "helloworld";
	}

	SECTION("empty after nonempty")
	{
		s = "hello";
		s2 = "";

		combiner.addRange(s.begin(), s.end());
		combiner.addRange(s2.begin(), s2.end());

		expectedResult = "hello";
	}

	SECTION("empty before nonempty")
	{
		s = "";
		s2 = "hello";		

		combiner.addRange(s.begin(), s.end());
		combiner.addRange(s2.begin(), s2.end());

		expectedResult = "hello";		
	}

	REQUIRE( checkEquality(combiner.begin(), combiner.end(), expectedResult.empty()) );

	std::string result( combiner.begin(), combiner.end() );
	REQUIRE( result==expectedResult );

	// also test reverse iteration
	
	std::string reverseResult;
	auto		it = combiner.end();
	
	while(it!=combiner.begin())
	{
		it--;
		reverseResult += *it;
	}

	std::string expectedReverseResult = expectedResult;
	std::reverse( expectedReverseResult.begin(), expectedReverseResult.end() );

	REQUIRE( reverseResult==expectedReverseResult );
}



