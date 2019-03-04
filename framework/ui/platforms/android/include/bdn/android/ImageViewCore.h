#pragma once

#include <bdn/ImageView.h>
#include <bdn/ImageViewCore.h>
#include <bdn/android/ViewCore.h>

namespace bdn::android
{
    class ImageViewCore : public ViewCore, virtual public bdn::ImageViewCore
    {
      public:
        ImageViewCore(std::shared_ptr<ImageView> outer);
        virtual ~ImageViewCore() = default;

      public:
        void imageLoaded(int width, int height);

      private:
        Size sizeForSpace(Size availableSpace = Size::none()) const override;

      private:
        void loadUrl();

      private:
        Size _imageSize;
    };
}