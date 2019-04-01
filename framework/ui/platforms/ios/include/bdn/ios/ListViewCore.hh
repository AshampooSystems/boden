#pragma once

#include <bdn/ListView.h>
#import <bdn/ios/ViewCore.hh>

@class ListViewDelegateIOS;

namespace bdn::ios
{
    class ListViewCore : public ViewCore, virtual public bdn::ListView::Core
    {
        friend class bdn::ViewCoreFactory;

      public:
        ListViewCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory);

      public:
        void reloadData() override;
        void refreshDone() override;

        void fireRefresh();

      protected:
        void init() override;

        void updateRefresh(bool enable);

      private:
        ListViewDelegateIOS *_nativeDelegate;
    };
}
