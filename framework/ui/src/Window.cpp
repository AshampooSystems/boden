
#include <bdn/Window.h>

#include <bdn/UIAppControllerBase.h>
#include <bdn/debug.h>

namespace bdn
{

    Window::Window(std::shared_ptr<UIProvider> uiProvider)
    {
        visible = false;

        visible.onChange() += [this](auto va) {
            if (va->get()) {
                this->_initCore(nullptr);
            } else {
                this->_deinitCore();
            }
        };

        _uiProvider = (uiProvider != nullptr) ? uiProvider : UIAppControllerBase::get()->getUIProvider();
    }

    std::list<std::shared_ptr<View>> Window::getChildViews() const
    {
        AppRunnerBase::assertInMainThread();
        if (content.get()) {
            return {content.get()};
        }
        return {};
    }

    void Window::removeAllChildViews() { content = nullptr; }

    std::shared_ptr<View> Window::findPreviousChildView(std::shared_ptr<View> childView)
    {
        // we do not have multiple child views with an order - just a single
        // content view
        return nullptr;
    }

    void Window::_childViewStolen(std::shared_ptr<View> childView)
    {
        AppRunnerBase::assertInMainThread();

        if (childView == content.get())
            content = nullptr;
    }

    std::shared_ptr<UIProvider> Window::determineUIProvider(std::shared_ptr<View> parentView)
    {
        // our UI provider never changes. Just return the current one.
        return std::dynamic_pointer_cast<UIProvider>(_uiProvider);
    }

    void Window::bindViewCore()
    {
        if (auto windowCore = std::dynamic_pointer_cast<IWindowCore>(getViewCore())) {
            windowCore->content.bind(content);
            contentGeometry.bind(windowCore->contentGeometry);
            windowCore->title.bind(title);
        }
        View::bindViewCore();
    }
}
