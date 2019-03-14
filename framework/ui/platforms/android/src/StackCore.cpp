#include <bdn/Stack.h>
#include <bdn/android/StackCore.h>
#include <bdn/android/wrapper/NativeStackView.h>

namespace bdn::android
{
    StackCore::StackCore(const std::shared_ptr<UIProvider> &uiProvider)
        : ViewCore(uiProvider, createAndroidViewClass<wrapper::NativeStackView>(uiProvider))
    {
        geometry.onChange() += [=](auto va) { this->reLayout(); };
    }

    StackCore::~StackCore() { getJViewAS<wrapper::NativeStackView>().close(); }

    void StackCore::pushView(std::shared_ptr<View> view, String title)
    {
        auto fixedView = std::make_shared<FixedView>(uiProvider());
        fixedView->addChildView(view);
        fixedView->offerLayout(layout());

        bool isFirst = _stack.empty();

        _stack.push_back({fixedView, view, title});
        updateCurrentView(isFirst, true);
    }

    void StackCore::popView()
    {
        _stack.pop_back();
        updateCurrentView(false, false);
    }

    std::list<std::shared_ptr<View>> StackCore::childViews()
    {
        if (!_stack.empty()) {
            return {_stack.back().container};
        }
        return {};
    }

    void StackCore::visitInternalChildren(std::function<void(std::shared_ptr<bdn::ViewCore>)> function)
    {
        for (auto entry : _stack) {
            function(entry.container->viewCore());
        }
    }

    void StackCore::updateCurrentView(bool first, bool enter)
    {
        if (!_stack.empty()) {
            auto fixedCore = _stack.back().container->core<android::ContainerViewCore>();

            auto nativeStackView = getJViewAS<wrapper::NativeStackView>();

            nativeStackView.setWindowTitle(_stack.back().title);
            nativeStackView.enableBackButton(_stack.size() > 1);
            nativeStackView.changeContent(fixedCore->getJView(), !first, enter);
        }

        updateChildren();
        reLayout();
    }

    bool StackCore::handleBackButton()
    {
        if (_stack.size() > 1) {
            popView();
            return true;
        }
        return false;
    }

    void StackCore::reLayout()
    {
        if (!_stack.empty()) {
            _stack.back().container->geometry = Rect{0, 0, geometry->width, geometry->height};
        }
    }
}
