#pragma once

#include <bdn/ui/Window.h>

#import <Cocoa/Cocoa.h>
#import <bdn/mac/util.hh>

@class BdnMacWindowContentViewParent_;

namespace bdn::ui::mac
{
    class WindowCore : public ViewCore, virtual public Window::Core
    {
      public:
        WindowCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory);
        ~WindowCore();

      public:
        void init() override;

        NSWindow *getNSWindow() { return _nsWindow; }

        void _movedOrResized();

        void scheduleLayout() override;

      private:
        Rect getContentArea();
        Rect getScreenWorkArea() const;
        Size getMinimumSize() const;

        NSScreen *_getNsScreen() const;

        void updateContent(const std::shared_ptr<View> &newContent);

      private:
        bool _isInMoveOrResize = false;
        NSWindow *_nsWindow;
        BdnMacWindowContentViewParent_ *_nsContentParent;

        NSObject *_ourDelegate;
    };
}
