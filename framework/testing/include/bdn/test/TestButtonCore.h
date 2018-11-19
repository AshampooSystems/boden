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
                P<Button> button = newObj<Button>();
                button->setLabel("hello");

                return button;
            }

            void setView(View *view) override
            {
                TestViewCore::setView(view);

                _button = cast<Button>(view);
            }

            void runInitTests() override
            {
                TestViewCore::runInitTests();

                SECTION("label")
                {
                    _button->setLabel("helloworld");
                    initCore();
                    verifyCoreLabel();
                }
            }

            void runPostInitTests() override
            {
                P<TestButtonCore> self(this);

                TestViewCore::runPostInitTests();

                SECTION("label")
                {
                    SECTION("value")
                    {
                        _button->setLabel("helloworld");

                        CONTINUE_SECTION_WHEN_IDLE(self) { self->verifyCoreLabel(); };
                    }

                    SECTION("effectsOnPreferredSize")
                    {
                        String labelBefore = _button->label();

                        // the label should not be empty here
                        REQUIRE(labelBefore.getLength() > 3);

                        Size prefSizeBefore = _button->calcPreferredSize();

                        _button->setLabel(labelBefore + labelBefore + labelBefore);

                        Size prefSize = self->_button->calcPreferredSize();

                        // width must increase with a bigger label
                        REQUIRE(prefSize.width > prefSizeBefore.width);

                        // note that the height might or might not increase. But
                        // it cannot be smaller.
                        REQUIRE(prefSize.height >= prefSizeBefore.height);

                        // when we go back to the same label as before then the
                        // preferred size should also be the same again
                        self->_button->setLabel(labelBefore);

                        REQUIRE(self->_button->calcPreferredSize() == prefSizeBefore);
                    }
                }
            }

            /** Verifies that the button core's label has the expected value
                (the label set in the outer button object's Button::label()
               property.*/
            virtual void verifyCoreLabel() = 0;

            P<Button> _button;
        };
    }
}

#endif
