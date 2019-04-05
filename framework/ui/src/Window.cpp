
#include <bdn/Window.h>

#include <bdn/Application.h>
#include <bdn/UIApplicationController.h>
#include <bdn/debug.h>

#include <sstream>

namespace bdn
{
    namespace detail
    {
        VIEW_CORE_REGISTRY_IMPLEMENTATION(Window)
    }

    String Window::orientationToString(Window::Orientation orientation)
    {
        using namespace std::string_literals;
        std::stringstream str;
        if (orientation & Orientation::Portrait) {
            str << "Portrait"s;
        }
        if (orientation & Orientation::LandscapeLeft) {
            if (str.tellp() != 0) {
                str << " | ";
            }
            str << "LandscapeLeft"s;
        }
        if (orientation & Orientation::LandscapeRight) {
            if (str.tellp() != 0) {
                str << " | ";
            }
            str << "LandscapeRight"s;
        }
        if (orientation & Orientation::PortraitUpsideDown) {
            if (str.tellp() != 0) {
                str << " | ";
            }
            str << "PortraitUpsideDown"s;
        }
        return str.str();
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
        Application::assertInMainThread();
        if (content.get()) {
            return {content.get()};
        }
        return {};
    }

    void Window::removeAllChildViews() { content = nullptr; }

    void Window::childViewStolen(const std::shared_ptr<View> &childView)
    {
        Application::assertInMainThread();

        if (childView == content.get()) {
            content = nullptr;
        }
    }

    void Window::bindViewCore()
    {
        View::bindViewCore();

        auto windowCore = View::core<Window::Core>();

        windowCore->content.bind(content);
        windowCore->title.bind(title);

        windowCore->allowedOrientations.bind(allowedOrientations, BindMode::unidirectional);
        currentOrientation.bind(windowCore->currentOrientation, BindMode::unidirectional);

        contentGeometry.bind(windowCore->contentGeometry);
    }
}
