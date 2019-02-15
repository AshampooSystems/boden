#pragma once

#include <bdn/Checkbox.h>
#include <bdn/test/TestViewCore.h>

namespace bdn
{
    namespace test
    {

        /** Helper for tests that verify CheckboxCore implementations.*/
        class TestCheckboxCore : public TestViewCore<Checkbox>
        {

          protected:
            std::shared_ptr<View> createView() override
            {
                std::shared_ptr<Checkbox> checkbox = std::make_shared<Checkbox>();
                checkbox->label = ("hello");

                return checkbox;
            }

            void setView(std::shared_ptr<View> view) override
            {
                TestViewCore::setView(view);

                _checkbox = std::dynamic_pointer_cast<Checkbox>(view);
            }

            void runInitTests() override
            {
                TestViewCore::runInitTests();

                // Test correctness of core initialization
                SECTION("init")
                {
                    // label set before init
                    SECTION("label")
                    {
                        _checkbox->label = ("helloworld");
                        initCore();
                        verifyCoreLabel();
                    }

                    // state set before init
                    SECTION("state")
                    {
                        _checkbox->state = (TriState::on);
                        initCore();
                        verifyCoreState();
                    }
                }
            }

            void runPostInitTests() override
            {
                std::shared_ptr<TestCheckboxCore> self =
                    std::dynamic_pointer_cast<TestCheckboxCore>(shared_from_this());

                TestViewCore::runPostInitTests();

                // Set label from framework
                SECTION("label")
                {
                    SECTION("value")
                    {
                        _checkbox->label = ("helloworld");

                        CONTINUE_SECTION_WHEN_IDLE(self) { self->verifyCoreLabel(); };
                    }

                    SECTION("effectsOnPreferredSize")
                    {
                        String labelBefore = _checkbox->label;

                        // the label should not be empty here
                        REQUIRE(labelBefore.size() > 3);

                        Size prefSizeBefore = _checkbox->calcPreferredSize();

                        _checkbox->label = (labelBefore + labelBefore + labelBefore);

                        CONTINUE_SECTION_WHEN_IDLE(self, prefSizeBefore, labelBefore)
                        {
                            Size prefSize = self->_checkbox->calcPreferredSize();

                            // width must increase with a bigger label
                            REQUIRE(prefSize.width > prefSizeBefore.width);

                            // note that the height might or might not increase.
                            // But it cannot be smaller.
                            REQUIRE(prefSize.height >= prefSizeBefore.height);

                            // when we go back to the same label as before then
                            // the preferred size should also be the same again
                            self->_checkbox->label = (labelBefore);

                            CONTINUE_SECTION_WHEN_IDLE(self, labelBefore, prefSizeBefore)
                            {
                                REQUIRE(self->_checkbox->calcPreferredSize() == prefSizeBefore);
                            };
                        };
                    }
                }

                // Set on from framework and via platform
                SECTION("state")
                {
                    SECTION("valueOn")
                    {
                        _checkbox->state = (TriState::on);

                        CONTINUE_SECTION_WHEN_IDLE(self) { self->verifyCoreState(); };
                    }

                    SECTION("valueOff")
                    {
                        _checkbox->state = (TriState::off);

                        CONTINUE_SECTION_WHEN_IDLE(self) { self->verifyCoreState(); };
                    }

                    SECTION("valueMixed")
                    {
                        _checkbox->state = (TriState::mixed);

                        CONTINUE_SECTION_WHEN_IDLE(self) { self->verifyCoreState(); };
                    }
                }
            }

            // Pure virtual methods to be implemented by platform-specific core
            // tests

            /** Verifies that the checkbox's core's label has the expected value
                (the label set in the outer checkbox object's Checkbox::label()
               property.*/
            virtual void verifyCoreLabel() = 0;

            /** Verifies that the checkbox's core's on state has the expected
               value (the on state set in the outer checkbox object's
               Checkbox::on() property.*/
            virtual void verifyCoreState() = 0;

            std::shared_ptr<Checkbox> _checkbox;
        };
    }
}
