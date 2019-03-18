#pragma once

#include <bdn/ImageView.h>
#include <bdn/ImageViewCore.h>
#include <bdn/ios/ViewCore.hh>

namespace bdn::ios
{
    class ImageViewCore : public ViewCore, virtual public bdn::ImageViewCore
    {
      public:
        ImageViewCore(const std::shared_ptr<bdn::UIProvider> &uiProvider);

      protected:
        void setUrl(const String &url);

      private:
        static UIView<UIViewWithFrameNotification> *createUIImageView();
        Size sizeForSpace(Size availableSize) const override;
    };
}
