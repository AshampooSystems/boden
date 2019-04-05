#include <bdn/LottieView.h>
#include <bdn/ViewCoreFactory.h>
#include <bdn/ViewUtilities.h>

namespace bdn
{
    namespace lottieview::detail
    {
        VIEW_CORE_REGISTRY_IMPLEMENTATION(LottieView)
    }

    LottieView::LottieView(std::shared_ptr<ViewCoreFactory> viewCoreFactory) : View(std::move(viewCoreFactory))
    {
        lottieview::detail::VIEW_CORE_REGISTER(LottieView, View::viewCoreFactory());

        registerCoreCreatingProperties(this, &url);
        url.onChange() += [this](auto) { loadURL(url); };
    }

    void LottieView::loadURL(const String &url)
    {
        auto lottieCore = core<LottieView::Core>();
        lottieCore->loadURL(url);
    }

    void LottieView::bindViewCore()
    {
        View::bindViewCore();
        auto lottieCore = core<LottieView::Core>();
        lottieCore->running.bind(running);
        lottieCore->loop.bind(loop);
    }
}
