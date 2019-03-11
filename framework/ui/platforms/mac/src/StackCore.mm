#import <bdn/mac/ContainerViewCore.hh>
#import <bdn/mac/StackCore.hh>
#include <iostream>

@interface BdnBackButtonClickHandler : NSObject

@property(nonatomic, assign) std::weak_ptr<bdn::mac::StackCore> stackCore;

@end

@implementation BdnBackButtonClickHandler

- (void)clicked
{
    if (auto core = self.stackCore.lock()) {
        core->popView();
    }
}

@end

namespace bdn::mac
{
    NSView *createStackView()
    {
        NSView *root = [[NSView alloc] init];

        return root;
    }

    StackCore::StackCore() : ViewCore(createStackView()) {}

    StackCore::~StackCore() { _backButtonClickHandler.stackCore = std::weak_ptr<StackCore>(); }

    void StackCore::init()
    {
        ViewCore::init();

        auto self = std::dynamic_pointer_cast<StackCore>(shared_from_this());

        _navigationBar = [[NSView alloc] init];
        [nsView() addSubview:_navigationBar];

        _backButton = [[NSButton alloc] init];
        [_backButton setTitle:@"Back"];

        _backButtonClickHandler = [[BdnBackButtonClickHandler alloc] init];
        _backButtonClickHandler.stackCore = self;
        [_backButton setTarget:_backButtonClickHandler];
        [_backButton setAction:@selector(clicked)];

        _title = [[NSTextField alloc] init];
        _title.stringValue = @"Hallo Welt nicht wahr?";
        _title.editable = NO;

        [_navigationBar addSubview:_title];
        [_navigationBar addSubview:_backButton];

        geometry.onChange() += [self](auto va) { self->reLayout(); };
    }

    void StackCore::pushView(std::shared_ptr<bdn::View> view, bdn::String title)
    {
        _stack.push_back({view, title});
        updateCurrentView();
    }

    void StackCore::popView()
    {
        _stack.pop_back();
        updateCurrentView();
    }

    std::list<std::shared_ptr<View>> StackCore::childViews()
    {
        if (_container) {
            return {_container};
        }
        return {};
    }

    void StackCore::setLayout(std::shared_ptr<Layout> layout)
    {
        if (_container) {
            _container->offerLayout(layout);
        }
        ViewCore::setLayout(std::move(layout));
    }

    void StackCore::updateCurrentView()
    {
        if (!_container) {
            _container = std::make_shared<FixedView>(_uiProvider);
            _container->offerLayout(_layout);
            if (auto containerCore = _container->core<bdn::mac::ContainerViewCore>()) {
                addChildNSView(containerCore->nsView());
            } else {
                throw std::runtime_error("Container did not have the right core type!");
            }
        }

        _backButton.hidden = _stack.size() < 2;

        _container->removeAllChildViews();

        if (_currentView) {
            _currentView->setParentView(nullptr);
        }

        if (_stack.empty()) {
            _title.stringValue = @"--";
        } else {
            _title.stringValue = stringToNSString(_stack.back().title);

            auto newView = _stack.back().view;

            _container->addChildView(newView);

            _currentView = newView;
        }

        reLayout();
    }

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
