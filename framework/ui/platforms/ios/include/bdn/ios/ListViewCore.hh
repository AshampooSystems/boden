#pragma once

#include <bdn/ListView.h>
#include <bdn/ListViewCore.h>
#import <bdn/ios/ViewCore.hh>

@class ListViewDelegateIOS;

namespace bdn::ios
{
    class ListViewCore : public ViewCore, virtual public bdn::ListViewCore
    {
        friend class bdn::ViewCoreFactory;

      public:
        ListViewCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory);
        ~ListViewCore() override = default;

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
