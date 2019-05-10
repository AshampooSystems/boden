#include <bdn/ui/ImageView.h>

namespace bdn::ui
{
    namespace detail
    {
        VIEW_CORE_REGISTRY_IMPLEMENTATION(ImageView)
    }

    ImageView::ImageView(std::shared_ptr<ViewCoreFactory> viewCoreFactory) : View(std::move(viewCoreFactory))
    {
        detail::VIEW_CORE_REGISTER(ImageView, View::viewCoreFactory());
    }

    void ImageView::bindViewCore()
    {
        View::bindViewCore();

        auto imageCore = core<ImageView::Core>();

        iOriginalSize.bind(imageCore->originalSize, BindMode::unidirectional);
        iAspectRatio.bind(imageCore->aspectRatio, BindMode::unidirectional);

        imageCore->url.bind(url);
    }
}
