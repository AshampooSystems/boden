#include <bdn/Application.h>
#include <bdn/log.h>
#import <bdn/mac/ImageViewCore.hh>

using namespace std::string_literals;

namespace bdn::detail
{
    CORE_REGISTER(ImageView, bdn::mac::ImageViewCore, ImageView)
}

namespace bdn::mac
{
    NSView *ImageViewCore::createNSImageView()
    {
        NSImageView *view = [[NSImageView alloc] init];
        view.imageScaling = NSImageScaleAxesIndependently;

        return view;
    }

    ImageViewCore::ImageViewCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory)
        : bdn::mac::ViewCore(viewCoreFactory, createNSImageView())
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

        if (bdn::cpp20::starts_with(url, "resource://"s)) {
            if (auto nsURL = [NSURL URLWithString:fk::stringToNSString(url)]) {
                auto server = nsURL.host;
                NSBundle *bundle = [NSBundle mainBundle];
                if (server && [server compare:@"main"] != NSOrderedSame) {
                    bundle = [NSBundle bundleWithIdentifier:server];
                }
                if (bundle) {
                    if (auto localPath = [nsURL.relativePath substringFromIndex:1]) {
                        NSString *imageName = [localPath stringByReplacingOccurrencesOfString:@"/" withString:@"_"];
                        imageName = [imageName stringByReplacingOccurrencesOfString:@"." withString:@"_"];

                        NSImage *image = [NSImage imageNamed:imageName];

                        if (image) {
                            ((NSImageView *)this->nsView()).image = image;
                        }
                    }
                }
            }
        } else if (bdn::cpp20::starts_with(url, "file://"s)) {
            if (auto nsURL = [NSURL URLWithString:fk::stringToNSString(url)]) {
                if (auto localPath = nsURL.relativePath) {
                    ((NSImageView *)this->nsView()).image = [[NSImage alloc] initWithContentsOfFile:localPath];
                }
            }
        } else {

            NSURLSession *session = [NSURLSession sharedSession];
            NSURL *nsURL = [NSURL URLWithString:fk::stringToNSString(url)];

            if (nsURL == nullptr) {
                return;
            }

            NSURLSessionDataTask *dataTask =
                [session dataTaskWithURL:nsURL
                       completionHandler:^(NSData *_Nullable nsData, NSURLResponse *_Nullable nsResponse,
                                           NSError *_Nullable error) {
                         if (auto err = error) {
                             logstream() << "Failed loading '" << fk::nsStringToString([nsURL absoluteString]) << "' ("
                                         << fk::nsStringToString([err localizedDescription]) << ")";
                         } else {

                             App()->dispatcher()->enqueue([=]() {
                                 ((NSImageView *)this->nsView()).image = [[NSImage alloc] initWithData:nsData];
                                 this->scheduleLayout();
                                 this->markDirty();
                             });
                         }
                       }];

            [dataTask resume];
        }

        this->scheduleLayout();
        this->markDirty();
    }
}
