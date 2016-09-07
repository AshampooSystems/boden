#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Button.h>
#include <bdn/Window.h>
#include <bdn/test/TestTextViewCore.h>
#include "TestWebViewCoreMixin.h"

using namespace bdn;


class TestWebTextViewCore : public bdn::test::TestWebViewCoreMixin< bdn::test::TestTextViewCore >
{
protected:

    void verifyCoreText() override
    {
    	String expectedText = _pTextView->text();

        String text = _domObject["textContent"].as<std::string>();
        
        REQUIRE( text == expectedText );
    }
};

TEST_CASE("web.TextViewCore")
{
    P<TestWebTextViewCore> pTest = newObj<TestWebTextViewCore>();

    pTest->runTests();
}








