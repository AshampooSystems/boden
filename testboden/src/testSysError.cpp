#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/sysError.h>
#include <bdn/ErrorInfo.h>

#include <system_error>

#if BDN_PLATFORM_WINDOWS
#include <windows.h>
#endif

using namespace bdn;

void verifyErrorCodeMapping(int errorCode, const std::list<int>& expectedPosixCodeList)
{
	const std::error_category& cat = std::system_category();

	std::error_condition cond = cat.default_error_condition(errorCode);

	if(expectedPosixCodeList.empty())
	{
		REQUIRE( cond.category()==std::system_category() );
		REQUIRE( cond.value()==errorCode );
	}
	else
	{
		REQUIRE( cond.category()==std::generic_category() );
	
		int posixValue = cond.value();
		REQUIRE_IN( posixValue, expectedPosixCodeList );
	}
}

struct ExpectedMapping
{
	ExpectedMapping(const String& sectionName, int errorCode, std::list<int> posixCodeList)
	{
		this->sectionName = sectionName;
		this->errorCode = errorCode;
		this->posixCodeList = posixCodeList;
	}

	String			sectionName;
	int				errorCode;
	std::list<int>	posixCodeList;
};

#define EXPECTED_MAPPING(errorCode, ...) \
ExpectedMapping( #errorCode " in " #__VA_ARGS__, errorCode, __VA_ARGS__ )




void testErrorCodeMapping()
{
#if BDN_PLATFORM_WINDOWS
	std::list<ExpectedMapping> expectedMappings(
	{
		EXPECTED_MAPPING(ERROR_FILE_NOT_FOUND, {ENOENT} ),
		EXPECTED_MAPPING(ERROR_PATH_NOT_FOUND, {ENOENT, ENOTDIR} ),
		EXPECTED_MAPPING(ERROR_DEV_NOT_EXIST, {ENOENT, ENODEV} ),

		EXPECTED_MAPPING(ERROR_FILE_EXISTS, {EEXIST} ),
		EXPECTED_MAPPING(ERROR_ALREADY_EXISTS, {EEXIST} ),

		EXPECTED_MAPPING(ERROR_DIR_NOT_EMPTY, {ENOTEMPTY} ),

		EXPECTED_MAPPING(ERROR_ACCESS_DENIED, {EACCES} ),		
		EXPECTED_MAPPING(ERROR_SHARING_VIOLATION, {EACCES} ),

		EXPECTED_MAPPING(ERROR_DISK_FULL, {ENOSPC} ),

		EXPECTED_MAPPING(ERROR_USER_MAPPED_FILE, {} ),
		
	} );

#else

	std::map<int, std::list<int> > expectedMap =
	{
		{ENOENT, {ENOENT} ),
		{ENOTDIR, {ENOTDIR} ),

		{EEXIST, {EEXIST} ),

		{ENOTEMPTY, {ENOTEMPTY} ),

		{EACCES, {EACCES} ),

		{ENOSPC, {ENOSPC} ),
	};

#endif

	for(auto expectedMapping: expectedMappings)
	{
		SECTION(expectedMapping.sectionName)
			verifyErrorCodeMapping(expectedMapping.errorCode, expectedMapping.posixCodeList);
	}
}



int getNotFoundErrorCode()
{
#if BDN_PLATFORM_WINDOWS
	return ERROR_FILE_NOT_FOUND;
#else
	return ENOENT;
#endif
}


void verifyMakeThrowSysError(const SystemError& err)
{
	REQUIRE( err.code().value()==getNotFoundErrorCode() );
	REQUIRE( err.code().category()==std::system_category() );

	String combinedMessage = err.what();

	ErrorInfo info(err);
		
	REQUIRE( info.getField("bla") == "blub");
	REQUIRE( info.getField("gubbel") == "hurz");

	String message = info.getMessage();
	REQUIRE( message.getLength()>=5 );	// should not be empty
				
}

void testMakeThrowSysError()
{
	int errorCode = getNotFoundErrorCode();

	SECTION("makeSysError")
	{
		SystemError err = makeSysError(errorCode, ErrorFields().add("bla", "blub")
																.add("gubbel", "hurz") );

		verifyMakeThrowSysError(err);
	}


	SECTION("throwSysError")
	{
		try
		{
			throwSysError(errorCode, ErrorFields().add("bla", "blub")
													.add("gubbel", "hurz") );

			REQUIRE(false);
		}
		catch(SystemError& e)
		{
			verifyMakeThrowSysError(e);
		}
	}	
}



TEST_CASE("sysError")
{
	SECTION("codeMapping")
		testErrorCodeMapping();
	
	SECTION("makeThrowSysError")
		testMakeThrowSysError();
}

