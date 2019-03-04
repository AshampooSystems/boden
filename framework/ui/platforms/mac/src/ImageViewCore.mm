#import <bdn/mac/ImageViewCore.hh>

#include <bdn/log.h>

namespace bdn::mac
{
    NSView *ImageViewCore::createNSImageView(std::shared_ptr<ImageView> outer)
    {
        NSImageView *view = [[NSImageView alloc] initWithFrame:rectToMacRect(outer->geometry, -1)];
        view.imageScaling = NSImageScaleAxesIndependently;

        return view;
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

        return ChildViewCore::sizeForSpace(availableSize);
    }

    ImageViewCore::ImageViewCore(std::shared_ptr<ImageView> outer) : ChildViewCore(outer, createNSImageView(outer))
    {
        url.onChange() += [=](auto va) { setUrl(va->get()); };
    }

    void ImageViewCore::setUrl(const String &url)
    {
        ((NSImageView *)this->nsView()).image = nullptr;

        NSURLSession *session = [NSURLSession sharedSession];
        NSURL *nsURL = [NSURL URLWithString:stringToNSString(url)];

        NSURLSessionDataTask *dataTask =
            [session dataTaskWithURL:nsURL
                   completionHandler:^(NSData *_Nullable nsData, NSURLResponse *_Nullable nsResponse,
                                       NSError *_Nullable error) {
                     if (auto err = error) {
                         logstream() << "Failed loading '" << nsStringToString([nsURL absoluteString]) << "' ("
                                     << nsStringToString([err localizedDescription]) << ")";
                     } else {

                         getMainDispatcher()->enqueue([=]() {
                             ((NSImageView *)this->nsView()).image = [[NSImage alloc] initWithData:nsData];
                             this->scheduleLayout();
                             if (auto outer = outerView()) {
                                 if (auto layout = outer->getLayout()) {
                                     layout->markDirty(outer.get());
                                 }
                             }
                         });
                     }
                   }];

        [dataTask resume];
    }
}
