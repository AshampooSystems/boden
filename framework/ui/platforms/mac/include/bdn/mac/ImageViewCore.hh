#pragma once

#include <bdn/mac/ViewCore.hh>
#include <bdn/ui/ImageView.h>

namespace bdn::ui::mac
{
    class ImageViewCore : public ViewCore, virtual public ImageView::Core
    {
      public:
        ImageViewCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory);

      protected:
        void setUrl(const std::string &url);

      private:
        static NSView *createNSImageView();
        Size sizeForSpace(Size availableSize) const override;
    };
}
