#import <bdn/foundationkit/stringUtil.hh>
#include <bdn/ios/CheckboxCore.hh>

#define BDN_IOS_CHECKBOX_LABEL_MARGIN 5

@interface BdnIosCheckboxClickManager : NSObject

@property(nonatomic, assign) std::weak_ptr<bdn::ios::CheckboxCore> core;
@property(nonatomic, weak) BdnIosCheckboxComposite *composite;

@end

@implementation BdnIosCheckboxClickManager

- (void)clicked
{
    if (auto core = self.core.lock()) {
        // Make sure state is up to date before posting click event
        if (self.composite != nullptr) {
            core->state = self.composite.checkbox.checkboxState;
        }

        core->handleClick();
    }
}

@end

@implementation BdnIosCheckboxComposite

// Use systemLayoutSizeFittingSize instead of intrinsicContentSize
- (CGSize)systemLayoutSizeFittingSize:(CGSize)targetSize
{
    CGFloat minMargin = BDN_IOS_CHECKBOX_LABEL_MARGIN;
    CGSize switchSize = [self.checkbox systemLayoutSizeFittingSize:targetSize];
    CGSize labelSize = [self.uiLabel systemLayoutSizeFittingSize:targetSize];
    return CGSizeMake(switchSize.width + labelSize.width + minMargin, fmax(switchSize.height, labelSize.height));
}

- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    self.checkbox.isTouching = YES;
    [self.checkbox setNeedsDisplay];
}

- (void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    // Change state and send click event only if touch ends inside the control's
    // bounds
    CGPoint touchPoint = [[touches anyObject] locationInView:self];
    if (CGRectContainsPoint(self.bounds, touchPoint)) {
        self.checkbox.isTouching = NO;

        switch (self.checkbox.checkboxState) {
        case bdn::TriState::on:
            self.checkbox.checkboxState = bdn::TriState::off;
            break;
        case bdn::TriState::mixed:
        case bdn::TriState::off:
            self.checkbox.checkboxState = bdn::TriState::on;
            break;
        }

        [self sendActionsForControlEvents:UIControlEventTouchUpInside];
    }
}

- (void)setFrame:(CGRect)frame
{
    [super setFrame:frame];

    CGRect compositeBounds = self.bounds;
    CGRect checkboxBounds = self.checkbox.bounds;
    CGRect labelBounds = self.uiLabel.bounds;

    // Center checkbox vertically, left align horizontally in composite
    CGRect checkboxFrame = CGRectMake(0, compositeBounds.size.height / 2. - checkboxBounds.size.height / 2.,
                                      checkboxBounds.size.width, checkboxBounds.size.height);
    self.checkbox.frame = checkboxFrame;

    // Center label vertically, align next to checkbox horizontally in
    // composite
    CGRect labelFrame = CGRectMake(checkboxBounds.size.width + BDN_IOS_CHECKBOX_LABEL_MARGIN,
                                   compositeBounds.size.height / 2. - labelBounds.size.height / 2.,
                                   labelBounds.size.width, labelBounds.size.height);
    self.uiLabel.frame = labelFrame;

    if (auto viewCore = self.viewCore.lock()) {
        viewCore->frameChanged();
    }
}
@end

namespace bdn::detail
{
    CORE_REGISTER(Checkbox, bdn::ios::CheckboxCore, Checkbox)
}

namespace bdn::ios
{
    BdnIosCheckboxComposite *CheckboxCore::_createCheckboxComposite()
    {
        BdnIosCheckboxComposite *switchComposite =
            [[BdnIosCheckboxComposite alloc] initWithFrame:CGRectMake(0, 0, 0, 0)];

        switchComposite.checkbox = [[BdnIosCheckbox alloc] initWithFrame:CGRectMake(0, 0, 0, 0)];
        [switchComposite addSubview:switchComposite.checkbox];

        switchComposite.uiLabel = [[UILabel alloc] initWithFrame:CGRectMake(0, 0, 0, 0)];
        [switchComposite addSubview:switchComposite.uiLabel];

        return switchComposite;
    }

    CheckboxCore::CheckboxCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory)
        : ViewCore(viewCoreFactory, _createCheckboxComposite())
    {}

    void CheckboxCore::init()
    {
        ViewCore::init();

        _composite = (BdnIosCheckboxComposite *)uiView();

        _clickManager = [[BdnIosCheckboxClickManager alloc] init];
        _clickManager.core = shared_from_this<CheckboxCore>();
        _clickManager.composite = _composite;

        auto checkboxComposite = (BdnIosCheckboxComposite *)_composite;
        [checkboxComposite.checkbox addTarget:_clickManager
                                       action:@selector(clicked)
                             forControlEvents:UIControlEventTouchUpInside];
        [checkboxComposite addTarget:_clickManager
                              action:@selector(clicked)
                    forControlEvents:UIControlEventTouchUpInside];

        state.onChange() += [=](auto va) { ((BdnIosCheckboxComposite *)_composite).checkbox.checkboxState = state; };

        label.onChange() += [=](auto va) {
            _composite.uiLabel.text = fk::stringToNSString(label);
            [_composite.uiLabel sizeToFit];
        };
    }

    CheckboxCore::~CheckboxCore()
    {
        auto checkboxComposite = (BdnIosCheckboxComposite *)_composite;
        [checkboxComposite.checkbox removeTarget:_clickManager action:nil forControlEvents:UIControlEventTouchUpInside];
        [checkboxComposite removeTarget:_clickManager action:nil forControlEvents:UIControlEventTouchUpInside];
    }

    void CheckboxCore::handleClick() { _clickCallback.fire(); }
}
