#include <bdn/ui/ViewCoreFactory.h>
#include <bdn/ui/ViewUtilities.h>
#include <bdn/ui/lottie/View.h>

namespace bdn::ui::lottie
{
    namespace detail
    {
        VIEW_CORE_REGISTRY_IMPLEMENTATION(View)
    }

    View::View(std::shared_ptr<ViewCoreFactory> viewCoreFactory) : ui::View(std::move(viewCoreFactory))
    {
        detail::VIEW_CORE_REGISTER(View, ui::View::viewCoreFactory());

        registerCoreCreatingProperties(this, &url);
        url.onChange() += [this](auto) { loadURL(url); };
    }

    void View::loadURL(const String &url)
    {
        auto lottieCore = core<View::Core>();
        lottieCore->loadURL(url);
    }

    void View::bindViewCore()
    {
        ui::View::bindViewCore();
        auto lottieCore = core<View::Core>();
        lottieCore->running.bind(running);
        lottieCore->loop.bind(loop);
    }
}
