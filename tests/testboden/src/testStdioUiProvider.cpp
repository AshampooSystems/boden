#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/StdioUiProvider.h>
#include <bdn/Button.h>

using namespace bdn;

template <class CharType> static void testStdioUiProvider()
{
    std::basic_istringstream<CharType> inStream;
    std::basic_ostringstream<CharType> outStream;
    std::basic_ostringstream<CharType> errStream;

    P<StdioUiProvider<CharType>> pProvider = newObj<StdioUiProvider<CharType>>(&inStream, &outStream, &errStream);

    SECTION("getName")
    REQUIRE(pProvider->getName() == "stdio");

    SECTION("createViewCore")
    {
        // we expect an exception, since the implementation does not support any
        // View types.
        P<Button> pButton = newObj<Button>();

        REQUIRE_THROWS_AS(pProvider->createViewCore(Button::getButtonCoreTypeName(), pButton),
                          ViewCoreTypeNotSupportedError);
    }

    SECTION("getTextUi")
    {
        P<ITextUi> pUi = pProvider->getTextUi();

        // the StdioTextUi is tested separately. So we only verify here that
        // we get a valid object
        REQUIRE(pUi != nullptr);
    }
}

TEST_CASE("StdioUiProvider")
{
    SECTION("char")
    testStdioUiProvider<char>();

    SECTION("wchar_t")
    testStdioUiProvider<wchar_t>();
}
