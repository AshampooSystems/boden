#import <bdn/foundationkit/AttributedString.hh>
#include <bdn/ios/LabelCore.hh>

#include <bdn/log.h>

namespace bdn::ui::detail
{
    CORE_REGISTER(Label, bdn::ui::ios::LabelCore, Label)
}

@implementation BodenUILabel {
    bdn::ui::ios::LabelCore *_labelCore;
}

- (id)initWithCore:(bdn::ui::ios::LabelCore *)labelCore
{
    self = [super init];
    if (self) {
        _labelCore = labelCore;
        UITapGestureRecognizer *tap =
            [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(linkTouched:)];

        [self addGestureRecognizer:tap];
    }

    return self;
}

- (UIView *)hitTest:(CGPoint)point withEvent:(UIEvent *)event
{
    if ([self linkAt:point]) {
        return self;
    }

    return nil;
}

- (id)linkAt:(CGPoint)pos
{
    if (auto aTxt = self.attributedText) {
        pos.x -= self.textContainerInset.left;
        pos.y -= self.textContainerInset.top;

        NSUInteger characterIndex;
        characterIndex = [self.layoutManager characterIndexForPoint:pos
                                                    inTextContainer:self.textContainer
                           fractionOfDistanceBetweenInsertionPoints:NULL];

        return [aTxt attribute:NSLinkAttributeName atIndex:characterIndex effectiveRange:nullptr];
    }

    return nil;
}

- (void)linkTouched:(UITapGestureRecognizer *)recognizer
{
    if (id link = [self linkAt:[recognizer locationInView:self]]) {
        NSURL *url = (NSURL *)link;

        _labelCore->_linkClickCallback.fire(bdn::fk::nsStringToString(url.absoluteString));

        bdn::logstream() << bdn::fk::nsStringToString(url.absoluteString);
    }
}

- (void)setFrame:(CGRect)frame
{
    [super setFrame:frame];
    if (auto viewCore = self.viewCore.lock()) {
        viewCore->frameChanged();
    }
}
@end

namespace bdn::ui::ios
{
    BodenUILabel *LabelCore::createUILabel(LabelCore *core)
    {
        BodenUILabel *label = [[BodenUILabel alloc] initWithCore:core];
        label.textContainer.maximumNumberOfLines = 0;
        label.font = [UIFont systemFontOfSize:[UIFont labelFontSize]];
        label.selectable = false;
        label.editable = false;

        return label;
    }

    LabelCore::LabelCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory)
        : ViewCore(viewCoreFactory, createUILabel(this))
    {
        _uiLabel = (UITextView *)uiView();

        text.onChange() += [=](auto &property) { textChanged(property.get()); };

        wrap.onChange() += [=](auto &property) {
            _uiLabel.textContainer.maximumNumberOfLines = wrap ? 0 : 1;
            markDirty();
        };
    }

    UITextView *LabelCore::getUILabel() { return _uiLabel; }

    float LabelCore::calculateBaseline(bdn::Size forSize) const
    {
        auto br =
            [_uiLabel.layoutManager boundingRectForGlyphRange:NSMakeRange(0, 1) inTextContainer:_uiLabel.textContainer];

        if (id idFont = [_uiLabel.textStorage attribute:NSFontAttributeName atIndex:0 effectiveRange:nullptr]) {
            UIFont *font = (UIFont *)idFont;

            if (id idBaselineOffset =
                    [_uiLabel.textStorage attribute:NSBaselineOffsetAttributeName atIndex:0 effectiveRange:nullptr]) {
                NSNumber *baselineOffset = (NSNumber *)idBaselineOffset;
                return br.size.height + _uiLabel.textContainerInset.top + font.descender - baselineOffset.floatValue;
            }
            return br.size.height + _uiLabel.textContainerInset.top + font.descender;
        }

        return br.size.height + _uiLabel.textContainerInset.top;
    }

    void LabelCore::textChanged(const Text &text)
    {
        std::visit(
            [uiLabel = this->_uiLabel](auto &&arg) {
                using T = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<T, String>) {
                    uiLabel.text = fk::stringToNSString(arg);
                } else if constexpr (std::is_same_v<T, std::shared_ptr<AttributedString>>) {
                    if (auto fkAttrString = std::dynamic_pointer_cast<bdn::fk::AttributedString>(arg)) {
                        uiLabel.attributedText = fkAttrString->nsAttributedString();
                    }
                }
            },
            text);

        markDirty();
        scheduleLayout();
    }
}
