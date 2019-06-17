#import <bdn/mac/ContainerViewCore.hh>
#import <bdn/mac/NavigationViewCore.hh>
#include <iostream>

@interface BdnBackButtonClickHandler : NSObject

@property(nonatomic, assign) std::weak_ptr<bdn::ui::mac::NavigationViewCore> stackCore;

@end

@implementation BdnBackButtonClickHandler

- (void)clicked
{
    if (auto core = self.stackCore.lock()) {
        core->popView();
    }
}

@end

namespace bdn::ui::detail
{
    CORE_REGISTER(NavigationView, bdn::ui::mac::NavigationViewCore, NavigationView)
}

namespace bdn::ui::mac
{
    NSView *createNavigationView()
    {
        NSView *root = [[NSView alloc] init];

        return root;
    }

    NavigationViewCore::NavigationViewCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory)
        : ViewCore(viewCoreFactory, createNavigationView())
    {}

    NavigationViewCore::~NavigationViewCore()
    {
        _backButtonClickHandler.stackCore = std::weak_ptr<NavigationViewCore>();
    }

    void NavigationViewCore::init()
    {
        ViewCore::init();

        auto self = shared_from_this<NavigationViewCore>();

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

        geometry.onChange() += [self](auto &property) { self->reLayout(); };
    }

    void NavigationViewCore::pushView(std::shared_ptr<View> view, bdn::String title)
    {
        _stack.push_back({view, title});
        updateCurrentView();
    }

    void NavigationViewCore::popView()
    {
        _stack.pop_back();
        updateCurrentView();
    }

    std::list<std::shared_ptr<View>> NavigationViewCore::childViews()
    {
        if (_container) {
            return {_container};
        }
        return {};
    }

    void NavigationViewCore::setLayout(std::shared_ptr<Layout> layout)
    {
        if (_container) {
            _container->offerLayout(layout);
        }
        ViewCore::setLayout(std::move(layout));
    }

    void NavigationViewCore::updateCurrentView()
    {
        if (!_container) {
            _container = std::make_shared<ContainerView>(viewCoreFactory());
            _container->isLayoutRoot = true;
            _container->offerLayout(layout());
            if (auto containerCore = _container->core<mac::ContainerViewCore>()) {
                addChildNSView(containerCore->nsView());
            } else {
                throw std::runtime_error("Container did not have the right core type!");
            }
        }

        _backButton.hidden = static_cast<BOOL>(_stack.size() < 2);

        _container->removeAllChildViews();

        auto oldCurrentView = _currentView;

        if (_currentView) {
            _currentView->setParentView(nullptr);
        }

        if (_stack.empty()) {
            _title.stringValue = @"--";
        } else {
            _title.stringValue = fk::stringToNSString(_stack.back().title);

            auto newView = _stack.back().view;

            _container->addChildView(newView);

            _currentView = newView;
            _currentView->visible = true;
        }

        if (oldCurrentView) {
            oldCurrentView->visible = false;
        }

        reLayout();
    }

    void NavigationViewCore::reLayout()
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
