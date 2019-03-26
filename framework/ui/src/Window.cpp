
#include <bdn/Window.h>

#include <bdn/UIApplicationController.h>
#include <bdn/debug.h>

namespace bdn
{
    namespace detail
    {
        VIEW_CORE_REGISTRY_IMPLEMENTATION(Window)
    }

    Window::Window(std::shared_ptr<ViewCoreFactory> viewCoreFactory) : View(std::move(viewCoreFactory))
    {
        detail::VIEW_CORE_REGISTER(Window, View::viewCoreFactory());

        visible = false;
        allowedOrientations = Orientation::All;

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

    void Window::childViewStolen(const std::shared_ptr<View> &childView)
    {
        AppRunnerBase::assertInMainThread();

        if (childView == content.get()) {
            content = nullptr;
        }
    }

    void Window::bindViewCore()
    {
        View::bindViewCore();

        auto windowCore = View::core<WindowCore>();

        windowCore->content.bind(content);
        windowCore->title.bind(title);

        windowCore->allowedOrientations.bind(allowedOrientations, BindMode::unidirectional);
        currentOrientation.bind(windowCore->currentOrientation, BindMode::unidirectional);

        contentGeometry.bind(windowCore->contentGeometry);
    }
}
