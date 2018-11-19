#ifndef BDN_TEST_TestSwitchCore_H_
#define BDN_TEST_TestSwitchCore_H_

#include <bdn/test/TestViewCore.h>
#include <bdn/Switch.h>

namespace bdn
{
    namespace test
    {

        /** Helper for tests that verify ISwitchCore implementations.*/
        class TestSwitchCore : public TestViewCore<Switch>
        {

          protected:
            P<View> createView() override
            {
                P<Switch> switchControl = newObj<Switch>();
                switchControl->setLabel("hello");

                return switchControl;
            }

            void setView(View *view) override
            {
                TestViewCore::setView(view);

                _switch = cast<Switch>(view);
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
                        _switch->setLabel("helloworld");
                        initCore();
                        verifyCoreLabel();
                    }

                    // on set before init
                    SECTION("on")
                    {
                        _switch->setOn(true);
                        initCore();
                        verifyCoreOn();
                    }
                }
            }

            void runPostInitTests() override
            {
                P<TestSwitchCore> self(this);

                TestViewCore::runPostInitTests();

                // Set label from framework
                SECTION("label")
                {
                    SECTION("value")
                    {
                        _switch->setLabel("helloworld");

                        CONTINUE_SECTION_WHEN_IDLE(self) { self->verifyCoreLabel(); };
                    }

                    SECTION("effectsOnPreferredSize")
                    {
                        String labelBefore = _switch->label();

                        // the label should not be empty here
                        REQUIRE(labelBefore.getLength() > 3);

                        Size prefSizeBefore = _switch->calcPreferredSize();

                        _switch->setLabel(labelBefore + labelBefore + labelBefore);

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
                            self->_switch->setLabel(labelBefore);

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
                        _switch->setOn(true);

                        CONTINUE_SECTION_WHEN_IDLE(self) { self->verifyCoreOn(); };
                    }

                    SECTION("valueFalse")
                    {
                        _switch->setOn(false);

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

            P<Switch> _switch;
        };
    }
}

#endif
