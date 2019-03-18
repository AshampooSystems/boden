#pragma once

#include <bdn/ImageView.h>
#include <bdn/ImageViewCore.h>
#include <bdn/mac/ViewCore.hh>

namespace bdn::mac
{
    class ImageViewCore : public ViewCore, virtual public bdn::ImageViewCore
    {
      public:
        ImageViewCore(const std::shared_ptr<bdn::UIProvider> &uiProvider);

      protected:
        void setUrl(const String &url);

      private:
        static NSView *createNSImageView();
        Size sizeForSpace(Size availableSize) const override;
    };
}
