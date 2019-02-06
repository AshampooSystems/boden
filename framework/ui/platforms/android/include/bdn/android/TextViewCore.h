#pragma once

#include <bdn/android/ViewCore.h>
#include <bdn/android/JTextView.h>
#include <bdn/android/JRStyle.h>
#include <bdn/ITextViewCore.h>
#include <bdn/TextView.h>

namespace bdn
{
    namespace android
    {

        class TextViewCore : public ViewCore, virtual public ITextViewCore
        {
          public:
            TextViewCore(std::shared_ptr<TextView> outerTextView)
                : ViewCore(outerTextView, ViewCore::createAndroidViewClass<JTextView>(outerTextView)),
                  _jTextView(getJViewAS<JTextView>())
            {
                setText(outerTextView->text);
            }

            JTextView &getJTextView() { return _jTextView; }

            void setText(const String &text) override
            {
                // Android treats carriage return like a space. So we filter
                // those out.
                String textToSet = text;
                // textToSet.findAndReplace("\r", "");
                textToSet.erase(
                    std::remove_if(textToSet.begin(), textToSet.end(), [](unsigned char x) { return x == '\r'; }),
                    textToSet.end());

                _jTextView.setText(textToSet);

                // we must re-layout the button - otherwise its preferred size
                // is not updated.
                _jTextView.requestLayout();
            }

            Rect adjustAndSetBounds(const Rect &requestedBounds) override
            {
                Rect adjustedBounds = ViewCore::adjustAndSetBounds(requestedBounds);

                // for some reason the TextView does not wrap its text, unless
                // we explicitly set the width with setMaxWidth (even if the
                // widget's size is actually smaller than the text). This seems
                // to be a bug in android.
                int widthPixels = std::lround(adjustedBounds.width * getUIScaleFactor());

                _jTextView.setMaxWidth(widthPixels);
                _currWidthPixels = widthPixels;

                return adjustedBounds;
            }

            Size calcPreferredSize(const Size &availableSpace = Size::none()) const override
            {
                // we must unset the fixed width we set in the last setSize
                // call, otherwise it will influence the size we measure here.

                int maxWidthPixels = 0x7fffffff;

                // if we have a preferred width hint then we use that. The
                // text view core uses the "max width" to do its wrapping.
                // Same if we have a maximum width
                std::shared_ptr<const View> view = getOuterViewIfStillAttached();
                if (view != nullptr) {
                    Size limit = view->preferredSizeHint;
                    limit.applyMaximum(view->preferredSizeMaximum);

                    if (std::isfinite(limit.width))
                        maxWidthPixels = std::lround(limit.width * getUIScaleFactor());
                }

                _jTextView.setMaxWidth(maxWidthPixels);

                Size resultSize;

                try {
                    resultSize = ViewCore::calcPreferredSize(availableSpace);
                }
                catch (...) {
                    try {
                        _jTextView.setMaxWidth(_currWidthPixels);
                    }
                    catch (...) {
                        // ignore.
                    }

                    throw;
                }

                _jTextView.setMaxWidth(_currWidthPixels);

                return resultSize;
            }

          protected:
            bool canAdjustWidthToAvailableSpace() const override { return true; }

            double getFontSizeDips() const override
            {
                // the text size is in pixels
                return _jTextView.getTextSize() / getUIScaleFactor();
            }

          private:
            mutable JTextView _jTextView;

            int _currWidthPixels = 0x7fffffff;
        };
    }
}
