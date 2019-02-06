#pragma once

#include <bdn/TextField.h>
#include <bdn/ITextFieldCore.h>
#include <bdn/mac/ChildViewCore.hh>
#include <bdn/mac/util.hh>

@class BdnTextFieldDelegate;

namespace bdn
{
    namespace mac
    {

        class TextFieldCore : public ChildViewCore, virtual public ITextFieldCore
        {
          private:
            static NSTextField *_createNsTextView(std::shared_ptr<TextField> outerTextField)
            {
                NSTextField *textField = [[NSTextField alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
                textField.allowsEditingTextAttributes = NO; // plain textfield, no attribution/formatting
                textField.cell.wraps = NO;                  // no word wrapping
                textField.cell.scrollable = YES;            // but scroll horizontally instead
                return textField;
            }

          public:
            TextFieldCore(std::shared_ptr<TextField> outerTextField);
            ~TextFieldCore();

            void setText(const String &text) override
            {
                NSTextField *textField = (NSTextField *)getNSView();
                if (nsStringToString(textField.stringValue) != text) {
                    textField.stringValue = stringToNSString(text);
                }
            }

          private:
            BdnTextFieldDelegate *_delegate;
        };
    }
}
