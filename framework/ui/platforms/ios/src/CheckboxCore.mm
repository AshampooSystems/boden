
#include <bdn/ios/CheckboxCore.hh>

#define BDN_IOS_CHECKBOX_LABEL_MARGIN 5

@interface BdnIosCheckboxClickManager : NSObject

@property(nonatomic, assign) std::weak_ptr<bdn::Checkbox> outer;
@property(nonatomic, weak) BdnIosCheckboxComposite *composite;

@end

@implementation BdnIosCheckboxClickManager

- (void)clicked
{
    if (auto outer = self.outer.lock()) {
        // Make sure outer state is updated before posting click event
        if (self.composite) {
            outer->state = self.composite.checkbox.checkboxState;
        }

        bdn::ClickEvent clickEvent(outer);
        outer->onClick().notify(clickEvent);
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
    if (_viewCore) {
        _viewCore->frameChanged();
    }
}
@end

namespace bdn
{
    namespace ios
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

        CheckboxCore::CheckboxCore(std::shared_ptr<Checkbox> outer) : ViewCore(outer, _createCheckboxComposite())
        {
            _composite = (BdnIosCheckboxComposite *)getUIView();

            _clickManager = [[BdnIosCheckboxClickManager alloc] init];
            _clickManager.outer = outer;
            _clickManager.composite = _composite;

            BdnIosCheckboxComposite *checkboxComposite = (BdnIosCheckboxComposite *)_composite;
            [checkboxComposite.checkbox addTarget:_clickManager
                                           action:@selector(clicked)
                                 forControlEvents:UIControlEventTouchUpInside];
            [checkboxComposite addTarget:_clickManager
                                  action:@selector(clicked)
                        forControlEvents:UIControlEventTouchUpInside];

            geometry.onChange() += [=](auto va) {
                CGRect compositeBounds = _composite.bounds;
                CGRect checkboxBounds = ((BdnIosCheckboxComposite *)_composite).checkbox.bounds;
                CGRect labelBounds = _composite.uiLabel.bounds;

                // Center checkbox vertically, left align horizontally in composite
                CGRect checkboxFrame = CGRectMake(0, compositeBounds.size.height / 2. - checkboxBounds.size.height / 2.,
                                                  checkboxBounds.size.width, checkboxBounds.size.height);
                ((BdnIosCheckboxComposite *)_composite).checkbox.frame = checkboxFrame;

                // Center label vertically, align next to checkbox horizontally in
                // composite
                CGRect labelFrame = CGRectMake(checkboxBounds.size.width + BDN_IOS_CHECKBOX_LABEL_MARGIN,
                                               compositeBounds.size.height / 2. - labelBounds.size.height / 2.,
                                               labelBounds.size.width, labelBounds.size.height);
                _composite.uiLabel.frame = labelFrame;
            };

            // Set initial state
            setLabel(outer->label);
            setState(outer->state);
        }

        CheckboxCore::~CheckboxCore()
        {
            BdnIosCheckboxComposite *checkboxComposite = (BdnIosCheckboxComposite *)_composite;
            [checkboxComposite.checkbox removeTarget:_clickManager
                                              action:nil
                                    forControlEvents:UIControlEventTouchUpInside];
            [checkboxComposite removeTarget:_clickManager action:nil forControlEvents:UIControlEventTouchUpInside];
        }

        void CheckboxCore::setState(const TriState &state)
        {
            ((BdnIosCheckboxComposite *)_composite).checkbox.checkboxState = state;
        }

        void CheckboxCore::setLabel(const String &label)
        {
            _composite.uiLabel.text = stringToNSString(label);
            [_composite.uiLabel sizeToFit];
        }
    }
}
