#ifndef BDN_TEST_TestButtonCore_H_
#define BDN_TEST_TestButtonCore_H_

#include <bdn/test/TestViewCore.h>
#include <bdn/Button.h>

namespace bdn
{
namespace test
{


/** Helper for tests that verify IButtonCore implementations.*/
class TestButtonCore : public TestViewCore<Button>
{

protected:

    P<View> createView() override
    {
        P<Button> pButton = newObj<Button>();
        pButton->setLabel( "hello" );

        return pButton;
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
            _pButton->setLabel( "helloworld" );
            initCore();
            verifyCoreLabel();
        }
    }

    void runPostInitTests() override
    {
        P<TestButtonCore> pThis(this);
    
        TestViewCore::runPostInitTests();

        SECTION("label")
        {
            SECTION("value")
            {
                _pButton->setLabel( "helloworld" );
                
                CONTINUE_SECTION_WHEN_IDLE(pThis)
                {
                    pThis->verifyCoreLabel();
                };
            }

            SECTION("effectsOnPreferredSize")
            {
                String labelBefore = _pButton->label();

                // the label should not be empty here
                REQUIRE(labelBefore.getLength()>3);

                Size prefSizeBefore = _pButton->calcPreferredSize();

                _pButton->setLabel( labelBefore + labelBefore + labelBefore );
                
                CONTINUE_SECTION_WHEN_IDLE(pThis, prefSizeBefore, labelBefore)
                {
                    Size prefSize = pThis->_pButton->calcPreferredSize();

                    // width must increase with a bigger label
                    REQUIRE(prefSize.width > prefSizeBefore.width);

                    // note that the height might or might not increase. But it cannot be smaller.
                    REQUIRE(prefSize.height >= prefSizeBefore.height);

                    // when we go back to the same label as before then the preferred size should
                    // also be the same again
                    pThis->_pButton->setLabel( labelBefore );
                    
                    CONTINUE_SECTION_WHEN_IDLE(pThis, labelBefore, prefSizeBefore)
                    {
                        REQUIRE(pThis->_pButton->calcPreferredSize() == prefSizeBefore);
                    };
                };
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

