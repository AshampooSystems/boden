#pragma once

#include <Cocoa/Cocoa.h>

#include <bdn/NotImplementedError.h>
#include <bdn/Window.h>
#include <bdn/WindowCore.h>

#import <bdn/mac/UIProvider.hh>

#import <bdn/mac/util.hh>

@class BdnMacWindowContentViewParent_;

namespace bdn::mac
{
    class WindowCore : virtual public bdn::WindowCore
    {
      public:
        WindowCore();
        ~WindowCore();

      public:
        virtual void init() override;

        NSWindow *getNSWindow() { return _nsWindow; }

        bool canMoveToParentView(std::shared_ptr<View> newParentView) const override;

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

        void updateContent(const std::shared_ptr<View> newContent);

      private:
        NSWindow *_nsWindow;
        BdnMacWindowContentViewParent_ *_nsContentParent;

        NSObject *_ourDelegate;

        mutable double _emDipsIfInitialized = -1;
        mutable double _semDipsIfInitialized = -1;
    };
}
