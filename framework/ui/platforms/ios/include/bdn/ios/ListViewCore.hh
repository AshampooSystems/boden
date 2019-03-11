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
        ListViewCore();
        virtual ~ListViewCore() = default;

        virtual void reloadData() override;

      protected:
        virtual void init() override;

      private:
        ListViewDelegateIOS *_nativeDelegate;
    };
}
