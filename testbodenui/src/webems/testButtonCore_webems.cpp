#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Button.h>
#include <bdn/Window.h>
#include <bdn/test/TestButtonCore.h>
#include "TestWebViewCoreMixin.h"

using namespace bdn;


class TestWebButtonCore : public bdn::test::TestWebViewCoreMixin< bdn::test::TestButtonCore >
{
protected:

    void verifyCoreLabel() override
    {
        String expectedLabel = _pButton->label();

        emscripten::val contentObj = _domObject["textContent"];
        REQUIRE( !contentObj.isUndefined() );

        String label = contentObj.as<std::string>();
        
        REQUIRE( label == expectedLabel );
    }
};

TEST_CASE("webems.ButtonCore")
{
    P<TestWebButtonCore> pTest = newObj<TestWebButtonCore>();

    pTest->runTests();
}









