#pragma once

#include <bdn/ListView.h>
#include <bdn/ListViewCore.h>
#import <bdn/ios/ViewCore.hh>

@class ListViewDelegateIOS;

namespace bdn::ios
{
    class ListViewCore : public ViewCore, virtual public bdn::ListViewCore
    {
      public:
        ListViewCore(std::shared_ptr<ListView> outerListView);

        virtual void reloadData() override;

      private:
        ListViewDelegateIOS *_nativeDelegate;
    };
}
