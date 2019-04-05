#pragma once

#include <Cocoa/Cocoa.h>

#include <bdn/View.h>
#import <bdn/mac/util.hh>

@protocol BdnLayoutable
@end

namespace bdn::mac
{
    class ViewCore : public std::enable_shared_from_this<ViewCore>, public bdn::View::Core
    {
      public:
        ViewCore() = delete;
        ViewCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory, NSView *nsView);
        virtual ~ViewCore() = default;

      public:
        void init() override;

        template <class T> std::shared_ptr<T> shared_from_this()
        {
            return std::dynamic_pointer_cast<T>(std::enable_shared_from_this<ViewCore>::shared_from_this());
        }

        bool canMoveToParentView(std::shared_ptr<View> newParentView) const override { return true; }

        NSView *nsView() const;

        void addChildNSView(NSView *childView);
        void removeFromNsSuperview();

        void frameChanged();

        void scheduleLayout() override;

        Size sizeForSpace(Size availableSpace) const override;

      private:
        NSView *_nsView;
        NSObject *_eventForwarder;
    };
}
