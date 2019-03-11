#pragma once

#include <bdn/ContainerView.h>
#include <bdn/ContainerViewCore.h>
#import <bdn/mac/ViewCore.hh>

namespace bdn::mac
{
    class ContainerViewCore : public ViewCore, virtual public bdn::ContainerViewCore
    {
      private:
        static NSView *_createContainer();

      public:
        ContainerViewCore();
        explicit ContainerViewCore(NSView *view);

      public:
        virtual void init() override;

        virtual void addChildView(std::shared_ptr<View> child) override;
        virtual void removeChildView(std::shared_ptr<View> child) override;

        virtual std::list<std::shared_ptr<View>> childViews() override;

      private:
        std::list<std::shared_ptr<View>> _children;
    };
}
