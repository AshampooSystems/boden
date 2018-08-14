#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Button.h>
#include <bdn/Window.h>
#include <bdn/test/TestButtonCore.h>
#include "TestGtkViewCoreMixin.h"

using namespace bdn;


class TestGtkButtonCore : public bdn::test::TestGtkViewCoreMixin< bdn::test::TestButtonCore >
{
protected:

    void verifyCoreLabel() override
    {
        String expectedLabel = _pButton->label();
        
        String label = gtk_button_get_label( GTK_BUTTON(_pGtkWidget) );
        
        REQUIRE( label == expectedLabel );
    }
};

TEST_CASE("gtk.ButtonCore")
{
    P<TestGtkButtonCore> pTest = newObj<TestGtkButtonCore>();

    pTest->runTests();
}




