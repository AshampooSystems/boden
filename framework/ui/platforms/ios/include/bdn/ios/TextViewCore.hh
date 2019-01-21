#pragma once

#include <bdn/ITextViewCore.h>
#include <bdn/ClickEvent.h>
#include <bdn/TextView.h>

#import <bdn/ios/ViewCore.hh>

namespace bdn
{
    namespace ios
    {

        class TextViewCore : public ViewCore, virtual public ITextViewCore
        {
          private:
            static UILabel *_createUILabel(std::shared_ptr<TextView> outerTextView)
            {
                UILabel *label = [[UILabel alloc] initWithFrame:CGRectMake(0, 0, 0, 0)];
                label.numberOfLines = 0;
                return label;
            }

          public:
            TextViewCore(std::shared_ptr<TextView> outerTextView)
                : ViewCore(outerTextView, _createUILabel(outerTextView))
            {
                _uiLabel = (UILabel *)getUIView();
                setText(outerTextView->text);
            }

            UILabel *getUILabel() { return _uiLabel; }

            void setText(const String &text) override { _uiLabel.text = stringToIosString(text); }

            Size calcPreferredSize(const Size &availableSpace = Size::none()) const override
            {
                // we want the preferred width hint to act as the wrap width
                // where the view wraps its text. To achieve that we incorporate
                // it into the available width value.
                Size availableSpaceToUse(availableSpace);
                std::shared_ptr<const View> view = getOuterViewIfStillAttached();
                if (view != nullptr) {
                    Size hint = view->preferredSizeHint;

                    // ignore the height hint - the view cannot change its width
                    // to match a certain height, only the other way round.
                    hint.height = Size::componentNone();

                    availableSpaceToUse.applyMaximum(hint);
                }

                // now pass that to the base class implementation
                return ViewCore::calcPreferredSize(availableSpaceToUse);
            }

          protected:
            double getFontSize() const override { return _uiLabel.font.pointSize; }

            bool canAdjustToAvailableWidth() const override { return true; }

          private:
            UILabel *_uiLabel;
        };
    }
}
