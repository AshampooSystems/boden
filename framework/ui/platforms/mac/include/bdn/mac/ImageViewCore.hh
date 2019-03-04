#pragma once

#include <bdn/ImageView.h>
#include <bdn/ImageViewCore.h>
#include <bdn/mac/ChildViewCore.hh>

namespace bdn::mac
{
    class ImageViewCore : public ChildViewCore, virtual public bdn::ImageViewCore
    {
      public:
        ImageViewCore(std::shared_ptr<ImageView> outer);

      protected:
        void setUrl(const String &url);

      private:
        static NSView *createNSImageView(std::shared_ptr<ImageView> outer);
        virtual Size sizeForSpace(Size availableSize) const override;
    };
}
