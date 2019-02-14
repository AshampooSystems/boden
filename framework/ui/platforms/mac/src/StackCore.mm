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

namespace bdn
{
    namespace mac
    {

        NSView *createStackView(std::shared_ptr<Stack> outerStack)
        {
            NSView *root = [[NSView alloc] init];

            return root;
        }

        StackCore::StackCore(std::shared_ptr<Stack> outerStack) : ChildViewCore(outerStack, createStackView(outerStack))
        {
            _navigationBar = [[NSView alloc] init];
            [getNSView() addSubview:_navigationBar];

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

            _contentView = [[NSView alloc] init];
            [getNSView() addSubview:_contentView];
        }

        void StackCore::pushView(std::shared_ptr<bdn::View> view, bdn::String title) { updateCurrentView(); }

        void StackCore::popView() { updateCurrentView(); }

        void StackCore::layout()
        {
            ChildViewCore::layout();

            Size outerSize{getNSView().frame.size.width, getNSView().frame.size.height};

            _navigationBar.frame = NSMakeRect(0, outerSize.height - 50, outerSize.width, 50);
            [_navigationBar translateOriginToPoint:NSMakePoint(0, 0)];
            //_backButton.frame = NSMakeRect(0, 0, outerSize.width / 2, 50);

            NSSize titleSize = _title.intrinsicContentSize;
            NSSize buttonSize = _backButton.intrinsicContentSize;
            NSSize navBarSize = _navigationBar.frame.size;

            _backButton.frame =
                NSMakeRect(5, navBarSize.height / 2 - buttonSize.height / 2, buttonSize.width, buttonSize.height);

            _title.frame = NSMakeRect(navBarSize.width / 2 - titleSize.width / 2,
                                      navBarSize.height / 2 - titleSize.height / 2, titleSize.width, titleSize.height);

            _contentView.frame = NSMakeRect(0, 0, outerSize.width, outerSize.height - _navigationBar.frame.size.height);

            auto outerStack = getStack();
            if (!outerStack->stack().empty()) {
                Rect rChild = outerStack->stack().back().view->adjustAndSetBounds(
                    Rect{0, 0, _contentView.frame.size.width, _contentView.frame.size.height});
            }
        }

        Size StackCore::calcPreferredSize(const Size &availableSpace) const
        {
            int width = _title.intrinsicContentSize.width + _backButton.intrinsicContentSize.width * 2 + 10;
            int height = 200;

            auto outerStack = getStack();
            if (!outerStack->stack().empty()) {
                Size childPreferredSize = outerStack->stack().back().view->calcPreferredSize(availableSpace);
                height = 50 + childPreferredSize.height;
                width = std::max((int)childPreferredSize.width, width);
            }

            return Size(width, height);
        }

        void StackCore::updateCurrentView()
        {
            auto outerStack = getStack();

            _backButton.hidden = outerStack->stack().size() < 2;

            for (id oldViewObject in _contentView.subviews) {
                NSView *oldView = (NSView *)oldViewObject;
                [oldView removeFromSuperview];
            }

            if (_currentView) {
                _currentView->_setParentView(nullptr);
            }

            if (outerStack->stack().empty()) {
                _title.stringValue = @"--";
            } else {
                _title.stringValue = stringToNSString(outerStack->stack().back().title);

                auto newView = outerStack->stack().back().view;

                newView->_setParentView(getOuterViewIfStillAttached());

                _currentView = newView;

                if (auto childCore = std::dynamic_pointer_cast<ChildViewCore>(newView->getViewCore())) {
                    if (auto nsView = childCore->getNSView()) {
                        [_contentView addSubview:nsView];
                    }
                }
            }
        }

        std::shared_ptr<Stack> StackCore::getStack() const
        {
            return std::static_pointer_cast<Stack>(getOuterViewIfStillAttached());
        }
    }
}
