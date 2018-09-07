#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/TextField.h>
#include <bdn/Window.h>
#include <bdn/test/TestTextFieldCore.h>
#include "TestWebemsViewCoreMixin.h"

using namespace bdn;

class TestWebemsTextFieldCore
    : public bdn::test::TestWebemsViewCoreMixin<bdn::test::TestTextFieldCore>
{
  protected:
    void verifyCoreText() override
    {
        String expectedText = _pTextField->text();

        emscripten::val contentObj = _domObject["value"];
        REQUIRE(!contentObj.isUndefined());

        String text = contentObj.as<std::string>();
        REQUIRE(text == expectedText);
    }
};

TEST_CASE("webems.TextFieldCore")
{
    P<TestWebemsTextFieldCore> pTest = newObj<TestWebemsTextFieldCore>();
    pTest->runTests();
}