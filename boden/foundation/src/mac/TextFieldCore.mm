#include <bdn/init.h>

#import <Cocoa/Cocoa.h>

#import <bdn/mac/TextFieldCore.hh>

@interface BdnTextFieldDelegate : NSObject<NSTextFieldDelegate>

@property (nonatomic, assign) bdn::WeakP<bdn::mac::TextFieldCore> core;

- (id)initWithCore:(bdn::WeakP<bdn::mac::TextFieldCore>)core;
- (void)controlTextDidChange:(NSNotification*)obj;
- (void)submitted;

@end

@implementation BdnTextFieldDelegate

- (id)initWithCore:(bdn::WeakP<bdn::mac::TextFieldCore>)core
{
    if ((self = [super init])) {
        self.core = core;
        
        bdn::P<bdn::mac::TextFieldCore> textFieldCore = self.core.toStrong();
        NSTextField* textField = (NSTextField*)textFieldCore->getNSView();
        textField.delegate = self;
        textField.target = self;
        textField.action = @selector(submitted);
        
        return self;
    }
    
    return nil;
}

- (void)controlTextDidChange:(NSNotification *)obj
{
    bdn::P<bdn::mac::TextFieldCore> textFieldCore = self.core.toStrong();
    bdn::P<bdn::TextField> outerTextField = bdn::cast<bdn::TextField>(textFieldCore->getOuterViewIfStillAttached());
    if (outerTextField) {
        NSTextView* textView = [obj.userInfo objectForKey:@"NSFieldEditor"];
        outerTextField->setText( bdn::mac::macStringToString(textView.textStorage.string) );
    }
}

- (void)submitted
{
    bdn::P<bdn::mac::TextFieldCore> textFieldCore = self.core.toStrong();
    bdn::P<bdn::TextField> outerTextField = bdn::cast<bdn::TextField>(textFieldCore->getOuterViewIfStillAttached());
    if (outerTextField) {
        outerTextField->submit();
    }
}

@end

namespace bdn
{
namespace mac
{

TextFieldCore::TextFieldCore(TextField* pOuterTextField)
    : ChildViewCore(pOuterTextField, _createNsTextView(pOuterTextField)),
      _delegate([[BdnTextFieldDelegate alloc] initWithCore:this])
{
    setText(pOuterTextField->text());
}
    
TextFieldCore::~TextFieldCore()
{
    _delegate = nil;
}

}
}
