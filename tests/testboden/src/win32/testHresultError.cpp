#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/win32/hresultError.h>
#include <bdn/ErrorInfo.h>

#include <system_error>

using namespace bdn;

void verifyToFromException(HRESULT result, int expectedCode)
{
    SystemError err = bdn::win32::hresultToSystemError(result, ErrorFields().add("bla", "blub"));

    REQUIRE(err.code().category() == std::system_category());
    REQUIRE(err.code().value() == expectedCode);

    ErrorInfo info(err);

    REQUIRE(info.getField("bla") == "blub");

    HRESULT outResult = bdn::win32::exceptionToHresult(err);
    REQUIRE(outResult == result);
}

TEST_CASE("hresultError")
{
    SECTION("toFromException")
    {
        SECTION("0x80070005")
        verifyToFromException(0x80070005, ERROR_ACCESS_DENIED);

        SECTION("E_FAIL")
        verifyToFromException(E_FAIL, E_FAIL);

        SECTION("S_OK")
        verifyToFromException(S_OK, ERROR_SUCCESS);
    }

    SECTION("fromNonSystemException")
    {
        SECTION("std::exception")
        {
            HRESULT outResult = bdn::win32::exceptionToHresult(std::exception());
            REQUIRE(outResult == E_FAIL);
        }
    }
}