
#include <bdn/Window.h>

#include <bdn/UIAppControllerBase.h>
#include <bdn/debug.h>

namespace bdn
{

    Window::Window(std::shared_ptr<UIProvider> uiProvider) : View(std::move(uiProvider))
    {
        visible = false;

        registerCoreCreatingProperties(this, &visible, &content, &geometry, &contentGeometry);

        content.onChange() += [=](auto va) { _content.update(shared_from_this(), va->get()); };
    }

    std::list<std::shared_ptr<View>> Window::childViews()
    {
        AppRunnerBase::assertInMainThread();
        if (content.get()) {
            return {content.get()};
        }
        return {};
    }

    void Window::removeAllChildViews() { content = nullptr; }

    void Window::childViewStolen(std::shared_ptr<View> childView)
    {
        AppRunnerBase::assertInMainThread();

        if (childView == content.get())
            content = nullptr;
    }

    void Window::bindViewCore()
    {
        View::bindViewCore();

        auto windowCore = View::core<WindowCore>();

        windowCore->content.bind(content);
        windowCore->title.bind(title);

        contentGeometry.bind(windowCore->contentGeometry);
    }
}
