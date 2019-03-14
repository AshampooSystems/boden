#import <bdn/mac/ImageViewCore.hh>

#include <bdn/log.h>

namespace bdn::mac
{
    NSView *ImageViewCore::createNSImageView()
    {
        NSImageView *view = [[NSImageView alloc] init];
        view.imageScaling = NSImageScaleAxesIndependently;

        return view;
    }

    ImageViewCore::ImageViewCore(const std::shared_ptr<bdn::UIProvider> &uiProvider)
        : bdn::mac::ViewCore(uiProvider, createNSImageView())
    {
        url.onChange() += [=](auto va) { setUrl(va->get()); };
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

        NSURLSession *session = [NSURLSession sharedSession];
        NSURL *nsURL = [NSURL URLWithString:fk::stringToNSString(url)];

        NSURLSessionDataTask *dataTask =
            [session dataTaskWithURL:nsURL
                   completionHandler:^(NSData *_Nullable nsData, NSURLResponse *_Nullable nsResponse,
                                       NSError *_Nullable error) {
                     if (auto err = error) {
                         logstream() << "Failed loading '" << fk::nsStringToString([nsURL absoluteString]) << "' ("
                                     << fk::nsStringToString([err localizedDescription]) << ")";
                     } else {

                         getMainDispatcher()->enqueue([=]() {
                             ((NSImageView *)this->nsView()).image = [[NSImage alloc] initWithData:nsData];
                             this->scheduleLayout();
                             this->markDirty();
                         });
                     }
                   }];

        [dataTask resume];
    }
}
