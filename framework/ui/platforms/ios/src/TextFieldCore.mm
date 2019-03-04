

#import <UIKit/UIKit.h>

#import <bdn/ios/TextFieldCore.hh>

@interface BdnTextFieldDelegate : NSObject <UITextFieldDelegate>

@property(nonatomic, assign) std::weak_ptr<bdn::TextField> outerTextField;

- (id)initWithTextField:(UITextField *)textField outerTextField:(std::shared_ptr<bdn::TextField>)outerTextField;
- (void)textFieldDidChange:(UITextField *)textField;
- (BOOL)textFieldShouldReturn:(UITextField *)textField;

@end

@implementation BdnTextFieldDelegate

- (id)initWithTextField:(UITextField *)textField outerTextField:(std::shared_ptr<bdn::TextField>)outerTextField
{
    if ((self = [super init])) {
        self.outerTextField = outerTextField;
        textField.delegate = self;

        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(textFieldDidChange:)
                                                     name:UITextFieldTextDidChangeNotification
                                                   object:textField];

        return self;
    }

    return nil;
}

- (void)dealloc { [[NSNotificationCenter defaultCenter] removeObserver:self]; }

- (void)textFieldDidChange:(NSNotification *)notification
{
    self.outerTextField.lock()->text = (bdn::ios::nsStringToString(((UITextField *)notification.object).text));
}

- (BOOL)textFieldShouldReturn:(UITextField *)textField
{
    self.outerTextField.lock()->submit();

    // close virtual keyboard
    [textField resignFirstResponder];

    return YES;
}
@end

@interface BodenUITextField : UITextField <UIViewWithFrameNotification>
@property(nonatomic, assign) bdn::ios::ViewCore *viewCore;
@end

@implementation BodenUITextField

- (void)setFrame:(CGRect)frame
{
    [super setFrame:frame];
    if (_viewCore) {
        _viewCore->frameChanged();
    }
}

@end

namespace bdn::ios
{
    BodenUITextField *_createUITextField(std::shared_ptr<TextField> outerTextField)
    {
        BodenUITextField *textField = [[BodenUITextField alloc] initWithFrame:CGRectMake(0, 0, 0, 0)];
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
        textField.returnKeyType = UIReturnKeyDone;

        return textField;
    }

    TextFieldCore::TextFieldCore(std::shared_ptr<TextField> outerTextField)
        : ViewCore(outerTextField, _createUITextField(outerTextField))
    {
        setText(outerTextField->text);
        _delegate =
            [[BdnTextFieldDelegate alloc] initWithTextField:(UITextField *)uiView() outerTextField:outerTextField];
    }

    TextFieldCore::~TextFieldCore() { _delegate = nil; }

    void TextFieldCore::setText(const String &text)
    {
        UITextField *textField = (UITextField *)uiView();
        if (nsStringToString(textField.text) != text) {
            textField.text = stringToNSString(text);
        }
    }
}
