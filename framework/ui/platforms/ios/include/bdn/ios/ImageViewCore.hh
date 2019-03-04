#pragma once

#include <bdn/ImageView.h>
#include <bdn/ImageViewCore.h>
#include <bdn/ios/ViewCore.hh>

namespace bdn::ios
{
    class ImageViewCore : public ViewCore, virtual public bdn::ImageViewCore
    {
      public:
        ImageViewCore(std::shared_ptr<ImageView> outer);

      protected:
        void setUrl(const String &url);

      private:
        static UIView<UIViewWithFrameNotification> *createUIImageView(std::shared_ptr<ImageView> outer);
        virtual Size sizeForSpace(Size availableSize) const override;
    };
}
