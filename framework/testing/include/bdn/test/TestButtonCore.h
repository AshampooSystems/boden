#pragma once

#include <bdn/Button.h>
#include <bdn/test/TestViewCore.h>

namespace bdn
{
    namespace test
    {

        /** Helper for tests that verify IButtonCore implementations.*/
        class TestButtonCore : public TestViewCore<Button>
        {

          protected:
            std::shared_ptr<View> createView() override
            {
                std::shared_ptr<Button> button = std::make_shared<Button>();
                button->label = ("hello");

                return button;
            }

            void setView(std::shared_ptr<View> view) override
            {
                TestViewCore::setView(view);

                _button = std::dynamic_pointer_cast<Button>(view);
            }

            void runInitTests() override
            {
                TestViewCore::runInitTests();

                SECTION("label")
                {
                    _button->label = ("helloworld");
                    initCore();
                    verifyCoreLabel();
                }
            }

            void runPostInitTests() override
            {
                std::shared_ptr<TestButtonCore> self = std::dynamic_pointer_cast<TestButtonCore>(shared_from_this());

                TestViewCore::runPostInitTests();

                SECTION("label")
                {
                    SECTION("value")
                    {
                        _button->label = ("helloworld");

                        CONTINUE_SECTION_WHEN_IDLE(self) { self->verifyCoreLabel(); };
                    }

                    SECTION("effectsOnPreferredSize")
                    {
                        String labelBefore = _button->label;

                        // the label should not be empty here
                        REQUIRE(labelBefore.size() > 3);

                        Size prefSizeBefore = _button->calcPreferredSize();

                        _button->label = (labelBefore + labelBefore + labelBefore);

                        Size prefSize = self->_button->calcPreferredSize();

                        // width must increase with a bigger label
                        REQUIRE(prefSize.width > prefSizeBefore.width);

                        // note that the height might or might not increase. But
                        // it cannot be smaller.
                        REQUIRE(prefSize.height >= prefSizeBefore.height);

                        // when we go back to the same label as before then the
                        // preferred size should also be the same again
                        self->_button->label = (labelBefore);

                        REQUIRE(self->_button->calcPreferredSize() == prefSizeBefore);
                    }
                }
            }

            /** Verifies that the button core's label has the expected value
                (the label set in the outer button object's Button::label()
               property.*/
            virtual void verifyCoreLabel() = 0;

            std::shared_ptr<Button> _button;
        };
    }
}
