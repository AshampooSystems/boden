#pragma once

#include <bdn/ContainerView.h>
#import <bdn/mac/ChildViewCore.hh>
#include <bdn/mac/IParentViewCore.h>

namespace bdn::mac
{
    class ContainerViewCore : public ChildViewCore, virtual public IParentViewCore
    {
      private:
        static NSView *_createContainer(std::shared_ptr<ContainerView> outer);

      public:
        ContainerViewCore(std::shared_ptr<ContainerView> outer) : ChildViewCore(outer, _createContainer(outer)) {}
        ContainerViewCore(std::shared_ptr<ContainerView> outer, NSView *view) : ChildViewCore(outer, view) {}
    };
}
