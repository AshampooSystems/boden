#pragma once

#import <bdn/ios/ViewCore.hh>
#include <bdn/ui/ListView.h>

@class ListViewDelegateIOS;

namespace bdn::ui::ios
{
    class ListViewCore : public ViewCore, virtual public ListView::Core
    {
        friend class ViewCoreFactory;

      public:
        ListViewCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory);
        void init() override;

      public:
        void reloadData() override;
        void refreshDone() override;

        void fireRefresh();

      protected:
        void updateRefresh(bool enable);

      private:
        ListViewDelegateIOS *_nativeDelegate;
    };
}
