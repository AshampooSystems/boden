#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Toggle.h>
#include <bdn/Window.h>
#include <bdn/test/TestToggleCore.h>
#include <bdn/webems/CheckboxCore.h>
#include "TestWebemsViewCoreMixin.h"

using namespace bdn;

class TestWebemsToggleCore
    : public bdn::test::TestWebemsViewCoreMixin<bdn::test::TestToggleCore>
{
  protected:
    void verifyCoreLabel() override
    {
        String expectedLabel = _pToggle->label();

        emscripten::val labelDomObject =
            cast<webems::CheckboxCore<Toggle>>(_pWebCore)->_getLabelDomObject();
        emscripten::val contentObj = labelDomObject["textContent"];
        REQUIRE(!labelDomObject.isUndefined());

        String label = contentObj.as<std::string>();

        REQUIRE(label == expectedLabel);
    }

    void verifyCoreOn() override
    {
        bool expectedOn = _pToggle->on();
        bool on = cast<webems::CheckboxCore<Toggle>>(_pWebCore)
                      ->_getCheckboxDomObject()["checked"]
                      .as<bool>();
        REQUIRE(on == expectedOn);
    }
};

TEST_CASE("webems.ToggleCore")
{
    P<TestWebemsToggleCore> pTest = newObj<TestWebemsToggleCore>();

    pTest->runTests();
}
