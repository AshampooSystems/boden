
#include <bdn/Slider.h>

namespace bdn
{
    namespace detail
    {
        VIEW_CORE_REGISTRY_IMPLEMENTATION(Slider)
    }

    Slider::Slider(std::shared_ptr<ViewCoreFactory> viewCoreFactory) : View(std::move(viewCoreFactory))
    {
        detail::VIEW_CORE_REGISTER(Slider, View::viewCoreFactory());
    }

    void Slider::bindViewCore()
    {
        View::bindViewCore();
        auto sliderCore = View::core<Slider::Core>();
        sliderCore->value.bind(value);
    }
}
