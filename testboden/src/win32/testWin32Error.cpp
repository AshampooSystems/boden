#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/win32/win32Error.h>
#include <bdn/ErrorInfo.h>

#include <system_error>

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

	for(auto expectedMapping: expectedMappings)
	{
		SECTION(expectedMapping.sectionName)
			verifyErrorCodeMapping(expectedMapping.errorCode, expectedMapping.posixCodeList);
	}
}


void verifyNotFoundSystemSysError(const SystemError& err)
{
    REQUIRE( err.code().category()==std::system_category() );
	REQUIRE( err.code().value()==ERROR_FILE_NOT_FOUND );	

	String combinedMessage = err.what();

	ErrorInfo info(err);

	REQUIRE( info.getField("bla") == "blub");
	REQUIRE( info.getField("gubbel") == "hurz");

	String message = info.getMessage();
	REQUIRE( message.getLength()>=5 );	// should not be empty

    // the error condition should be set properly
    std::error_condition cond = err.code().default_error_condition();    
	REQUIRE( cond.category()==std::generic_category() );

	REQUIRE( cond.value() == ENOENT );
}

void testWin32ErrorCodeToSystemError()
{
	SystemError err = bdn::win32::win32ErrorCodeToSystemError(ERROR_FILE_NOT_FOUND, ErrorFields().add("bla", "blub")
															.add("gubbel", "hurz") );

	verifyNotFoundSystemSysError(err);
}

bool setAccessError()
{
	SetLastError(ERROR_ACCESS_DENIED);
	return true;
}

void testThrowLastError()
{
	SECTION("normal")
    {
        // set the last error to "not found"
        ::SetLastError( ERROR_FILE_NOT_FOUND );
        
        try
	    {
		    BDN_WIN32_throwLastError( ErrorFields().add("bla", "blub")
													.add("gubbel", "hurz") );

		    REQUIRE(false);
	    }
	    catch(SystemError& e)
	    {
		    verifyNotFoundSystemSysError(e);
	    }
    }

    SECTION("parameterCodeModifiesSysError")
    {
        // set the last error to "not found"
        ::SetLastError( ERROR_FILE_NOT_FOUND );

	    try
	    {
		    // then call BDN_throwLastSysError with a parameter object that will modify
		    // the last sys error

		    BDN_WIN32_throwLastError( ErrorFields()
			    // construct the ErrorFields object in a way so that it modifies the last error
			    .add(setAccessError() ? "bla" : "blax", "blub")
			    .add("gubbel", "hurz") );

		    REQUIRE(false);
	    }
	    catch(SystemError& e)
	    {
		    // sanity check. Last error at this point should NOT be "not found"
		    REQUIRE( ::GetLastError() != ERROR_FILE_NOT_FOUND );

 		    // but the exception object should still have the not found error.
		    verifyNotFoundSystemSysError(e);
	    }
    }
}

TEST_CASE("win32Error")
{
	SECTION("codeMapping")
		testErrorCodeMapping();

	SECTION("win32ErrorCodeToSystemError")
		testWin32ErrorCodeToSystemError();

	SECTION("throwLastError")
		testThrowLastError();
}

