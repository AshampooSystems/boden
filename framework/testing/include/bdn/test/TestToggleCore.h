#pragma once

#include <bdn/test/TestViewCore.h>
#include <bdn/Toggle.h>

namespace bdn
{
    namespace test
    {

        /** Helper for tests that verify IToggleCore implementations.*/
        class TestToggleCore : public TestViewCore<Toggle>
        {

          protected:
            std::shared_ptr<View> createView() override
            {
                std::shared_ptr<Toggle> toggle = std::make_shared<Toggle>();
                toggle->label = ("hello");

                return toggle;
            }

            void setView(std::shared_ptr<View> view) override
            {
                TestViewCore::setView(view);

                _toggle = std::dynamic_pointer_cast<Toggle>(view);
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
                        _toggle->label = ("helloworld");
                        initCore();
                        verifyCoreLabel();
                    }

                    // on set before init
                    SECTION("on")
                    {
                        _toggle->on = (true);
                        initCore();
                        verifyCoreOn();
                    }
                }
            }

            void runPostInitTests() override
            {
                std::shared_ptr<TestToggleCore> self = std::dynamic_pointer_cast<TestToggleCore>(shared_from_this());

                TestViewCore::runPostInitTests();

                // Set label from framework
                SECTION("label")
                {
                    SECTION("value")
                    {
                        _toggle->label = ("helloworld");

                        CONTINUE_SECTION_WHEN_IDLE(self) { self->verifyCoreLabel(); };
                    }

                    SECTION("effectsOnPreferredSize")
                    {
                        String labelBefore = _toggle->label;

                        // the label should not be empty here
                        REQUIRE(labelBefore.size() > 3);

                        Size prefSizeBefore = _toggle->calcPreferredSize();

                        _toggle->label = (labelBefore + labelBefore + labelBefore);

                        CONTINUE_SECTION_WHEN_IDLE(self, prefSizeBefore, labelBefore)
                        {
                            Size prefSize = self->_toggle->calcPreferredSize();

                            // width must increase with a bigger label
                            REQUIRE(prefSize.width > prefSizeBefore.width);

                            // note that the height might or might not increase.
                            // But it cannot be smaller.
                            REQUIRE(prefSize.height >= prefSizeBefore.height);

                            // when we go back to the same label as before then
                            // the preferred size should also be the same again
                            self->_toggle->label = (labelBefore);

                            CONTINUE_SECTION_WHEN_IDLE(self, labelBefore, prefSizeBefore)
                            {
                                REQUIRE(self->_toggle->calcPreferredSize() == prefSizeBefore);
                            };
                        };
                    }
                }

                // Set on from framework and via platform
                SECTION("on")
                {
                    SECTION("valueTrue")
                    {
                        _toggle->on = (true);

                        CONTINUE_SECTION_WHEN_IDLE(self) { self->verifyCoreOn(); };
                    }

                    SECTION("valueFalse")
                    {
                        _toggle->on = (false);

                        CONTINUE_SECTION_WHEN_IDLE(self) { self->verifyCoreOn(); };
                    }
                }
            }

            // Pure virtual methods to be implemented by platform-specific core
            // tests

            /** Verifies that the toggle core's label has the expected value
                (the label set in the outer toggle object's Toggle::label()
               property.*/
            virtual void verifyCoreLabel() = 0;

            /** Verifies that the toggle core's on state has the expected value
                (the on state set in the outer toggle object's Toggle::on()
               property.*/
            virtual void verifyCoreOn() = 0;

            /** Sets the toggle's on state on the platform only without using
               the framework. This is used to test whether the core correctly
               notifies its outer control
                when the toggle state has been changed by a platform event. */
            /*virtual void setCoreOnPlatform(bool on)=0;*/

            std::shared_ptr<Toggle> _toggle;
        };
    }
}
