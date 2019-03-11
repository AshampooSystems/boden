#pragma once

#include <bdn/TextView.h>
#include <bdn/TextViewCore.h>
#include <bdn/android/ViewCore.h>
#include <bdn/android/wrapper/RStyle.h>
#include <bdn/android/wrapper/TextView.h>

#include <limits>

namespace bdn::android
{
    class TextViewCore : public ViewCore, virtual public bdn::TextViewCore
    {
      public:
        TextViewCore(const ContextWrapper &ctxt)
            : ViewCore(createAndroidViewClass<wrapper::TextView>(ctxt)), _jTextView(getJViewAS<wrapper::TextView>())
        {
            text.onChange() += [=](auto va) {
                // Remove '\r' as android treats them as a space
                String textToSet = va->get();
                textToSet.erase(
                    std::remove_if(textToSet.begin(), textToSet.end(), [](unsigned char x) { return x == '\r'; }),
                    textToSet.end());
                _jTextView.setText(textToSet);
                scheduleLayout();
            };

            wrap.onChange() += [=](auto va) {
                _jTextView.setMaxLines(va->get() ? std::numeric_limits<int>::max() : 1);
                _wrap = va->get();
                scheduleLayout();
            };

            geometry.onChange() += [=](auto va) {
                int widthPixels = va->get().width * getUIScaleFactor();

                if (_wrap) {
                    _jTextView.setMaxWidth(widthPixels);
                }
            };
        }

        wrapper::TextView &getJTextView() { return _jTextView; }

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
