#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/ErrorFields.h>

using namespace bdn;


void verifyFields(const std::map<String,String>& inFields, const String& expectedString)
{
	ErrorFields fields;

	for(auto item: inFields)
		fields.add(item.first, item.second);

	String s = fields.toString();

	REQUIRE(s==expectedString);

	ErrorFields parsedFields(s);


	for(auto item: inFields)
	{
		REQUIRE( parsedFields.contains(item.first) );
		REQUIRE( parsedFields[item.first]==item.second );
	}

	REQUIRE( parsedFields.size()==inFields.size() );
}

TEST_CASE("ErrorFields")
{
	SECTION("empty")
		verifyFields( {}, "");	

	SECTION("simpleItem")
		verifyFields( { {"hello", "world"} }, "[[hello: \"world\"]]");	

	SECTION("emptyStrings")
		verifyFields( { {"", ""} }, "[[: \"\"]]");	

	SECTION("withSpecialChars")
		verifyFields( { {U"hell\U00012345o", U"worl\U00012345d"},
						{"he%llo:\"[[b]la,]]", "wo%r:\"[[b]l,]]d"},
						{"", ""},
						{"a", "b"} },
					 U"[[: \"\", a: \"b\", hell\U00012345o: \"worl\U00012345d\", he%25llo%3a\"[[b]la,%5d%5d: \"wo%25r:%22[[b]l,%5d%5dd\"]]");	
}

