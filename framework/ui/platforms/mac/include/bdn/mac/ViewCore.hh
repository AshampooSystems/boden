#pragma once

#include <Cocoa/Cocoa.h>

#include <bdn/ProgrammingError.h>
#include <bdn/ViewCore.h>

#import <bdn/mac/UIProvider.hh>
#import <bdn/mac/util.hh>

@protocol BdnLayoutable
@end

namespace bdn::mac
{
    class ViewCore : public bdn::ViewCore
    {
      public:
        ViewCore() = delete;
        ViewCore(const std::shared_ptr<bdn::UIProvider> &uiProvider, NSView *nsView);
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

      protected:
        virtual double getFontSize() const
        {
            // most views do not have a font size attached to them in cocoa.
            // Those should override this function.
            // In the default implementation we simply return the system
            // font size.
            return getSemSizeDips();
        }

        double getEmSizeDips() const
        {
            if (_emDipsIfInitialized == -1) {
                _emDipsIfInitialized = getFontSize();
            }

            return _emDipsIfInitialized;
        }

        double getSemSizeDips() const
        {
            if (_semDipsIfInitialized == -1) {
                _semDipsIfInitialized = UIProvider::get()->getSemSizeDips();
            }

            return _semDipsIfInitialized;
        }

      private:
        NSView *_nsView;

        mutable double _emDipsIfInitialized = -1;
        mutable double _semDipsIfInitialized = -1;

        NSObject *_eventForwarder;
    };
}
