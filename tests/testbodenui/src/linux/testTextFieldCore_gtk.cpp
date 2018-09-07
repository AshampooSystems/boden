#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/TextField.h>
#include <bdn/Window.h>
#include <bdn/test/TestTextFieldCore.h>
#include "TestGtkViewCoreMixin.h"

using namespace bdn;

class TestGtkTextFieldCore
    : public bdn::test::TestGtkViewCoreMixin<bdn::test::TestTextFieldCore>
{
  protected:
    void verifyCoreText() override
    {
        String expectedText = _pTextField->text();
        String text = gtk_entry_get_text(GTK_ENTRY(_pGtkWidget));
        REQUIRE(text == expectedText);
    }
};

TEST_CASE("gtk.TextFieldCore")
{
    P<TestGtkTextFieldCore> pTest = newObj<TestGtkTextFieldCore>();
    pTest->runTests();
}
