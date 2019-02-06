#pragma once

#include <bdn/test/TestViewCore.h>
#include <bdn/Switch.h>

namespace bdn
{
    namespace test
    {

        /** Helper for tests that verify SwitchCore implementations.*/
        class TestSwitchCore : public TestViewCore<Switch>
        {

          protected:
            std::shared_ptr<View> createView() override
            {
                std::shared_ptr<Switch> switchControl = std::make_shared<Switch>();
                switchControl->label = ("hello");

                return switchControl;
            }

            void setView(std::shared_ptr<View> view) override
            {
                TestViewCore::setView(view);

                _switch = std::dynamic_pointer_cast<Switch>(view);
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
                        _switch->label = ("helloworld");
                        initCore();
                        verifyCoreLabel();
                    }

                    // on set before init
                    SECTION("on")
                    {
                        _switch->on = (true);
                        initCore();
                        verifyCoreOn();
                    }
                }
            }

            void runPostInitTests() override
            {
                std::shared_ptr<TestSwitchCore> self = std::dynamic_pointer_cast<TestSwitchCore>(shared_from_this());

                TestViewCore::runPostInitTests();

                // Set label from framework
                SECTION("label")
                {
                    SECTION("value")
                    {
                        _switch->label = ("helloworld");

                        CONTINUE_SECTION_WHEN_IDLE(self) { self->verifyCoreLabel(); };
                    }

                    SECTION("effectsOnPreferredSize")
                    {
                        String labelBefore = _switch->label;

                        // the label should not be empty here
                        REQUIRE(labelBefore.size() > 3);

                        Size prefSizeBefore = _switch->calcPreferredSize();

                        _switch->label = (labelBefore + labelBefore + labelBefore);

                        CONTINUE_SECTION_WHEN_IDLE(self, prefSizeBefore, labelBefore)
                        {
                            Size prefSize = self->_switch->calcPreferredSize();

                            // width must increase with a bigger label
                            REQUIRE(prefSize.width > prefSizeBefore.width);

                            // note that the height might or might not increase.
                            // But it cannot be smaller.
                            REQUIRE(prefSize.height >= prefSizeBefore.height);

                            // when we go back to the same label as before then
                            // the preferred size should also be the same again
                            self->_switch->label = (labelBefore);

                            CONTINUE_SECTION_WHEN_IDLE(self, labelBefore, prefSizeBefore)
                            {
                                REQUIRE(self->_switch->calcPreferredSize() == prefSizeBefore);
                            };
                        };
                    }
                }

                // Set on from framework and via platform
                SECTION("on")
                {
                    SECTION("valueTrue")
                    {
                        _switch->on = (true);

                        CONTINUE_SECTION_WHEN_IDLE(self) { self->verifyCoreOn(); };
                    }

                    SECTION("valueFalse")
                    {
                        _switch->on = (false);

                        CONTINUE_SECTION_WHEN_IDLE(self) { self->verifyCoreOn(); };
                    }

                    // We decided to NOT test the case where the platform state
                    // is set programatiacally
                    /*SECTION("platform")
                    {
                        // If core is implemented correctly, this will notify
                    the outer control (_switch)
                        // that on has been changed externally using mechanisms
                    provided by the platform core. setCoreOnPlatform(true);

                        CONTINUE_SECTION_WHEN_IDLE(this)
                        {
                            this->verifyCoreOn();
                        };
                    }*/
                }
            }

            // Pure virtual methods to be implemented by platform-specific core
            // tests

            /** Verifies that the Switch core's label has the expected value
                (the label set in the outer Switch object's Switch::label()
               property.*/
            virtual void verifyCoreLabel() = 0;

            /** Verifies that the Switch core's on state has the expected value
                (the on state set in the outer Switch object's Switch::on()
               property.*/
            virtual void verifyCoreOn() = 0;

            /** Sets the Switch's on state on the platform only without using
               the framework. This is used to test whether the core correctly
               notifies its outer control
                when the Switch state has been changed by a platform event. */
            /*virtual void setCoreOnPlatform(bool on)=0;*/

            std::shared_ptr<Switch> _switch;
        };
    }
}
