#ifndef BDN_TEST_TestTextFieldCore_H_
#define BDN_TEST_TestTextFieldCore_H_

#include <bdn/test/TestViewCore.h>
#include <bdn/TextField.h>

namespace bdn
{
    namespace test
    {

        /** Helper for tests that verify ITextFieldCore implementations.*/
        class TestTextFieldCore : public TestViewCore<TextField>
        {

          protected:
            P<View> createView() override
            {
                P<TextField> pTextField = newObj<TextField>();
                pTextField->setText("hello");

                return pTextField;
            }

            void setView(View *pView) override
            {
                TestViewCore::setView(pView);
                _pTextField = cast<TextField>(pView);
            }

            void runInitTests() override
            {
                TestViewCore::runInitTests();

                SECTION("text")
                {
                    _pTextField->setText("helloworld");
                    initCore();
                    verifyCoreText();
                }
            }

            void runPostInitTests() override
            {
                P<TestTextFieldCore> pThis(this);

                TestViewCore::runPostInitTests();

                SECTION("text")
                {
                    SECTION("value")
                    {
                        _pTextField->setText("helloworld");

                        CONTINUE_SECTION_WHEN_IDLE(pThis) { pThis->verifyCoreText(); };
                    }

                    // Text should not affect preferred size
                    SECTION("effectsOnPreferredSize")
                    {
                        String textBefore = _pTextField->text();

                        // the text should not be empty here
                        REQUIRE(textBefore.getLength() > 3);

                        Size prefSizeBefore = _pTextField->calcPreferredSize();

                        _pTextField->setText(textBefore + textBefore + textBefore);

                        Size prefSizeAfter = _pTextField->calcPreferredSize();

                        REQUIRE(prefSizeAfter == prefSizeBefore);
                    }
                }
            }

            virtual void verifyCoreText() = 0;

            P<TextField> _pTextField;
        };
    }
}

#endif
