#pragma once

#include <bdn/ui/ImageView.h>

#include <bdn/android/ViewCore.h>

namespace bdn::ui::android
{
    class ImageViewCore : public ViewCore, virtual public ImageView::Core
    {
      public:
        ImageViewCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory);
        ~ImageViewCore() override = default;

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
