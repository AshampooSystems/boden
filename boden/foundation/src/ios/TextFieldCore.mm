#include <bdn/init.h>

#import <UIKit/UIKit.h>

#import <bdn/ios/TextFieldCore.hh>

@interface BdnTextFieldDelegate : NSObject <UITextFieldDelegate>

@property(nonatomic, assign) bdn::WeakP<bdn::ios::TextFieldCore> core;

- (id)initWithCore:(bdn::WeakP<bdn::ios::TextFieldCore>)core;
- (void)textFieldDidChange:(UITextField *)textField;
- (BOOL)textFieldShouldReturn:(UITextField *)textField;

@end

@implementation BdnTextFieldDelegate

- (id)initWithCore:(bdn::WeakP<bdn::ios::TextFieldCore>)core
{
    if ((self = [super init])) {
        self.core = core;

        bdn::P<bdn::ios::TextFieldCore> textFieldCore = self.core.toStrong();
        UITextField *textField = (UITextField *)textFieldCore->getUIView();
        textField.delegate = self;

        [[NSNotificationCenter defaultCenter]
            addObserver:self
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
    bdn::P<bdn::ios::TextFieldCore> textFieldCore = self.core.toStrong();
    bdn::P<bdn::TextField> outerTextField =
        bdn::cast<bdn::TextField>(textFieldCore->getOuterViewIfStillAttached());
    if (outerTextField) {
        outerTextField->setText(bdn::ios::iosStringToString(
            ((UITextField *)notification.object).text));
    }
}

- (BOOL)textFieldShouldReturn:(UITextField *)textField
{
    bdn::P<bdn::ios::TextFieldCore> textFieldCore = self.core.toStrong();
    bdn::P<bdn::TextField> outerTextField =
        bdn::cast<bdn::TextField>(textFieldCore->getOuterViewIfStillAttached());
    if (outerTextField) {
        outerTextField->submit();
    }

    return YES;
}

@end

namespace bdn
{
    namespace ios
    {

        TextFieldCore::TextFieldCore(TextField *pOuterTextField)
            : ViewCore(pOuterTextField, _createUITextField(pOuterTextField)),
              _delegate([[BdnTextFieldDelegate alloc] initWithCore:this])
        {
            setText(pOuterTextField->text());
        }

        TextFieldCore::~TextFieldCore() { _delegate = nil; }
    }
}
