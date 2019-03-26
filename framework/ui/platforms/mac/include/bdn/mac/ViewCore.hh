#pragma once

#include <Cocoa/Cocoa.h>

#include <bdn/ProgrammingError.h>
#include <bdn/ViewCore.h>

#import <bdn/mac/util.hh>

@protocol BdnLayoutable
@end

namespace bdn::mac
{
    class ViewCore : public bdn::ViewCore
    {
      public:
        ViewCore() = delete;
        ViewCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory, NSView *nsView);
        ~ViewCore() override = default;

      public:
        void init() override;

        bool canMoveToParentView(std::shared_ptr<View> newParentView) const override { return true; }

        NSView *nsView() const;

        void addChildNSView(NSView *childView);
        void removeFromNsSuperview();

        void frameChanged();

        void scheduleLayout() override;

        Size sizeForSpace(Size availableSpace) const override;

      private:
        NSView *_nsView;

        mutable double _emDipsIfInitialized = -1;
        mutable double _semDipsIfInitialized = -1;

        NSObject *_eventForwarder;
    };
}
