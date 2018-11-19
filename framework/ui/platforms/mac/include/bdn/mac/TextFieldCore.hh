#ifndef BDN_MAC_TextFieldCore_HH_
#define BDN_MAC_TextFieldCore_HH_

#include <bdn/init.h>
#include <bdn/TextField.h>
#include <bdn/ITextFieldCore.h>
#include <bdn/mac/ChildViewCore.hh>
#include <bdn/mac/util.hh>

@class BdnTextFieldDelegate;

namespace bdn
{
    namespace mac
    {

        class TextFieldCore : public ChildViewCore, BDN_IMPLEMENTS ITextFieldCore
        {
          private:
            static NSTextField *_createNsTextView(TextField *outerTextField)
            {
                NSTextField *textField = [[NSTextField alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
                textField.allowsEditingTextAttributes = NO; // plain textfield, no attribution/formatting
                textField.cell.wraps = NO;                  // no word wrapping
                textField.cell.scrollable = YES;            // but scroll horizontally instead
                return textField;
            }

          public:
            TextFieldCore(TextField *outerTextField);
            ~TextFieldCore();

            void setText(const String &text) override
            {
                NSTextField *textField = (NSTextField *)getNSView();
                if (macStringToString(textField.stringValue) != text) {
                    textField.stringValue = stringToMacString(text);
                }
            }

          private:
            BdnTextFieldDelegate *_delegate;
        };
    }
}

#endif
