#ifndef BDN_TEST_TestToggleCore_H_
#define BDN_TEST_TestToggleCore_H_

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
            P<View> createView() override
            {
                P<Toggle> pToggle = newObj<Toggle>();
                pToggle->setLabel("hello");

                return pToggle;
            }

            void setView(View *pView) override
            {
                TestViewCore::setView(pView);

                _pToggle = cast<Toggle>(pView);
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
                        _pToggle->setLabel("helloworld");
                        initCore();
                        verifyCoreLabel();
                    }

                    // on set before init
                    SECTION("on")
                    {
                        _pToggle->setOn(true);
                        initCore();
                        verifyCoreOn();
                    }
                }
            }

            void runPostInitTests() override
            {
                P<TestToggleCore> pThis(this);

                TestViewCore::runPostInitTests();

                // Set label from framework
                SECTION("label")
                {
                    SECTION("value")
                    {
                        _pToggle->setLabel("helloworld");

                        CONTINUE_SECTION_WHEN_IDLE(pThis)
                        {
                            pThis->verifyCoreLabel();
                        };
                    }

                    SECTION("effectsOnPreferredSize")
                    {
                        String labelBefore = _pToggle->label();

                        // the label should not be empty here
                        REQUIRE(labelBefore.getLength() > 3);

                        Size prefSizeBefore = _pToggle->calcPreferredSize();

                        _pToggle->setLabel(labelBefore + labelBefore +
                                           labelBefore);

                        CONTINUE_SECTION_WHEN_IDLE(pThis, prefSizeBefore,
                                                   labelBefore)
                        {
                            Size prefSize =
                                pThis->_pToggle->calcPreferredSize();

                            // width must increase with a bigger label
                            REQUIRE(prefSize.width > prefSizeBefore.width);

                            // note that the height might or might not increase.
                            // But it cannot be smaller.
                            REQUIRE(prefSize.height >= prefSizeBefore.height);

                            // when we go back to the same label as before then
                            // the preferred size should also be the same again
                            pThis->_pToggle->setLabel(labelBefore);

                            CONTINUE_SECTION_WHEN_IDLE(pThis, labelBefore,
                                                       prefSizeBefore)
                            {
                                REQUIRE(pThis->_pToggle->calcPreferredSize() ==
                                        prefSizeBefore);
                            };
                        };
                    }
                }

                // Set on from framework and via platform
                SECTION("on")
                {
                    SECTION("valueTrue")
                    {
                        _pToggle->setOn(true);

                        CONTINUE_SECTION_WHEN_IDLE(pThis)
                        {
                            pThis->verifyCoreOn();
                        };
                    }

                    SECTION("valueFalse")
                    {
                        _pToggle->setOn(false);

                        CONTINUE_SECTION_WHEN_IDLE(pThis)
                        {
                            pThis->verifyCoreOn();
                        };
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

            P<Toggle> _pToggle;
        };
    }
}

#endif
