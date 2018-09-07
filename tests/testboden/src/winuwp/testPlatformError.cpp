#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/winuwp/platformError.h>
#include <bdn/win32/hresultError.h>

using namespace bdn;
using namespace bdn::winuwp;

void verifyErrorCodeMapping(int errorCode,
                            const std::list<int> &expectedPosixCodeList)
{
    const std::error_category &cat = std::system_category();

    std::error_condition cond = cat.default_error_condition(errorCode);

    if (expectedPosixCodeList.empty()) {
        REQUIRE(cond.category() == std::system_category());
        REQUIRE(cond.value() == errorCode);
    } else {
#if BDN_PLATFORM_LINUX
        // some implementations return system_category here. Since we are on a
        // posix system it is the same.
        REQUIRE((cond.category() == std::generic_category() ||
                 cond.category() == std::system_category()));
#else
        REQUIRE(cond.category() == std::generic_category());
#endif

        int posixValue = cond.value();
        REQUIRE_IN(posixValue, expectedPosixCodeList);
    }
}

struct ExpectedMapping
{
    ExpectedMapping(const String &sectionName, int errorCode,
                    std::list<int> posixCodeList)
    {
        this->sectionName = sectionName;
        this->errorCode = errorCode;
        this->posixCodeList = posixCodeList;
    }

    String sectionName;
    int errorCode;
    std::list<int> posixCodeList;
};

#define EXPECTED_MAPPING(errorCode, ...)                                       \
    ExpectedMapping(#errorCode " in " #__VA_ARGS__, errorCode, __VA_ARGS__)

void testErrorCodeMapping()
{

    std::list<ExpectedMapping> expectedMappings({
        EXPECTED_MAPPING(ERROR_FILE_NOT_FOUND, {ENOENT}),
        EXPECTED_MAPPING(ERROR_PATH_NOT_FOUND, {ENOENT, ENOTDIR}),
        EXPECTED_MAPPING(ERROR_DEV_NOT_EXIST, {ENOENT, ENODEV}),

        EXPECTED_MAPPING(ERROR_FILE_EXISTS, {EEXIST}),

        EXPECTED_MAPPING(ERROR_ALREADY_EXISTS, {EEXIST}),

        EXPECTED_MAPPING(ERROR_DIR_NOT_EMPTY, {ENOTEMPTY}),

        EXPECTED_MAPPING(ERROR_ACCESS_DENIED, {EACCES}),

        EXPECTED_MAPPING(ERROR_SHARING_VIOLATION, {EACCES}),

        EXPECTED_MAPPING(ERROR_DISK_FULL, {ENOSPC}),

        EXPECTED_MAPPING(ERROR_USER_MAPPED_FILE, {}),

        // even though HRESULT are the basic error code of winuwp apps, the
        // runtime lib currently does NOT map them to generic error conditions.
        // We want to know if that is ever changed, so we add tests that verify
        // the current behaviour.
        EXPECTED_MAPPING(HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND), {}),
        EXPECTED_MAPPING(HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND), {}),
        EXPECTED_MAPPING(HRESULT_FROM_WIN32(ERROR_DEV_NOT_EXIST), {}),
        EXPECTED_MAPPING(HRESULT_FROM_WIN32(ERROR_FILE_EXISTS), {}),
        EXPECTED_MAPPING(HRESULT_FROM_WIN32(ERROR_ALREADY_EXISTS), {}),
        EXPECTED_MAPPING(HRESULT_FROM_WIN32(ERROR_DIR_NOT_EMPTY), {}),
        EXPECTED_MAPPING(HRESULT_FROM_WIN32(ERROR_ACCESS_DENIED), {}),
        EXPECTED_MAPPING(HRESULT_FROM_WIN32(ERROR_SHARING_VIOLATION), {}),
        EXPECTED_MAPPING(HRESULT_FROM_WIN32(ERROR_DISK_FULL), {}),
        EXPECTED_MAPPING(HRESULT_FROM_WIN32(ERROR_USER_MAPPED_FILE), {}),

    });

    for (auto expectedMapping : expectedMappings) {
        SECTION(expectedMapping.sectionName)
        verifyErrorCodeMapping(expectedMapping.errorCode,
                               expectedMapping.posixCodeList);
    }
}

void verifyToFromPlatformException(::Platform::Exception ^ pPlatformException,
                                   int expectedCode)
{
    std::system_error e = platformExceptionToSystemError(pPlatformException);

    REQUIRE(e.code().category() == std::system_category());

    REQUIRE(e.code().value() == expectedCode);

    ::Platform::Exception ^ pOutException = exceptionToPlatformException(e);

    REQUIRE(String(pPlatformException->GetType()->FullName->Data()) ==
            String(pOutException->GetType()->FullName->Data()));

    REQUIRE(String(pOutException->Message->Data()) == String(e.what()));
}

TEST_CASE("platformError")
{
    SECTION("errorCodeMapping")
    testErrorCodeMapping();

    SECTION("toFromPlatformException")
    {
        SECTION("AccessDeniedException")
        verifyToFromPlatformException(ref new ::Platform::AccessDeniedException,
                                      ERROR_ACCESS_DENIED);

        SECTION("DisconnectedException")
        verifyToFromPlatformException(ref new ::Platform::DisconnectedException,
                                      RPC_E_DISCONNECTED);

        SECTION("FailureException")
        verifyToFromPlatformException(ref new ::Platform::FailureException,
                                      E_FAIL);

        SECTION("InvalidArgumentException")
        verifyToFromPlatformException(
            ref new ::Platform::InvalidArgumentException,
            ERROR_INVALID_PARAMETER);

        SECTION("NullReferenceException")
        verifyToFromPlatformException(
            ref new ::Platform::NullReferenceException, E_POINTER);
    }

    SECTION("nonSystemError")
    {
        ::Platform::Exception ^ pOutException =
            exceptionToPlatformException(std::exception("hello"));

        REQUIRE(String(pOutException->Message->Data()) == "hello");
    }

    // we only do the following tests in release mode because
    // throwing platform exceptions will cause a debugger break with the default
    // settings.
#ifdef NDEBUG

    SECTION("BDN_WINUWP_TO_STDEXC")
    {
        SECTION("DisconnectedException")
        {
            try {
                BDN_WINUWP_TO_STDEXC_BEGIN

                throw ref new ::Platform::DisconnectedException();

                BDN_WINUWP_TO_STDEXC_END

                REQUIRE(false);
            }
            catch (std::system_error &e) {
                REQUIRE(e.code().category() == std::system_category());
                REQUIRE(e.code().value() == RPC_E_DISCONNECTED);
            }
        }

        SECTION("AccessDeniedException")
        {
            try {
                BDN_WINUWP_TO_STDEXC_BEGIN

                throw ref new ::Platform::AccessDeniedException();

                BDN_WINUWP_TO_STDEXC_END

                REQUIRE(false);
            }
            catch (std::system_error &e) {
                REQUIRE(e.code().category() == std::system_category());
                REQUIRE(e.code().value() == ERROR_ACCESS_DENIED);
            }
        }
    }

    SECTION("BDN_WINUWP_TO_PLATFORMEXC")
    {
        SECTION("RPC_E_DISCONNECTED")
        {
            try {
                BDN_WINUWP_TO_PLATFORMEXC_BEGIN

                throw bdn::win32::hresultToSystemError(RPC_E_DISCONNECTED);

                BDN_WINUWP_TO_PLATFORMEXC_END

                REQUIRE(false);
            }
            catch (::Platform::DisconnectedException ^ pEx) {
                // ok.
                int x = 0;
                x++;
            }
        }

        SECTION("ERROR_ACCESS_DENIED")
        {
            try {
                BDN_WINUWP_TO_PLATFORMEXC_BEGIN

                throw bdn::win32::hresultToSystemError(ERROR_ACCESS_DENIED);

                BDN_WINUWP_TO_PLATFORMEXC_END

                REQUIRE(false);
            }
            catch (::Platform::AccessDeniedException ^ pEx) {
                // ok.
                int x = 0;
                x++;
            }
        }
    }
#endif
}
