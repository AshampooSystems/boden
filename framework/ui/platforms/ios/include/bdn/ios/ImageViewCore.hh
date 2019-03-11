#pragma once

#include <bdn/ImageView.h>
#include <bdn/ImageViewCore.h>
#include <bdn/ios/ViewCore.hh>

namespace bdn::ios
{
    class ImageViewCore : public ViewCore, virtual public bdn::ImageViewCore
    {
      public:
        ImageViewCore();

      protected:
        void setUrl(const String &url);

      private:
        static UIView<UIViewWithFrameNotification> *createUIImageView();
        virtual Size sizeForSpace(Size availableSize) const override;
    };
}
