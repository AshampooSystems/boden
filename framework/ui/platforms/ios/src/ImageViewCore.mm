#import <bdn/foundationkit/conversionUtil.hh>
#import <bdn/ios/ImageViewCore.hh>
#import <bdn/ios/UIUtil.hh>

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
        originalSize = Size{0, 0};
        aspectRatio = 1.0;

        bool imageChangedImmediately = false;

        if (!url.empty()) {
            imageChangedImmediately = imageFromUrl(url, [=](auto image) {
                ((UIImageView *)this->uiView()).image = image;
                originalSize = iosSizeToSize(image.size);
                aspectRatio = originalSize->width / originalSize->height;

                scheduleLayout();
                markDirty();
            });
        }

        if (!imageChangedImmediately) {
            scheduleLayout();
            markDirty();
        }
    }
}
