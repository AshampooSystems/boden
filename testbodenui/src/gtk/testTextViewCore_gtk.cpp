#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/TextView.h>
#include <bdn/Window.h>
#include <bdn/test/TestTextViewCore.h>
#include "TestGtkViewCoreMixin.h"

using namespace bdn;


class TestGtkTextViewCore : public bdn::test::TestGtkViewCoreMixin< bdn::test::TestTextViewCore >
{
protected:

    void verifyCoreText() override
    {
        String expectedText = _pTextView->text();
        
        String text = gtk_label_get_text( GTK_LABEL(_pGtkWidget) );
        
        REQUIRE( text == expectedText );
    }
    
    bool usesAllAvailableWidthWhenWrapped() const override
    {
        // unfortunately we use all available space when wrapping occurs.
        // This has to do with how GTK calculates restricted height and widths.
        // See gtk::ViewCore::calcPreferredSize to find out why this is.
        return true;
    }
    
    
    bool clipsPreferredWidthToAvailableWidth() const override
    {
        // we cannot return a preferred size bigger than the available size.
        // See gtk::ViewCore::calcPreferredSize to find out why this is.
        return true;
    }
};

TEST_CASE("gtk.TextViewCore")
{
    P<TestGtkTextViewCore> pTest = newObj<TestGtkTextViewCore>();

    pTest->runTests();
}








