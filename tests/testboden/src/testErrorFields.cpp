#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/ErrorFields.h>

using namespace bdn;


void verifyParse(const String& inString, const std::map<String,String>& expectedFields)
{
	ErrorFields parsedFields(inString);

	for(auto item: expectedFields)
	{
		REQUIRE( parsedFields.contains(item.first) );
		REQUIRE( parsedFields[item.first]==item.second );
	}

	REQUIRE( parsedFields.size()==expectedFields.size() );
}

void verifyToStringParse(std::map<String,String> inFields, const String& expectedString)
{
	ErrorFields fields;

	for(auto item: inFields)
		fields.add(item.first, item.second);

	String s = fields.toString();

	REQUIRE(s==expectedString);

	verifyParse(s, inFields);
}



TEST_CASE("ErrorFields")
{
	SECTION("empty")
		verifyToStringParse( std::map<String,String>(), "");

	SECTION("simpleItem")
		verifyToStringParse( { {"hello", "world"} }, "[[hello: \"world\"]]");

	SECTION("emptyStrings")
		verifyToStringParse( { {"", ""} }, "[[: \"\"]]");

	SECTION("withSpecialChars")
		verifyToStringParse( { {U"hell\U00012345o", U"worl\U00012345d"},
								{"he%llo:\"[[b]la,]]", "wo%r:\"[[b]l,]]d"},
								{"", ""},
								{"a", "b"} },
							U"[[: \"\", a: \"b\", he%25llo%3a\"[[b]la,%5d%5d: \"wo%25r:%22[[b]l,%5d%5dd\", hell\U00012345o: \"worl\U00012345d\"]]");


	SECTION("parseInvalidData")
	{
		SECTION("startBracketsMissing")
			verifyParse("hello: \"world\"]]", std::map<String,String>() );

		SECTION("endBracketsMissing")
			verifyParse("[[hello: \"world\"", std::map<String,String>() );

		SECTION("noColon")
			verifyParse("[[hello \"world\"]]", std::map<String,String>() );

		SECTION("noStartQuotationMark")
			verifyParse("[[hello: world\"]]", std::map<String,String>() );

		SECTION("noEndQuotationMark")
			verifyParse("[[hello: \"world]]", std::map<String,String>() );
	}
}

