#import <UIKit/UIKit.h>

#import <bdn/foundationkit/stringUtil.hh>
#import <bdn/ios/TextFieldCore.hh>
#include <bdn/log.h>

@interface BdnTextFieldDelegate : NSObject <UITextFieldDelegate>

@property(nonatomic, assign) std::weak_ptr<bdn::ui::TextField::Core> core;

- (id)initWithTextField:(UITextField *)textField core:(std::shared_ptr<bdn::ui::TextField::Core>)core;
- (void)textFieldDidChange:(UITextField *)textField;
- (BOOL)textFieldShouldReturn:(UITextField *)textField;

@end

@implementation BdnTextFieldDelegate

- (id)initWithTextField:(UITextField *)textField core:(std::shared_ptr<bdn::ui::TextField::Core>)core
{
    if ((self = [super init]) != nullptr) {
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

    [textField resignFirstResponder]; // Close software keyboard

    return YES;
}
@end

@interface BodenUITextField : UITextField <UIViewWithFrameNotification>
@property(nonatomic, assign) std::weak_ptr<bdn::ui::ios::ViewCore> viewCore;
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

namespace bdn::ui::detail
{
    CORE_REGISTER(TextField, bdn::ui::ios::TextFieldCore, TextField)
}

namespace bdn::ui::ios
{
    BodenUITextField *_createUITextField()
    {
        BodenUITextField *textField = [[BodenUITextField alloc] initWithFrame:CGRectMake(0, 0, 0, 0)];
        textField.borderStyle = UITextBorderStyleRoundedRect;
        textField.clearButtonMode = UITextFieldViewModeWhileEditing;
        textField.keyboardType = UIKeyboardTypeDefault;
        textField.returnKeyType = UIReturnKeyDone;

        return textField;
    }

    TextFieldCore::TextFieldCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory)
        : ViewCore(viewCoreFactory, _createUITextField())
    {}

    void TextFieldCore::init()
    {
        ViewCore::init();
        _delegate = [[BdnTextFieldDelegate alloc] initWithTextField:(UITextField *)uiView()
                                                               core:shared_from_this<TextFieldCore>()];

        text.onChange() += [=](auto &property) {
            UITextField *textField = (UITextField *)uiView();
            if (fk::nsStringToString(textField.text) != text.get()) {
                textField.text = fk::stringToNSString(text);
            }
        };

        font.onChange() += [this](auto &property) { setFont(property.get()); };

        autocorrectionType.onChange() += [this](auto &property) { setAutocorrectionType(property.get()); };
    }

    TextFieldCore::~TextFieldCore() { _delegate = nil; }

    float TextFieldCore::calculateBaseline(Size forSize) const
    {
        UITextField *textField = (UITextField *)uiView();

        auto bounds = CGRectMake(0, 0, forSize.width, forSize.height);

        auto textBox = [textField textRectForBounds:bounds];
        auto baseline = textBox.origin.y + textField.font.ascender;

        if (textField.contentVerticalAlignment == UIControlContentVerticalAlignmentCenter) {
            auto offset = ((textBox.size.height - textBox.origin.y) / 2.0) - (textField.font.lineHeight / 2.0);
            baseline += offset;
        }
        if (textField.contentVerticalAlignment == UIControlContentVerticalAlignmentBottom) {
            baseline = textBox.size.height + textField.font.descender;
        }

        return baseline;
    }

    void TextFieldCore::setFont(const Font &font)
    {
        UITextField *textField = (UITextField *)uiView();

        static UIFontDescriptor *defaultFontDescriptor = textField.font.fontDescriptor;
        static float defaultSize = textField.font.pointSize;

        float size = defaultSize;

        switch (font.size.type) {
        case Font::Size::Type::Inherit:
        case Font::Size::Type::Medium:
            break;
        case Font::Size::Type::Small:
            size *= 0.75;
            break;
        case Font::Size::Type::XSmall:
            size *= 0.5;
            break;
        case Font::Size::Type::XXSmall:
            size *= 0.25;
            break;
        case Font::Size::Type::Large:
            size *= 1.25;
            break;
        case Font::Size::Type::XLarge:
            size *= 1.5;
            break;
        case Font::Size::Type::XXLarge:
            size *= 1.75;
            break;
        case Font::Size::Type::Percent:
            size *= font.size.value;
            break;
        case Font::Size::Type::Points:
        case Font::Size::Type::Pixels:
            size = font.size.value;
            break;
        }

        UIFontDescriptor *descriptor = nullptr;

        if (!font.family.empty()) {
            descriptor = [[UIFontDescriptor alloc] init];
            descriptor = [descriptor fontDescriptorWithFamily:fk::stringToNSString(font.family)];
        } else {
            descriptor = [defaultFontDescriptor copy];
        }

        descriptor = [descriptor fontDescriptorWithSize:size];

        if (font.style == Font::Style::Italic) {
            descriptor = [descriptor fontDescriptorWithSymbolicTraits:UIFontDescriptorTraitItalic];
        }

        if (font.weight != Font::Weight::Inherit && font.weight != Font::Weight::Normal) {
            if (font.weight == Font::Weight::Bold) {
                descriptor = [descriptor fontDescriptorWithSymbolicTraits:UIFontDescriptorTraitBold];
            } else {
                std::array<UIFontWeight, 9> weights = {UIFontWeightUltraLight, UIFontWeightThin,   UIFontWeightLight,
                                                       UIFontWeightRegular,    UIFontWeightMedium, UIFontWeightSemibold,
                                                       UIFontWeightBold,       UIFontWeightHeavy,  UIFontWeightBlack};

                UIFontWeight w = weights[(font.weight / 1000.0) * 8];

                descriptor = [descriptor fontDescriptorByAddingAttributes:@{
                    UIFontDescriptorTraitsAttribute : @{UIFontWeightTrait : @(w)}
                }];
            }
        }

        textField.font = [UIFont fontWithDescriptor:descriptor size:size];

        this->markDirty();
    }

    void TextFieldCore::setAutocorrectionType(AutocorrectionType autocorrectionType)
    {
        UITextField *textField = (UITextField *)uiView();

        switch (autocorrectionType) {
        case AutocorrectionType::Default:
            textField.autocorrectionType = UITextAutocorrectionTypeDefault;
            break;
        case AutocorrectionType::No:
            textField.autocorrectionType = UITextAutocorrectionTypeNo;
            break;
        case AutocorrectionType::Yes:
            textField.autocorrectionType = UITextAutocorrectionTypeYes;
            break;
        }
    }
}
