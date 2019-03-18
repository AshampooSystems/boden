#include <bdn/ImageView.h>
#include <bdn/ImageViewCore.h>

namespace bdn
{
    ImageView::ImageView(std::shared_ptr<UIProvider> uiProvider) : View(std::move(uiProvider)) {}

    String ImageView::viewCoreTypeName() const { return String(static_cast<const char *>(coreTypeName)); }

    void ImageView::bindViewCore()
    {
        View::bindViewCore();

        auto imageCore = core<ImageViewCore>();
        imageCore->url.bind(url);
    }
}
