#ifndef BDN_IOS_TextFieldCore_HH_
#define BDN_IOS_TextFieldCore_HH_

#include <bdn/TextField.h>
#include <bdn/ios/ViewCore.hh>
#include <bdn/ios/util.hh>

@class BdnTextFieldDelegate;

namespace bdn
{
namespace ios
{

class TextFieldCore : public ViewCore, virtual public ITextFieldCore
{
private:
    static UITextField* _createUITextField(TextField* pOuterTextField)
    {
        UITextField* textField = [[UITextField alloc] initWithFrame:CGRectMake(0, 0, 0, 0)];
	    textField.backgroundColor = [UIColor clearColor];
	    textField.layer.borderColor = [[UIColor colorWithRed:0.8 green:0.8 blue:0.8 alpha:1.0] CGColor];
	    textField.layer.borderWidth = 1;
	    textField.layer.cornerRadius = 5;
	    textField.font = [UIFont systemFontOfSize:15];
	    textField.borderStyle = UITextBorderStyleRoundedRect;
	    textField.clearButtonMode = UITextFieldViewModeWhileEditing;
	    textField.keyboardType = UIKeyboardTypeDefault;
	    textField.autocorrectionType = UITextAutocorrectionTypeNo;
	    textField.contentVerticalAlignment = UIControlContentVerticalAlignmentCenter;
        return textField;
    }

public:
    TextFieldCore(TextField* pOuterTextField);
    ~TextFieldCore();

    void setText(const String& text) override
    {
        UITextField* textField = (UITextField*)getUIView();
        if (iosStringToString(textField.text) != text) {
            textField.text = stringToIosString(text);
        }
    }    

private:
    BdnTextFieldDelegate* _delegate;

};

}
}


#endif
