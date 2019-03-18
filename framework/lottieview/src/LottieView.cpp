#include <bdn/LottieView.h>
#include <bdn/LottieViewCore.h>
#include <bdn/UIProvider.h>
#include <bdn/UIUtil.h>

namespace bdn
{
    namespace lottieview::detail
    {
        VIEW_CORE_REGISTRY_IMPLEMENTATION(LottieView)
    }

    LottieView::LottieView(std::shared_ptr<UIProvider> uiProvider) : View(std::move(uiProvider))
    {
        lottieview::detail::VIEW_CORE_REGISTER(LottieView, View::uiProvider());

        registerCoreCreatingProperties(this, &url);
        url.onChange() += [this](auto) { loadURL(url); };
    }

    void LottieView::loadURL(const String &url)
    {
        auto lottieCore = core<LottieViewCore>();
        lottieCore->loadURL(url);
    }

    String LottieView::viewCoreTypeName() const { return String((const char *)coreTypeName); }

    void LottieView::bindViewCore()
    {
        View::bindViewCore();
        auto lottieCore = core<LottieViewCore>();
        lottieCore->running.bind(running);
        lottieCore->loop.bind(loop);
    }
}
