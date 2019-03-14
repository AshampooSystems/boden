#import <bdn/foundationkit/stringUtil.hh>
#import <bdn/ios/ImageViewCore.hh>

#include <bdn/log.h>

#import <Foundation/Foundation.h>

@interface BodenUIImageView : UIImageView <UIViewWithFrameNotification>
@property(nonatomic, assign) std::weak_ptr<bdn::ios::ViewCore> viewCore;
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

namespace bdn::ios
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

    ImageViewCore::ImageViewCore(const std::shared_ptr<bdn::UIProvider> &uiProvider)
        : ViewCore(uiProvider, createUIImageView())
    {
        url.onChange() += [=](auto va) { setUrl(va->get()); };
    }

    void ImageViewCore::setUrl(const String &url)
    {
        ((UIImageView *)this->uiView()).image = nullptr;

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
                             ((UIImageView *)this->uiView()).image = [UIImage imageWithData:nsData];
                             this->scheduleLayout();
                             markDirty();
                         });
                     }
                   }];

        [dataTask resume];
    }
}
