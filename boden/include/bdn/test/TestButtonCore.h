#ifndef BDN_TEST_TestButtonCore_H_
#define BDN_TEST_TestButtonCore_H_

#include <bdn/test/TestViewCore.h>
#include <bdn/Button.h>

namespace bdn
{
namespace test
{


/** Helper for tests that verify IButtonCore implementations.*/
class TestButtonCore : public TestViewCore
{

protected:

    P<View> createView() override
    {
        return newObj<Button>();
    }

    void setView(View* pView) override
    {
        TestViewCore::setView(pView);

        _pButton = cast<Button>( pView );
    }

    void runInitTests() override
    {
        TestViewCore::runInitTests();

        SECTION("label")
        {
            _pButton->label() = "helloworld";
            initCore();
            verifyCoreLabel();
        }
    }

    void runPostInitTests() override
    {
        TestViewCore::runPostInitTests();

        SECTION("label")
        {
            SECTION("value")
            {
                _pButton->label() = "helloworld";
                verifyCoreLabel();
            }

            SECTION("effectsOnPreferredSize")
            {
                Size prefSizeBefore = _pButton->calcPreferredSize();

                _pButton->label() = "helloworld";

                Size prefSize = _pButton->calcPreferredSize();

                // width must increase with a bigger label
                REQUIRE( prefSize.width > prefSizeBefore.width );

                // note that the height might or might not increase. But it cannot be smaller.
                REQUIRE( prefSize.height >= prefSizeBefore.height );

                // when we go back to the same label as before then the preferred size should
                // also be the same again
                _pButton->label() = "";

                REQUIRE( _pButton->calcPreferredSize() == prefSizeBefore );
            }
        }
    }

    /** Verifies that the button core's label has the expected value
        (the label set in the outer button object's Button::label() property.*/
    virtual void verifyCoreLabel()=0;


    P<Button> _pButton;
};


}
}

#endif

