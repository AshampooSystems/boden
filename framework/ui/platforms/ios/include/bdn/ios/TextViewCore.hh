#pragma once

#include <bdn/ClickEvent.h>
#include <bdn/ITextViewCore.h>
#include <bdn/TextView.h>

#import <bdn/ios/ViewCore.hh>

@interface BodenUILabel : UILabel <UIViewWithFrameNotification>
@property(nonatomic, assign) bdn::ios::ViewCore *viewCore;
@end

@implementation BodenUILabel
- (void)setFrame:(CGRect)frame
{
    [super setFrame:frame];
    if (_viewCore) {
        _viewCore->frameChanged();
    }
}
@end

namespace bdn
{
    namespace ios
    {

        class TextViewCore : public ViewCore, virtual public ITextViewCore
        {
          private:
            static BodenUILabel *_createUILabel(std::shared_ptr<TextView> outerTextView)
            {
                BodenUILabel *label = [[BodenUILabel alloc] initWithFrame:CGRectMake(0, 0, 0, 0)];
                label.numberOfLines = 0;
                return label;
            }

          public:
            TextViewCore(std::shared_ptr<TextView> outerTextView)
                : ViewCore(outerTextView, _createUILabel(outerTextView))
            {
                _uiLabel = (UILabel *)getUIView();
                setText(outerTextView->text);
                setWrap(outerTextView->wrap);
            }

            UILabel *getUILabel() { return _uiLabel; }

            void setText(const String &text) override { _uiLabel.text = stringToNSString(text); }
            virtual void setWrap(const bool &wrap) override { _uiLabel.numberOfLines = wrap ? 0 : 1; }

          protected:
            double getFontSize() const override { return _uiLabel.font.pointSize; }

            bool canAdjustToAvailableWidth() const override { return true; }

          private:
            UILabel *_uiLabel;
        };
    }
}
