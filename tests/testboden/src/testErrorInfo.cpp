#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/ErrorInfo.h>

using namespace bdn;


void verifyInfo(const ErrorInfo& info, const String& expectedMessage, const String& expectedFieldsString)
{
	REQUIRE( info.getMessage() == expectedMessage);
	REQUIRE( info.getFields().toString() == expectedFieldsString );
}

void verifyConstruct(const String& inputMessage, const String& expectedMessage, const String& expectedFieldsString)
{
	SECTION("fromString")
	{
		ErrorInfo info(inputMessage);
		verifyInfo(info, expectedMessage, expectedFieldsString );
	}

	SECTION("fromException")
	{
		ProgrammingError err(inputMessage);
		ErrorInfo info(err);
		verifyInfo(info, expectedMessage, expectedFieldsString );
	}
}

void testConstruct()
{
	SECTION("empty")
		verifyConstruct("", "", "");

	SECTION("justNormalMessage")
		verifyConstruct("hello world", "hello world", "");

	SECTION("fieldsAtEnd")
		verifyConstruct("hello world [[bla: \"blub\"]]", "hello world", "[[bla: \"blub\"]]");

	SECTION("fieldsAtStart")
		verifyConstruct("[[bla: \"blub\"]] hello world", "hello world", "[[bla: \"blub\"]]");

	SECTION("fieldsAtStartWithColon")
		verifyConstruct("[[bla: \"blub\"]]: hello world", "hello world", "[[bla: \"blub\"]]");

	SECTION("fieldsAtStartWithSpaceAndColon")
		verifyConstruct("[[bla: \"blub\"]] : hello world", "hello world", "[[bla: \"blub\"]]");

	SECTION("fieldsInMiddleAtStart")
		verifyConstruct("hello [[bla: \"blub\"]] world", "hello world", "[[bla: \"blub\"]]");
}

void testFieldAccess()
{
	ErrorInfo info("hello world [[bla: \"blub\", holla: \"hurz\"]]");

	REQUIRE( info.hasField("bla") );
	REQUIRE( info.hasField("holla") );
	REQUIRE( !info.hasField("blub") );
	REQUIRE( !info.hasField("hurz") );
	REQUIRE( !info.hasField("") );

	REQUIRE( info.getField("bla")=="blub" );
	REQUIRE( info.getField("holla")=="hurz" );
	REQUIRE( info.getField("blax")=="" );
	REQUIRE( info.getField("blub")=="" );
	REQUIRE( info.getField("hurz")=="" );
	REQUIRE( info.getField("")=="" );
}

TEST_CASE("ErrorInfo")
{
	SECTION("construct")
		testConstruct();
	
	SECTION("testFieldAccess")
		testFieldAccess();
}


