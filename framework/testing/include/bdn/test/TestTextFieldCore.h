#pragma once

#include <bdn/TextField.h>
#include <bdn/test/TestViewCore.h>

namespace bdn
{
    namespace test
    {

        /** Helper for tests that verify ITextFieldCore implementations.*/
        class TestTextFieldCore : public TestViewCore<TextField>
        {

          protected:
            std::shared_ptr<View> createView() override
            {
                std::shared_ptr<TextField> textField = std::make_shared<TextField>();
                textField->text = ("hello");

                return textField;
            }

            void setView(std::shared_ptr<View> view) override
            {
                TestViewCore::setView(view);
                _textField = std::dynamic_pointer_cast<TextField>(view);
            }

            void runInitTests() override
            {
                TestViewCore::runInitTests();

                SECTION("text")
                {
                    _textField->text = ("helloworld");
                    initCore();
                    verifyCoreText();
                }
            }

            void runPostInitTests() override
            {
                std::shared_ptr<TestTextFieldCore> self =
                    std::dynamic_pointer_cast<TestTextFieldCore>(shared_from_this());

                TestViewCore::runPostInitTests();

                SECTION("text")
                {
                    SECTION("value")
                    {
                        _textField->text = ("helloworld");

                        CONTINUE_SECTION_WHEN_IDLE(self) { self->verifyCoreText(); };
                    }

                    // Text should not affect preferred size
                    SECTION("effectsOnPreferredSize")
                    {
                        String textBefore = _textField->text;

                        // the text should not be empty here
                        REQUIRE(textBefore.size() > 3);

                        Size prefSizeBefore = _textField->calcPreferredSize();

                        _textField->text = (textBefore + textBefore + textBefore);

                        Size prefSizeAfter = _textField->calcPreferredSize();

                        REQUIRE(prefSizeAfter == prefSizeBefore);
                    }
                }
            }

            virtual void verifyCoreText() = 0;

            std::shared_ptr<TextField> _textField;
        };
    }
}
