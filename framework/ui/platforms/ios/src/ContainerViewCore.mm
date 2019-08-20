
#include <bdn/ios/ContainerViewCore.hh>

@implementation BodenUIView
- (void)setFrame:(CGRect)frame { [super setFrame:frame]; }

- (void)layoutSubviews
{
    if (auto viewCore = self.viewCore.lock()) {
        viewCore->frameChanged();
    }

    if (auto viewCore = self.viewCore.lock()) {
        viewCore->startLayout();
    }
}

@end

namespace bdn::ui::detail
{
    CORE_REGISTER(ContainerView, bdn::ui::ios::ContainerViewCore, ContainerView)
}

namespace bdn::ui::ios
{
    BodenUIView *_createContainer() { return [[BodenUIView alloc] initWithFrame:CGRectZero]; }

    ContainerViewCore::ContainerViewCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory)
        : ViewCore(viewCoreFactory, _createContainer())
    {}

    ContainerViewCore::ContainerViewCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory,
                                         id<UIViewWithFrameNotification> view)
        : ViewCore(viewCoreFactory, view)
    {}

    bool ContainerViewCore::canAdjustToAvailableWidth() const { return true; }

    bool ContainerViewCore::canAdjustToAvailableHeight() const { return true; }

    void ContainerViewCore::addChildView(std::shared_ptr<View> child)
    {
        if (auto childCore = child->core<ViewCore>()) {
            addChildViewCore(childCore);
            _children.push_back(child);
        } else {
            throw std::runtime_error("Cannot add this type of View");
        }

        scheduleLayout();
    }

    void ContainerViewCore::removeChildView(std::shared_ptr<View> child)
    {
        if (auto childCore = child->core<ViewCore>()) {
            childCore->removeFromUISuperview();

            auto it = std::remove(_children.begin(), _children.end(), child);
            if (it != _children.end()) {
                _children.erase(it, _children.end());
            }
        } else {
            throw std::runtime_error("Cannot remove this type of View");
        }
        scheduleLayout();
    }

    std::vector<std::shared_ptr<View>> ContainerViewCore::childViews() const { return _children; }
}
