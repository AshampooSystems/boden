#include <bdn/ImageView.h>
#include <bdn/ImageViewCore.h>

namespace bdn
{
    String ImageView::viewCoreTypeName() const { return coreTypeName; }

    void ImageView::bindViewCore()
    {
        View::bindViewCore();

        if (auto core = std::dynamic_pointer_cast<ImageViewCore>(viewCore())) {
            core->url.bind(url);
        }
    }
}
