#pragma once

#include <Cocoa/Cocoa.h>

#include <bdn/IWindowCore.h>
#include <bdn/NotImplementedError.h>
#include <bdn/Window.h>

#import <bdn/mac/UIProvider.hh>

#include <bdn/mac/IParentViewCore.h>

#import <bdn/mac/util.hh>

@interface BdnMacWindowContentViewParent_ : NSView <BdnLayoutable>
@property bdn::Window *bdnWindow;
@end

namespace bdn
{
    namespace mac
    {

        class WindowCore : public Base, virtual public IWindowCore, virtual public IParentViewCore
        {
          public:
            WindowCore(std::shared_ptr<View> outer);
            ~WindowCore();

          public:
            NSWindow *getNSWindow() { return _nsWindow; }

            std::shared_ptr<Window> getOuterWindowIfStillAttached() { return _outerWindowWeak.lock(); }
            std::shared_ptr<const Window> getOuterWindowIfStillAttached() const { return _outerWindowWeak.lock(); }

            bool canMoveToParentView(std::shared_ptr<View> newParentView) const override;
            void moveToParentView(std::shared_ptr<View> newParentView) override;

            void dispose() override;

            void addChildNsView(NSView *childView) override;

            void _movedOrResized();

            virtual void scheduleLayout() override;

          private:
            Rect getContentArea();

            Rect getScreenWorkArea() const;

            Size getMinimumSize() const;

            Margin getNonClientMargin() const;

            double getEmSizeDips() const;

            double getSemSizeDips() const;

            NSScreen *_getNsScreen() const;

          private:
            std::weak_ptr<Window> _outerWindowWeak;
            NSWindow *_nsWindow;
            BdnMacWindowContentViewParent_ *_nsContentParent;

            NSObject *_ourDelegate;

            mutable double _emDipsIfInitialized = -1;
            mutable double _semDipsIfInitialized = -1;
        };
    }
}
