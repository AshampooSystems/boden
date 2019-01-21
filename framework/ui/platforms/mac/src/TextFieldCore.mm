

#import <Cocoa/Cocoa.h>

#import <bdn/mac/TextFieldCore.hh>

@interface BdnTextFieldDelegate : NSObject <NSTextFieldDelegate>

@property(nonatomic, assign) std::weak_ptr<bdn::mac::TextFieldCore> core;

- (id)initWithCore:(std::weak_ptr<bdn::mac::TextFieldCore>)core;
- (void)controlTextDidChange:(NSNotification *)obj;
- (void)submitted;

@end

@implementation BdnTextFieldDelegate

- (id)initWithCore:(std::weak_ptr<bdn::mac::TextFieldCore>)core
{
    if ((self = [super init])) {
        self.core = core;

        std::shared_ptr<bdn::mac::TextFieldCore> textFieldCore = self.core.lock();
        NSTextField *textField = (NSTextField *)textFieldCore->getNSView();
        textField.delegate = self;
        textField.target = self;
        textField.action = @selector(submitted);

        return self;
    }

    return nil;
}

- (void)controlTextDidChange:(NSNotification *)obj
{
    std::shared_ptr<bdn::mac::TextFieldCore> textFieldCore = self.core.lock();
    std::shared_ptr<bdn::TextField> outerTextField =
        std::dynamic_pointer_cast<bdn::TextField>(textFieldCore->getOuterViewIfStillAttached());
    if (outerTextField) {
        NSTextView *textView = [obj.userInfo objectForKey:@"NSFieldEditor"];
        outerTextField->text = bdn::mac::macStringToString(textView.textStorage.string);
    }
}

- (void)submitted
{
    std::shared_ptr<bdn::mac::TextFieldCore> textFieldCore = self.core.lock();
    std::shared_ptr<bdn::TextField> outerTextField =
        std::dynamic_pointer_cast<bdn::TextField>(textFieldCore->getOuterViewIfStillAttached());
    if (outerTextField) {
        outerTextField->submit();
    }
}

@end

namespace bdn
{
    namespace mac
    {

        TextFieldCore::TextFieldCore(std::shared_ptr<TextField> outerTextField)
            : ChildViewCore(outerTextField, _createNsTextView(outerTextField))

        {}

        void TextFieldCore::init(std::shared_ptr<TextField> outerTextField)
        {
            _delegate = [[BdnTextFieldDelegate alloc]
                initWithCore:std::dynamic_pointer_cast<TextFieldCore>(shared_from_this())];

            setText(outerTextField->text);
        }

        TextFieldCore::~TextFieldCore() { _delegate = nil; }
    }
}
