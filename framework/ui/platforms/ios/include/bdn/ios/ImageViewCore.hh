#pragma once

#include <bdn/ios/ViewCore.hh>
#include <bdn/ui/ImageView.h>

namespace bdn::ui::ios
{
    class ImageViewCore : public ViewCore, virtual public ImageView::Core
    {
      public:
        ImageViewCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory);

      protected:
        void setUrl(const String &url);

      private:
        static UIView<UIViewWithFrameNotification> *createUIImageView();
        Size sizeForSpace(Size availableSize) const override;
    };
}
