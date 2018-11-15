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
                P<Switch> pSwitch = newObj<Switch>();
                pSwitch->setLabel("hello");

                return pSwitch;
            }

            void setView(View *pView) override
            {
                TestViewCore::setView(pView);

                _pSwitch = cast<Switch>(pView);
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
                        _pSwitch->setLabel("helloworld");
                        initCore();
                        verifyCoreLabel();
                    }

                    // on set before init
                    SECTION("on")
                    {
                        _pSwitch->setOn(true);
                        initCore();
                        verifyCoreOn();
                    }
                }
            }

            void runPostInitTests() override
            {
                P<TestSwitchCore> pThis(this);

                TestViewCore::runPostInitTests();

                // Set label from framework
                SECTION("label")
                {
                    SECTION("value")
                    {
                        _pSwitch->setLabel("helloworld");

                        CONTINUE_SECTION_WHEN_IDLE(pThis) { pThis->verifyCoreLabel(); };
                    }

                    SECTION("effectsOnPreferredSize")
                    {
                        String labelBefore = _pSwitch->label();

                        // the label should not be empty here
                        REQUIRE(labelBefore.getLength() > 3);

                        Size prefSizeBefore = _pSwitch->calcPreferredSize();

                        _pSwitch->setLabel(labelBefore + labelBefore + labelBefore);

                        CONTINUE_SECTION_WHEN_IDLE(pThis, prefSizeBefore, labelBefore)
                        {
                            Size prefSize = pThis->_pSwitch->calcPreferredSize();

                            // width must increase with a bigger label
                            REQUIRE(prefSize.width > prefSizeBefore.width);

                            // note that the height might or might not increase.
                            // But it cannot be smaller.
                            REQUIRE(prefSize.height >= prefSizeBefore.height);

                            // when we go back to the same label as before then
                            // the preferred size should also be the same again
                            pThis->_pSwitch->setLabel(labelBefore);

                            CONTINUE_SECTION_WHEN_IDLE(pThis, labelBefore, prefSizeBefore)
                            {
                                REQUIRE(pThis->_pSwitch->calcPreferredSize() == prefSizeBefore);
                            };
                        };
                    }
                }

                // Set on from framework and via platform
                SECTION("on")
                {
                    SECTION("valueTrue")
                    {
                        _pSwitch->setOn(true);

                        CONTINUE_SECTION_WHEN_IDLE(pThis) { pThis->verifyCoreOn(); };
                    }

                    SECTION("valueFalse")
                    {
                        _pSwitch->setOn(false);

                        CONTINUE_SECTION_WHEN_IDLE(pThis) { pThis->verifyCoreOn(); };
                    }

                    // We decided to NOT test the case where the platform state
                    // is set programatiacally
                    /*SECTION("platform")
                    {
                        // If core is implemented correctly, this will notify
                    the outer control (_pSwitch)
                        // that on has been changed externally using mechanisms
                    provided by the platform core. setCoreOnPlatform(true);

                        CONTINUE_SECTION_WHEN_IDLE(pThis)
                        {
                            pThis->verifyCoreOn();
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

            P<Switch> _pSwitch;
        };
    }
}

#endif
