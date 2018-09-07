#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Switch.h>
#include <bdn/Window.h>
#include <bdn/test/TestSwitchCore.h>
#include "TestGtkViewCoreMixin.h"
#include <bdn/gtk/SwitchCore.h>

using namespace bdn;

class TestGtkSwitchCore
    : public bdn::test::TestGtkViewCoreMixin<bdn::test::TestSwitchCore>
{
  protected:
    void verifyCoreLabel() override
    {
        P<bdn::gtk::SwitchCore<Switch>> pSwitchCore =
            cast<bdn::gtk::SwitchCore<Switch>>(_pGtkCore);
        String expectedLabel = _pSwitch->label();
        String label =
            gtk_label_get_text(GTK_LABEL(pSwitchCore->_getLabelWidget()));
        REQUIRE(label == expectedLabel);
    }

    void verifyCoreOn() override
    {
        P<bdn::gtk::SwitchCore<Switch>> pSwitchCore =
            cast<bdn::gtk::SwitchCore<Switch>>(_pGtkCore);
        bool expectedOn = _pSwitch->on();
        bool on =
            gtk_switch_get_active(GTK_SWITCH(pSwitchCore->_getSwitchWidget()));
        REQUIRE(on == expectedOn);
    }
};

TEST_CASE("gtk.SwitchCore")
{
    P<TestGtkSwitchCore> pTest = newObj<TestGtkSwitchCore>();

    pTest->runTests();
}
