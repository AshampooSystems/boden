#pragma once

#include <bdn/ITextViewCore.h>
#include <bdn/TextView.h>
#include <bdn/android/ViewCore.h>
#include <bdn/android/wrapper/RStyle.h>
#include <bdn/android/wrapper/TextView.h>

#include <limits>

namespace bdn::android
{

    class TextViewCore : public ViewCore, virtual public ITextViewCore
    {
      public:
        TextViewCore(std::shared_ptr<TextView> outerTextView)
            : ViewCore(outerTextView, createAndroidViewClass<wrapper::TextView>(outerTextView)),
              _jTextView(getJViewAS<wrapper::TextView>())
        {
            setText(outerTextView->text);
            setWrap(outerTextView->wrap.get());

            geometry.onChange() += [=](auto va) {
                int widthPixels = va->get().width * getUIScaleFactor();

                if (_wrap) {
                    _jTextView.setMaxWidth(widthPixels);
                }
            };
        }

        wrapper::TextView &getJTextView() { return _jTextView; }

        void setText(const String &text) override
        {
            // Remove '\r' as android treats them as a space
            String textToSet = text;
            textToSet.erase(
                std::remove_if(textToSet.begin(), textToSet.end(), [](unsigned char x) { return x == '\r'; }),
                textToSet.end());

            _jTextView.setText(textToSet);
        }

        virtual void setWrap(const bool &wrap) override
        {
            _jTextView.setMaxLines(wrap ? std::numeric_limits<int>::max() : 1);
            _wrap = wrap;
        }

        Size sizeForSpace(Size availableSpace = Size::none()) const override
        {
            if (_wrap) {
                _jTextView.setMaxWidth(availableSpace.width * getUIScaleFactor());
            }

            Size result = ViewCore::sizeForSpace(availableSpace);

            if (_wrap) {
                _jTextView.setMaxWidth(geometry->width * getUIScaleFactor());
            }

            return result;
        }

      protected:
        bool canAdjustWidthToAvailableSpace() const override { return false; } // return _wrap; }

        double getFontSizeDips() const override
        {
            // the text size is in pixels
            return _jTextView.getTextSize() / getUIScaleFactor();
        }

      private:
        mutable wrapper::TextView _jTextView;

        bool _wrap = true;
    };
}
