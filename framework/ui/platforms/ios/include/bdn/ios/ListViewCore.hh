#pragma once

#include <bdn/ListView.h>
#include <bdn/ListViewCore.h>
#import <bdn/ios/ViewCore.hh>

@class ListViewDelegateIOS;

namespace bdn::ios
{
    class ListViewCore : public ViewCore, virtual public bdn::ListViewCore
    {
        friend class bdn::UIProvider;

      public:
        ListViewCore(const std::shared_ptr<bdn::UIProvider> &uiProvider);
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
