#include <bdn/init.h>
#include <bdn/ios/CheckboxCore.hh>

#define BDN_IOS_CHECKBOX_LABEL_MARGIN 5

@interface BdnIosCheckboxClickManager : NSObject

@property bdn::ios::CheckboxCore *core;

@end

@implementation BdnIosCheckboxClickManager

- (void)setCheckboxCore:(bdn::ios::CheckboxCore *)core { _core = core; }

- (void)clicked { _core->_clicked(); }

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

        CheckboxCore::CheckboxCore(Checkbox *outerCheckbox) : ToggleCoreBase(outerCheckbox, _createCheckboxComposite())
        {
            _composite = (BdnIosCheckboxComposite *)getUIView();

            BdnIosCheckboxComposite *checkboxComposite = (BdnIosCheckboxComposite *)_composite;
            [checkboxComposite.checkbox addTarget:_clickManager
                                           action:@selector(clicked)
                                 forControlEvents:UIControlEventTouchUpInside];
            [checkboxComposite addTarget:_clickManager
                                  action:@selector(clicked)
                        forControlEvents:UIControlEventTouchUpInside];

            // Set initial state
            setLabel(outerCheckbox->label());
            setState(outerCheckbox->state());
        }

        CheckboxCore::~CheckboxCore()
        {
            BdnIosCheckboxComposite *checkboxComposite = (BdnIosCheckboxComposite *)_composite;
            [checkboxComposite.checkbox removeTarget:_clickManager
                                              action:nil
                                    forControlEvents:UIControlEventTouchUpInside];
            [checkboxComposite removeTarget:_clickManager action:nil forControlEvents:UIControlEventTouchUpInside];
        }

        void CheckboxCore::layout()
        {
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
        }

        void CheckboxCore::_clicked()
        {
            P<View> view = getOuterViewIfStillAttached();
            if (view != nullptr) {
                ClickEvent evt(view);

                // Observing the UISwitch state via KVO does not work when
                // the switch state is changed via user interaction. KVO
                // works though when state is set programatically, which
                // unfortunately is useless in the case that a user changes
                // the switch state. This means we have to stick to the
                // click event to propagate the state change to the framework.
                //
                // We guarantee that the on property will be set before
                // a notification is posted to onClick.
                cast<Checkbox>(view)->setState(((BdnIosCheckboxComposite *)_composite).checkbox.checkboxState);
                cast<Checkbox>(view)->onClick().notify(evt);
            }
        }
    }
}
