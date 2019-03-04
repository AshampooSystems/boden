
#include <bdn/ios/ContainerViewCore.hh>

@implementation BodenUIView
- (void)setFrame:(CGRect)frame
{
    [super setFrame:frame];
    if (_viewCore) {
        _viewCore->frameChanged();
    }
}

- (void)layoutSubviews
{
    if (_viewCore) {
        if (auto view = _viewCore->outerView()) {
            if (auto layout = view->getLayout()) {
                layout->layout(view.get());
            }
        }
    }
}

@end

namespace bdn::ios
{
    BodenUIView *_createContainer(std::shared_ptr<ContainerView> outer)
    {
        return [[BodenUIView alloc] initWithFrame:CGRectMake(0, 0, 0, 0)];
    }

    ContainerViewCore::ContainerViewCore(std::shared_ptr<ContainerView> outer)
        : ViewCore(outer, _createContainer(outer))
    {}

    ContainerViewCore::ContainerViewCore(std::shared_ptr<ContainerView> outer, id<UIViewWithFrameNotification> view)
        : ViewCore(outer, view)
    {}

    bool ContainerViewCore::canAdjustToAvailableWidth() const { return true; }

    bool ContainerViewCore::canAdjustToAvailableHeight() const { return true; }
}
