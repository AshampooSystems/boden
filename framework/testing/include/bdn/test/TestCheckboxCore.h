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
                P<Checkbox> pCheckbox = newObj<Checkbox>();
                pCheckbox->setLabel("hello");

                return pCheckbox;
            }

            void setView(View *pView) override
            {
                TestViewCore::setView(pView);

                _pCheckbox = cast<Checkbox>(pView);
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
                        _pCheckbox->setLabel("helloworld");
                        initCore();
                        verifyCoreLabel();
                    }

                    // state set before init
                    SECTION("state")
                    {
                        _pCheckbox->setState(TriState::on);
                        initCore();
                        verifyCoreState();
                    }
                }
            }

            void runPostInitTests() override
            {
                P<TestCheckboxCore> pThis(this);

                TestViewCore::runPostInitTests();

                // Set label from framework
                SECTION("label")
                {
                    SECTION("value")
                    {
                        _pCheckbox->setLabel("helloworld");

                        CONTINUE_SECTION_WHEN_IDLE(pThis) { pThis->verifyCoreLabel(); };
                    }

                    SECTION("effectsOnPreferredSize")
                    {
                        String labelBefore = _pCheckbox->label();

                        // the label should not be empty here
                        REQUIRE(labelBefore.getLength() > 3);

                        Size prefSizeBefore = _pCheckbox->calcPreferredSize();

                        _pCheckbox->setLabel(labelBefore + labelBefore + labelBefore);

                        CONTINUE_SECTION_WHEN_IDLE(pThis, prefSizeBefore, labelBefore)
                        {
                            Size prefSize = pThis->_pCheckbox->calcPreferredSize();

                            // width must increase with a bigger label
                            REQUIRE(prefSize.width > prefSizeBefore.width);

                            // note that the height might or might not increase.
                            // But it cannot be smaller.
                            REQUIRE(prefSize.height >= prefSizeBefore.height);

                            // when we go back to the same label as before then
                            // the preferred size should also be the same again
                            pThis->_pCheckbox->setLabel(labelBefore);

                            CONTINUE_SECTION_WHEN_IDLE(pThis, labelBefore, prefSizeBefore)
                            {
                                REQUIRE(pThis->_pCheckbox->calcPreferredSize() == prefSizeBefore);
                            };
                        };
                    }
                }

                // Set on from framework and via platform
                SECTION("state")
                {
                    SECTION("valueOn")
                    {
                        _pCheckbox->setState(TriState::on);

                        CONTINUE_SECTION_WHEN_IDLE(pThis) { pThis->verifyCoreState(); };
                    }

                    SECTION("valueOff")
                    {
                        _pCheckbox->setState(TriState::off);

                        CONTINUE_SECTION_WHEN_IDLE(pThis) { pThis->verifyCoreState(); };
                    }

                    SECTION("valueMixed")
                    {
                        _pCheckbox->setState(TriState::mixed);

                        CONTINUE_SECTION_WHEN_IDLE(pThis) { pThis->verifyCoreState(); };
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

            P<Checkbox> _pCheckbox;
        };
    }
}

#endif
