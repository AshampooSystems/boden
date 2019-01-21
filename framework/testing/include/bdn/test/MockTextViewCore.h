#pragma once

#include <bdn/test/MockViewCore.h>

#include <bdn/ITextViewCore.h>
#include <bdn/TextView.h>

namespace bdn
{
    namespace test
    {

        /** Implementation of a "fake" text view core that does not actually
           show anything visible, but behaves otherwise like a normal text view
           core.

            See MockUiProvider.
            */
        class MockTextViewCore : public MockViewCore, virtual public ITextViewCore
        {
          public:
            MockTextViewCore(std::shared_ptr<TextView> view) : MockViewCore(view) { _text = view->text; }

            /** Returns the current text content of the text view.*/
            String getText() const { return _text; }

            /** Returns the number of times that a new text has been set.*/
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

                Size preferredSizeHint(Size::none());
                Margin padding;

                std::shared_ptr<View> view = getOuterViewIfStillAttached();
                if (view != nullptr) {
                    if (view->padding.get())
                        padding = uiMarginToDipMargin(*view->padding.get());

                    preferredSizeHint = view->preferredSizeHint;
                }

                double wrapWidth = preferredSizeHint.width;

                if (std::isfinite(availableSpace.width) && availableSpace.width < wrapWidth)
                    wrapWidth = availableSpace.width;

                if (std::isfinite(wrapWidth))
                    wrapWidth -= padding.left + padding.right;

                if (wrapWidth < 0)
                    wrapWidth = 0;

                Size size = _getTextSize(_text, wrapWidth);

                size += padding;

                if (view != nullptr) {
                    // clip to min and max size
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
