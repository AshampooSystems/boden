#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Switch.h>
#include <bdn/Window.h>
#include <bdn/test/TestSwitchCore.h>
#include <bdn/webems/SwitchCore.h>
#include "TestWebemsViewCoreMixin.h"

using namespace bdn;


class TestWebemsSwitchCore : public bdn::test::TestWebemsViewCoreMixin< bdn::test::TestSwitchCore >
{
protected:

    void verifyCoreLabel() override
    {
        String expectedLabel = _pSwitch->label();

        emscripten::val labelDomObject = cast<webems::SwitchCore<Switch>>(_pWebCore)->_getLabelDomObject();
        emscripten::val contentObj = labelDomObject["textContent"];
        REQUIRE( !labelDomObject.isUndefined() );

        String label = contentObj.as<std::string>();
        
        REQUIRE( label == expectedLabel );
    }

    void verifyCoreOn() override
    {
        bool expectedOn = _pSwitch->on();
        bool on = cast<webems::SwitchCore<Switch>>(_pWebCore)->_getSwitchDomObject().call<bool>("getAttribute", emscripten::val("data-on"));
        REQUIRE( on == expectedOn );
    }

};

TEST_CASE("webems.SwitchCore")
{
    P<TestWebemsSwitchCore> pTest = newObj<TestWebemsSwitchCore>();

    pTest->runTests();
}









