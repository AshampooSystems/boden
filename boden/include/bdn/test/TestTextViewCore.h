#ifndef BDN_TEST_TestTextViewCore_H_
#define BDN_TEST_TestTextViewCore_H_

#include <bdn/TextView.h>
#include <bdn/test/TestViewCore.h>
#include <bdn/TextView.h>

namespace bdn
{
namespace test
{
    
/** Helper for tests that verify ITextViewCore implementations.*/
class TestTextViewCore : public TestViewCore
{

protected:

    P<View> createView() override
    {
        return newObj<TextView>();
    }

    void setView(View* pView) override
    {
        TestViewCore::setView(pView);

        _pTextView = cast<TextView>( pView );
    }

    void runInitTests() override
    {
        TestViewCore::runInitTests();

        SECTION("text")
        {
            _pTextView->text() = "helloworld";
            initCore();
            verifyCoreText();
        }
    }

    void runPostInitTests() override
    {
        TestViewCore::runPostInitTests();

        SECTION("text")
        {
            SECTION("value")
            {
                _pTextView->text() = "helloworld";
                verifyCoreText();
            }

            SECTION("effectsOnPreferredSize")
            {
                Size prefSizeBefore = _pTextView->calcPreferredSize();

                _pTextView->text() = "helloworld";

                Size prefSize = _pTextView->calcPreferredSize();

                // width must increase with a bigger text
                REQUIRE( prefSize.width > prefSizeBefore.width );

                // note that the height might or might not increase. But it cannot be smaller.
                REQUIRE( prefSize.height >= prefSizeBefore.height );

                // when we go back to the same text as before then the preferred size should
                // also be the same again
                _pTextView->text() = "";

                REQUIRE( _pTextView->calcPreferredSize() == prefSizeBefore );
            }
        }
    }

    /** Verifies that the text view core's text has the expected value
        (the text set in the outer TextView object's TextView::text() property.*/
    virtual void verifyCoreText()=0;


    P<TextView> _pTextView;
};


}
}

#endif

