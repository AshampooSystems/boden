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
                P<TextField> textField = newObj<TextField>();
                textField->setText("hello");

                return textField;
            }

            void setView(View *view) override
            {
                TestViewCore::setView(view);
                _textField = cast<TextField>(view);
            }

            void runInitTests() override
            {
                TestViewCore::runInitTests();

                SECTION("text")
                {
                    _textField->setText("helloworld");
                    initCore();
                    verifyCoreText();
                }
            }

            void runPostInitTests() override
            {
                P<TestTextFieldCore> self(this);

                TestViewCore::runPostInitTests();

                SECTION("text")
                {
                    SECTION("value")
                    {
                        _textField->setText("helloworld");

                        CONTINUE_SECTION_WHEN_IDLE(self) { self->verifyCoreText(); };
                    }

                    // Text should not affect preferred size
                    SECTION("effectsOnPreferredSize")
                    {
                        String textBefore = _textField->text();

                        // the text should not be empty here
                        REQUIRE(textBefore.getLength() > 3);

                        Size prefSizeBefore = _textField->calcPreferredSize();

                        _textField->setText(textBefore + textBefore + textBefore);

                        Size prefSizeAfter = _textField->calcPreferredSize();

                        REQUIRE(prefSizeAfter == prefSizeBefore);
                    }
                }
            }

            virtual void verifyCoreText() = 0;

            P<TextField> _textField;
        };
    }
}

#endif
