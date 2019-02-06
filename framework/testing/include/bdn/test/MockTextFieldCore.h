#pragma once

#include <bdn/test/MockViewCore.h>

#include <bdn/ITextFieldCore.h>
#include <bdn/TextField.h>

namespace bdn
{
    namespace test
    {

        /** Implementation of a "fake" TextField core that does not actually
           show anything visible, but behaves otherwise like a normal TextField
           core.

            See MockUIProvider.
            */
        class MockTextFieldCore : public MockViewCore, virtual public ITextFieldCore
        {
          public:
            MockTextFieldCore(std::shared_ptr<TextField> textField) : MockViewCore(textField)
            {
                _text = textField->text;
            }

            String getText() const { return _text; }

            int getTextChangeCount() const { return _textChangeCount; }

            void setText(const String &text) override
            {
                _text = text;
                _textChangeCount++;
            }

            Size calcPreferredSize(const Size &availableSpace = Size::none()) const override
            {
                MockViewCore::calcPreferredSize(availableSpace);

                BDN_REQUIRE_IN_MAIN_THREAD();

                Margin padding;

                std::shared_ptr<View> view = getOuterViewIfStillAttached();
                if (view != nullptr) {
                    if (view->padding.get())
                        padding = uiMarginToDipMargin(*view->padding.get());
                }

                // Calculate size from line height + padding
                Size textSize = _getTextSize(_text);
                Size size = Size(0, textSize.height) + padding;

                if (view != nullptr) {
                    // Clip to min and max size
                    size.applyMinimum(view->preferredSizeMinimum);
                    size.applyMaximum(view->preferredSizeMaximum);
                }

                return size;
            }

          protected:
            String _text;
            int _textChangeCount = 0;
        };
    }
}
