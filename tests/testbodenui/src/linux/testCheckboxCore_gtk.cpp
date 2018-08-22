#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Checkbox.h>
#include <bdn/gtk/CheckboxCore.h>
#include <bdn/Window.h>
#include <bdn/test/TestCheckboxCore.h>
#include "TestGtkViewCoreMixin.h"

using namespace bdn;


class TestGtkCheckboxCore : public bdn::test::TestGtkViewCoreMixin< bdn::test::TestCheckboxCore >
{
protected:

    void verifyCoreLabel() override
    {
        String expectedLabel = _pCheckbox->label();
        
        String label = gtk_button_get_label( GTK_BUTTON(_pGtkWidget) );
        
        REQUIRE( label == expectedLabel );
    }

    void verifyCoreState() override
    {
    	TriState expectedState = _pCheckbox->state();
    	TriState state = cast<bdn::gtk::CheckboxCore<Checkbox>>(_pCore)->_gtkToggleButtonStateToCheckboxState();
    	REQUIRE( state == expectedState );
    }

};

TEST_CASE("gtk.CheckboxCore")
{
    P<TestGtkCheckboxCore> pTest = newObj<TestGtkCheckboxCore>();

    pTest->runTests();
}




