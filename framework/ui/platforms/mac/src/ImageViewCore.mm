#import <bdn/mac/ImageViewCore.hh>
#import <bdn/mac/UIUtil.hh>

#include <bdn/Application.h>
#include <bdn/String.h>
#include <bdn/log.h>

using namespace std::string_literals;

namespace bdn::ui::detail
{
    CORE_REGISTER(ImageView, bdn::ui::mac::ImageViewCore, ImageView)
}

namespace bdn::ui::mac
{
    NSView *ImageViewCore::createNSImageView()
    {
        NSImageView *view = [[NSImageView alloc] init];
        view.imageScaling = NSImageScaleAxesIndependently;

        return view;
    }

    ImageViewCore::ImageViewCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory)
        : mac::ViewCore(viewCoreFactory, createNSImageView())
    {
        url.onChange() += [=](auto &property) { setUrl(property.get()); };
    }

    Size ImageViewCore::sizeForSpace(Size availableSize) const
    {
        if (NSImage *image = ((NSImageView *)this->nsView()).image) {
            Size result = macSizeToSize(image.size);

            if (availableSize.width > availableSize.height) {
                float factor = result.width / result.height;
                result.width = availableSize.height * factor;
                result.height = availableSize.height;
            } else {
                float factor = result.height / result.width;
                result.width = availableSize.width;
                result.height = availableSize.width * factor;
            }
            return result;
        }

        return ViewCore::sizeForSpace(availableSize);
    }

    void ImageViewCore::setUrl(const String &url)
    {
        ((NSImageView *)this->nsView()).image = nullptr;
        originalSize = Size{0, 0};
        aspectRatio = 1.0;

        bool imageChangedImmediately = false;

        if (!url.empty()) {
            imageChangedImmediately = imageFromUrl(url, [=](auto image) {
                ((NSImageView *)this->nsView()).image = image;

                if (image) {
                    originalSize = macSizeToSize(image.size);
                    aspectRatio = originalSize->width / originalSize->height;
                }
                this->scheduleLayout();
                this->markDirty();
            });
        }

        if (!imageChangedImmediately) {
            this->scheduleLayout();
            this->markDirty();
        }
    }
}
