
#include <bdn/test.h>

#include <bdn/errno.h>
#include <bdn/ErrorInfo.h>
#include <bdn/config.h>

#include <system_error>

using namespace bdn;

void verifyErrnoCodeToSystemError(int errnoCode)
{
    SystemError err = errnoCodeToSystemError(errnoCode, ErrorFields().add("bla", "blub").add("gubbel", "hurz"));

    if (config::is_family_posix) {
        REQUIRE(err.code().category() == std::system_category());
    } else {
        REQUIRE(err.code().category() == std::generic_category());
    }

    REQUIRE(err.code().value() == errnoCode);

    String combinedMessage = err.what();

    ErrorInfo info(err);

    REQUIRE(info.getField("bla") == "blub");
    REQUIRE(info.getField("gubbel") == "hurz");

    String message = info.getMessage();
    REQUIRE(message.size() >= 5); // should not be empty

    // the error condition should be set properly
    std::error_condition cond = err.code().default_error_condition();

    if (config::is_family_posix) {
        REQUIRE((cond.category() == std::system_category() || cond.category() == std::generic_category()));
    } else {
        REQUIRE(cond.category() == std::generic_category());
    }

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
