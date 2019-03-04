#import <Cocoa/Cocoa.h>
#import <bdn/mac/TextFieldCore.hh>

@interface BdnTextFieldDelegate : NSObject <NSTextFieldDelegate>

@property(nonatomic, assign) std::weak_ptr<bdn::TextField> outer;
@property(nonatomic, weak) NSTextField *nsTextField;

- (id)initWithOuter:(std::weak_ptr<bdn::TextField>)outer nsTextField:(NSTextField *)nsTextField;
- (void)controlTextDidChange:(NSNotification *)obj;
- (void)submitted;

@end

@implementation BdnTextFieldDelegate

- (id)initWithOuter:(std::weak_ptr<bdn::TextField>)outer nsTextField:(NSTextField *)nsTextField
{
    if ((self = [super init])) {
        self.outer = outer;
        self.nsTextField = nsTextField;

        nsTextField.delegate = self;
        nsTextField.target = self;
        nsTextField.action = @selector(submitted);

        return self;
    }

    return nil;
}

- (void)controlTextDidChange:(NSNotification *)obj
{
    if (auto outer = self.outer.lock()) {
        NSTextView *textView = [obj.userInfo objectForKey:@"NSFieldEditor"];
        outer->text = bdn::mac::nsStringToString(textView.textStorage.string);
    }
}

- (void)submitted
{
    if (auto outer = self.outer.lock()) {
        outer->submit();
    }
}

@end

namespace bdn::mac
{
    TextFieldCore::TextFieldCore(std::shared_ptr<TextField> outerTextField)
        : ChildViewCore(outerTextField, _createNsTextView(outerTextField))
    {
        _delegate = [[BdnTextFieldDelegate alloc] initWithOuter:outerTextField nsTextField:(NSTextField *)nsView()];

        setText(outerTextField->text);
    }

    TextFieldCore::~TextFieldCore() { _delegate = nil; }
}
