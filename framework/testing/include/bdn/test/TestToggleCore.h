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
                P<Toggle> toggle = newObj<Toggle>();
                toggle->setLabel("hello");

                return toggle;
            }

            void setView(View *view) override
            {
                TestViewCore::setView(view);

                _toggle = cast<Toggle>(view);
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
                        _toggle->setLabel("helloworld");
                        initCore();
                        verifyCoreLabel();
                    }

                    // on set before init
                    SECTION("on")
                    {
                        _toggle->setOn(true);
                        initCore();
                        verifyCoreOn();
                    }
                }
            }

            void runPostInitTests() override
            {
                P<TestToggleCore> self(this);

                TestViewCore::runPostInitTests();

                // Set label from framework
                SECTION("label")
                {
                    SECTION("value")
                    {
                        _toggle->setLabel("helloworld");

                        CONTINUE_SECTION_WHEN_IDLE(self) { self->verifyCoreLabel(); };
                    }

                    SECTION("effectsOnPreferredSize")
                    {
                        String labelBefore = _toggle->label();

                        // the label should not be empty here
                        REQUIRE(labelBefore.getLength() > 3);

                        Size prefSizeBefore = _toggle->calcPreferredSize();

                        _toggle->setLabel(labelBefore + labelBefore + labelBefore);

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
                            self->_toggle->setLabel(labelBefore);

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
                        _toggle->setOn(true);

                        CONTINUE_SECTION_WHEN_IDLE(self) { self->verifyCoreOn(); };
                    }

                    SECTION("valueFalse")
                    {
                        _toggle->setOn(self);

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

            P<Toggle> _toggle;
        };
    }
}

#endif
