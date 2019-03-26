#include <bdn/ImageView.h>
#include <bdn/ImageViewCore.h>

namespace bdn
{
    namespace detail
    {
        VIEW_CORE_REGISTRY_IMPLEMENTATION(ImageView)
    }

    ImageView::ImageView(std::shared_ptr<ViewCoreFactory> viewCoreFactory) : View(std::move(viewCoreFactory))
    {
        detail::VIEW_CORE_REGISTER(ImageView, View::viewCoreFactory());
    }

    String ImageView::viewCoreTypeName() const { return String(static_cast<const char *>(coreTypeName)); }

    void ImageView::bindViewCore()
    {
        View::bindViewCore();

        auto imageCore = core<ImageViewCore>();
        imageCore->url.bind(url);
    }
}
