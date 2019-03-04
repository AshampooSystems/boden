#import <bdn/mac/StackCore.hh>
#include <iostream>

@interface BdnBackButtonClickHandler : NSObject

@property(nonatomic, assign) std::weak_ptr<bdn::Stack> outer;

@end

@implementation BdnBackButtonClickHandler

- (void)clicked
{
    if (auto outer = self.outer.lock()) {
        outer->popView();
    }
}

@end

namespace bdn::mac
{
    NSView *createStackView(std::shared_ptr<Stack> outerStack)
    {
        NSView *root = [[NSView alloc] init];

        return root;
    }

    StackCore::StackCore(std::shared_ptr<Stack> outerStack) : ChildViewCore(outerStack, createStackView(outerStack))
    {
        _navigationBar = [[NSView alloc] init];
        [nsView() addSubview:_navigationBar];

        _backButton = [[NSButton alloc] init];
        [_backButton setTitle:@"Back"];

        _backButtonClickHandler = [[BdnBackButtonClickHandler alloc] init];
        _backButtonClickHandler.outer = outerStack;
        [_backButton setTarget:_backButtonClickHandler];
        [_backButton setAction:@selector(clicked)];

        _title = [[NSTextField alloc] init];
        _title.stringValue = @"Hallo Welt nicht wahr?";
        _title.editable = NO;

        [_navigationBar addSubview:_title];
        [_navigationBar addSubview:_backButton];

        geometry.onChange() += [=](auto va) { this->reLayout(); };
    }

    void StackCore::pushView(std::shared_ptr<bdn::View> view, bdn::String title) { updateCurrentView(); }

    void StackCore::popView() { updateCurrentView(); }

    std::list<std::shared_ptr<View>> StackCore::childViews()
    {
        if (_container) {
            return {_container};
        }
        return {};
    }

    void StackCore::updateCurrentView()
    {
        if (!_container) {
            _container = std::make_shared<FixedView>();
            _container->_setParentView(outerView());
        }

        auto outerStack = getStack();

        _backButton.hidden = outerStack->stack().size() < 2;

        _container->removeAllChildViews();

        if (_currentView) {
            _currentView->_setParentView(nullptr);
        }

        if (outerStack->stack().empty()) {
            _title.stringValue = @"--";
        } else {
            _title.stringValue = stringToNSString(outerStack->stack().back().title);

            auto newView = outerStack->stack().back().view;

            _container->addChildView(newView);

            _currentView = newView;
        }

        reLayout();
    }

    std::shared_ptr<Stack> StackCore::getStack() const { return std::static_pointer_cast<Stack>(outerView()); }

    void StackCore::reLayout()
    {
        Size outerSize{nsView().frame.size.width, nsView().frame.size.height};

        _navigationBar.frame = NSMakeRect(0, outerSize.height - 50, outerSize.width, 50);
        [_navigationBar translateOriginToPoint:NSMakePoint(0, 0)];

        NSSize titleSize = _title.intrinsicContentSize;
        NSSize buttonSize = _backButton.intrinsicContentSize;
        NSSize navBarSize = _navigationBar.frame.size;

        _backButton.frame =
            NSMakeRect(5, navBarSize.height / 2 - buttonSize.height / 2, buttonSize.width, buttonSize.height);

        _title.frame = NSMakeRect(navBarSize.width / 2 - titleSize.width / 2,
                                  navBarSize.height / 2 - titleSize.height / 2, titleSize.width, titleSize.height);

        if (_container) {
            _container->geometry = Rect{0, 0, outerSize.width, outerSize.height - _navigationBar.frame.size.height};
        }
    }
}
