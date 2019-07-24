#import <bdn/foundationkit/conversionUtil.hh>
#import <bdn/ios/ImageViewCore.hh>

#include <bdn/Application.h>
#include <bdn/log.h>

#import <Foundation/Foundation.h>

using namespace std::string_literals;

@interface BodenUIImageView : UIImageView <UIViewWithFrameNotification>
@property(nonatomic, assign) std::weak_ptr<bdn::ui::ios::ViewCore> viewCore;
@end

@implementation BodenUIImageView
- (void)setFrame:(CGRect)frame
{
    [super setFrame:frame];
    if (auto viewCore = self.viewCore.lock()) {
        viewCore->frameChanged();
    }
}
@end

namespace bdn::ui::detail
{
    CORE_REGISTER(ImageView, bdn::ui::ios::ImageViewCore, ImageView)
}

namespace bdn::ui::ios
{
    UIView<UIViewWithFrameNotification> *ImageViewCore::createUIImageView()
    {
        BodenUIImageView *view = [[BodenUIImageView alloc] initWithFrame:CGRectZero];
        view.contentMode = UIViewContentModeScaleToFill;

        return view;
    }

    Size ImageViewCore::sizeForSpace(Size availableSize) const
    {
        if (UIImage *image = ((UIImageView *)this->uiView()).image) {
            Size result = iosSizeToSize(image.size);

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

    ImageViewCore::ImageViewCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory)
        : ViewCore(viewCoreFactory, createUIImageView())
    {
        url.onChange() += [=](auto &property) { setUrl(property.get()); };
    }

    void ImageViewCore::setUrl(const String &url)
    {
        ((UIImageView *)this->uiView()).image = nullptr;

        auto uri = App()->uriToBundledFileUri(url);

        if (uri.empty()) {
            uri = url;
        }

        if (cpp20::starts_with(uri, "file:///")) {
            if (auto nsURL = [NSURL URLWithString:fk::stringToNSString(uri)]) {
                if (auto localPath = nsURL.relativePath) {
                    UIImage *image = [[UIImage alloc] initWithContentsOfFile:localPath];
                    ((UIImageView *)this->uiView()).image = image;

                    if (image) {
                        originalSize = iosSizeToSize(image.size);
                        aspectRatio = originalSize->width / originalSize->height;
                    }
                }
            }
        } else {
            NSURLSession *session = [NSURLSession sharedSession];
            NSURL *nsURL = [NSURL URLWithString:fk::stringToNSString(url)];

            if (nsURL == nullptr) {
                return;
            }

            ((UIImageView *)this->uiView()).image = nullptr;
            originalSize = Size{0, 0};
            aspectRatio = 1.0;

            NSURLSessionDataTask *dataTask = [session
                  dataTaskWithURL:nsURL
                completionHandler:^(NSData *_Nullable nsData, NSURLResponse *_Nullable nsResponse,
                                    NSError *_Nullable error) {
                  if (auto err = error) {
                      logstream() << "Failed loading '" << fk::nsStringToString([nsURL absoluteString]) << "' ("
                                  << fk::nsStringToString([err localizedDescription]) << ")";
                  } else {

                      App()->dispatchQueue()->dispatchAsync([nsData, self = shared_from_this<ImageViewCore>()]() {
                          if (self) {
                              UIImage *image = [[UIImage alloc] initWithData:nsData];
                              ((UIImageView *)self->uiView()).image = image;

                              if (image) {
                                  self->originalSize = iosSizeToSize(image.size);
                                  self->aspectRatio = self->originalSize->width / self->originalSize->height;
                              }
                              self->scheduleLayout();
                              self->markDirty();
                          }
                      });
                  }
                }];

            [dataTask resume];
        }
    }
}
