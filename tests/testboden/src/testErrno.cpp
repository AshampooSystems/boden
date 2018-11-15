#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/errno.h>
#include <bdn/ErrorInfo.h>

#include <system_error>

using namespace bdn;

void verifyErrnoCodeToSystemError(int errnoCode)
{
    SystemError err = errnoCodeToSystemError(errnoCode, ErrorFields().add("bla", "blub").add("gubbel", "hurz"));

#if BDN_PLATFORM_FAMILY_POSIX
    REQUIRE(err.code().category() == std::system_category());
#else
    REQUIRE(err.code().category() == std::generic_category());
#endif

    REQUIRE(err.code().value() == errnoCode);

    String combinedMessage = err.what();

    ErrorInfo info(err);

    REQUIRE(info.getField("bla") == "blub");
    REQUIRE(info.getField("gubbel") == "hurz");

    String message = info.getMessage();
    REQUIRE(message.getLength() >= 5); // should not be empty

    // the error condition should be set properly
    std::error_condition cond = err.code().default_error_condition();

#if BDN_PLATFORM_FAMILY_POSIX
    REQUIRE((cond.category() == std::system_category() || cond.category() == std::generic_category()));
#else
    REQUIRE(cond.category() == std::generic_category());
#endif

    REQUIRE(cond.value() == errnoCode);
}

TEST_CASE("errno")
{
    SECTION("errnoToSystemError")
    {
        SECTION("ERANGE")
        verifyErrnoCodeToSystemError(ERANGE);
        SECTION("EDOM")
        verifyErrnoCodeToSystemError(EDOM);
        SECTION("ENOENT")
        verifyErrnoCodeToSystemError(ENOENT);
    }
}
