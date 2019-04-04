#include <bdn/NavigationView.h>
#include <bdn/android/NavigationViewCore.h>
#include <bdn/android/wrapper/NativeNavigationView.h>

namespace bdn::detail
{
    CORE_REGISTER(NavigationView, bdn::android::NavigationViewCore, NavigationView)
}

namespace bdn::android
{
    NavigationViewCore::NavigationViewCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory)
        : ViewCore(viewCoreFactory, createAndroidViewClass<wrapper::NativeNavigationView>(viewCoreFactory))
    {
        geometry.onChange() += [=](auto va) { this->reLayout(); };
    }

    NavigationViewCore::~NavigationViewCore() { getJViewAS<wrapper::NativeNavigationView>().close(); }

    void NavigationViewCore::pushView(std::shared_ptr<View> view, String title)
    {
        auto fixedView = std::make_shared<FixedView>(viewCoreFactory());
        fixedView->addChildView(view);
        fixedView->offerLayout(layout());

        bool isFirst = _stack.empty();

        _stack.push_back({fixedView, view, title});
        updateCurrentView(isFirst, true);
    }

    void NavigationViewCore::popView()
    {
        _stack.pop_back();
        updateCurrentView(false, false);
    }

    std::list<std::shared_ptr<View>> NavigationViewCore::childViews()
    {
        if (!_stack.empty()) {
            return {_stack.back().container};
        }
        return {};
    }

    void
    NavigationViewCore::visitInternalChildren(const std::function<void(std::shared_ptr<bdn::View::Core>)> &function)
    {
        for (const auto &entry : _stack) {
            function(entry.container->viewCore());
        }
    }

    void NavigationViewCore::updateCurrentView(bool first, bool enter)
    {
        if (!_stack.empty()) {
            auto fixedCore = _stack.back().container->core<android::ContainerViewCore>();

            auto NativeNavigationView = getJViewAS<wrapper::NativeNavigationView>();

            NativeNavigationView.setWindowTitle(_stack.back().title);
            NativeNavigationView.enableBackButton(_stack.size() > 1);
            NativeNavigationView.changeContent(fixedCore->getJView(), !first, enter);
        }

        updateChildren();
        reLayout();
    }

    bool NavigationViewCore::handleBackButton()
    {
        if (_stack.size() > 1) {
            popView();
            return true;
        }
        return false;
    }

    void NavigationViewCore::reLayout()
    {
        if (!_stack.empty()) {
            _stack.back().container->geometry = Rect{0, 0, geometry->width, geometry->height};
        }
    }
}
