#include <bdn/Stack.h>
#include <bdn/android/StackCore.h>
#include <bdn/android/wrapper/NativeStackView.h>

namespace bdn::android
{
    StackCore::StackCore(const ContextWrapper &ctxt) : ViewCore(createAndroidViewClass<wrapper::NativeStackView>(ctxt))
    {
        _container = std::make_shared<FixedView>(_uiProvider);
        auto containerCore = _container->core<android::ViewCore>();
        getJViewAS<wrapper::NativeViewGroup>().addView(containerCore->getJView());

        geometry.onChange() += [=](auto va) { this->reLayout(); };
    }

    StackCore::~StackCore() { getJViewAS<wrapper::NativeStackView>().close(); }

    void StackCore::pushView(std::shared_ptr<View> view, String title)
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

    void StackCore::visitInternalChildren(std::function<void(std::shared_ptr<bdn::ViewCore>)> function)
    {
        function(_container->viewCore());
    }

    void StackCore::updateCurrentView()
    {
        if (_currentView) {
            _container->removeAllChildViews();
        }

        if (!_stack.empty()) {
            auto newView = _stack.back().view;

            _container->addChildView(newView);

            _currentView = newView;

            getJViewAS<wrapper::NativeStackView>().setWindowTitle(_stack.back().title);
            getJViewAS<wrapper::NativeStackView>().enableBackButton(_stack.size() > 1);
        }

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

    void StackCore::reLayout() { _container->geometry = Rect{0, 0, geometry->width, geometry->height}; }
}
