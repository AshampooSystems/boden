#pragma once

#include <bdn/ImageView.h>
#include <bdn/mac/ViewCore.hh>

namespace bdn::mac
{
    class ImageViewCore : public ViewCore, virtual public bdn::ImageView::Core
    {
      public:
        ImageViewCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory);

      protected:
        void setUrl(const String &url);

      private:
        static NSView *createNSImageView();
        Size sizeForSpace(Size availableSize) const override;
    };
}
