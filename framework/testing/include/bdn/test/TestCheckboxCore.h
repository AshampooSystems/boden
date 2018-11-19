#ifndef BDN_TEST_TestCheckboxCore_H_
#define BDN_TEST_TestCheckboxCore_H_

#include <bdn/test/TestViewCore.h>
#include <bdn/Checkbox.h>

namespace bdn
{
    namespace test
    {

        /** Helper for tests that verify ICheckboxCore implementations.*/
        class TestCheckboxCore : public TestViewCore<Checkbox>
        {

          protected:
            P<View> createView() override
            {
                P<Checkbox> checkbox = newObj<Checkbox>();
                checkbox->setLabel("hello");

                return checkbox;
            }

            void setView(View *view) override
            {
                TestViewCore::setView(view);

                _checkbox = cast<Checkbox>(view);
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
                        _checkbox->setLabel("helloworld");
                        initCore();
                        verifyCoreLabel();
                    }

                    // state set before init
                    SECTION("state")
                    {
                        _checkbox->setState(TriState::on);
                        initCore();
                        verifyCoreState();
                    }
                }
            }

            void runPostInitTests() override
            {
                P<TestCheckboxCore> self(this);

                TestViewCore::runPostInitTests();

                // Set label from framework
                SECTION("label")
                {
                    SECTION("value")
                    {
                        _checkbox->setLabel("helloworld");

                        CONTINUE_SECTION_WHEN_IDLE(self) { self->verifyCoreLabel(); };
                    }

                    SECTION("effectsOnPreferredSize")
                    {
                        String labelBefore = _checkbox->label();

                        // the label should not be empty here
                        REQUIRE(labelBefore.getLength() > 3);

                        Size prefSizeBefore = _checkbox->calcPreferredSize();

                        _checkbox->setLabel(labelBefore + labelBefore + labelBefore);

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
                            self->_checkbox->setLabel(labelBefore);

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
                        _checkbox->setState(TriState::on);

                        CONTINUE_SECTION_WHEN_IDLE(self) { self->verifyCoreState(); };
                    }

                    SECTION("valueOff")
                    {
                        _checkbox->setState(TriState::off);

                        CONTINUE_SECTION_WHEN_IDLE(self) { self->verifyCoreState(); };
                    }

                    SECTION("valueMixed")
                    {
                        _checkbox->setState(TriState::mixed);

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

            P<Checkbox> _checkbox;
        };
    }
}

#endif
