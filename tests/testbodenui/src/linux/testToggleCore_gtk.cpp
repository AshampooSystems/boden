#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Toggle.h>
#include <bdn/Window.h>
#include <bdn/test/TestToggleCore.h>
#include "TestGtkViewCoreMixin.h"

using namespace bdn;

class TestGtkToggleCore
    : public bdn::test::TestGtkViewCoreMixin<bdn::test::TestToggleCore>
{
  protected:
    void verifyCoreLabel() override
    {
        String expectedLabel = _pToggle->label();
        String label = gtk_button_get_label(GTK_BUTTON(_pGtkWidget));
        REQUIRE(label == expectedLabel);
    }

    void verifyCoreOn() override
    {
        bool expectedOn = _pToggle->on();
        bool on = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(_pGtkWidget));
        REQUIRE(on == expectedOn);
    }
};

TEST_CASE("gtk.ToggleCore")
{
    P<TestGtkToggleCore> pTest = newObj<TestGtkToggleCore>();

    pTest->runTests();
}
