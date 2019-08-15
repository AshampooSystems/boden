#pragma once

#include <bdn/ui/View.h>
#include <memory>

#import <Cocoa/Cocoa.h>
#import <bdn/foundationkit/conversionUtil.hh>
#import <bdn/mac/util.hh>

@protocol BdnLayoutable
@end

namespace bdn::ui::mac
{
    class ViewCore : public std::enable_shared_from_this<ViewCore>, public bdn::ui::View::Core
    {
      public:
        ViewCore() = delete;
        ViewCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory, NSView *nsView);

      public:
        void init() override;

        template <class T> std::shared_ptr<T> shared_from_this()
        {
            return std::dynamic_pointer_cast<T>(std::enable_shared_from_this<ViewCore>::shared_from_this());
        }

      public:
        NSView *nsView() const;

        void addChildNSView(NSView *childView);
        void removeFromNsSuperview();

        virtual void frameChanged();

        void scheduleLayout() override;

        Size sizeForSpace(Size availableSpace) const override;
        float baseline(Size forSize) const override;
        virtual float calculateBaseline(Size forSize, bool forIndicator) const;
        float pointScaleFactor() const override;

      protected:
        virtual void setFrame(Rect r);

        NSView *_baselineIndicator;

      private:
        NSView *_nsView;
        NSObject *_eventForwarder;
    };
}
