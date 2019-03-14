#import <UIKit/UIKit.h>

#import <bdn/foundationkit/stringUtil.hh>
#import <bdn/ios/TextFieldCore.hh>

@interface BdnTextFieldDelegate : NSObject <UITextFieldDelegate>

@property(nonatomic, assign) std::weak_ptr<bdn::TextFieldCore> core;

- (id)initWithTextField:(UITextField *)textField core:(std::shared_ptr<bdn::TextFieldCore>)core;
- (void)textFieldDidChange:(UITextField *)textField;
- (BOOL)textFieldShouldReturn:(UITextField *)textField;

@end

@implementation BdnTextFieldDelegate

- (id)initWithTextField:(UITextField *)textField core:(std::shared_ptr<bdn::TextFieldCore>)core
{
    if ((self = [super init])) {
        self.core = core;
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
    if (auto core = self.core.lock()) {
        core->text = (bdn::fk::nsStringToString(((UITextField *)notification.object).text));
    }
}

- (BOOL)textFieldShouldReturn:(UITextField *)textField
{
    if (auto core = self.core.lock()) {
        core->submitCallback.fire();
    }

    // close virtual keyboard
    [textField resignFirstResponder];

    return YES;
}
@end

@interface BodenUITextField : UITextField <UIViewWithFrameNotification>
@property(nonatomic, assign) std::weak_ptr<bdn::ios::ViewCore> viewCore;
@end

@implementation BodenUITextField

- (void)setFrame:(CGRect)frame
{
    [super setFrame:frame];
    if (auto viewCore = self.viewCore.lock()) {
        viewCore->frameChanged();
    }
}

@end

namespace bdn::ios
{
    BodenUITextField *_createUITextField()
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

    TextFieldCore::TextFieldCore(const std::shared_ptr<bdn::UIProvider> &uiProvider)
        : ViewCore(uiProvider, _createUITextField())
    {}

    void TextFieldCore::init()
    {
        ViewCore::init();
        _delegate = [[BdnTextFieldDelegate alloc]
            initWithTextField:(UITextField *)uiView()
                         core:std::dynamic_pointer_cast<TextFieldCore>(shared_from_this())];

        text.onChange() += [=](auto va) {
            UITextField *textField = (UITextField *)uiView();
            if (fk::nsStringToString(textField.text) != text.get()) {
                textField.text = fk::stringToNSString(text);
            }
        };
    }

    TextFieldCore::~TextFieldCore() { _delegate = nil; }
}
