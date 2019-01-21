#pragma once

#import <UIKit/UIKit.h>

#include <bdn/IWindowCore.h>
#include <bdn/Window.h>
#include <bdn/NotImplementedError.h>

#include <bdn/windowCoreUtil.h>

#import <bdn/ios/util.hh>
#import <bdn/ios/ViewCore.hh>

namespace bdn
{
    namespace ios
    {

        class WindowCore : public ViewCore,
                           virtual public IWindowCore,
                           virtual public LayoutCoordinator::IWindowCoreExtension
        {
          private:
            UIWindow *_createUIWindow(std::shared_ptr<Window> outerWindow);

          public:
            WindowCore(std::shared_ptr<Window> outerWindow);

            ~WindowCore();

            UIWindow *getUIWindow() const;
            Rect adjustBounds(const Rect &requestedBounds, RoundType positionRoundType,
                              RoundType sizeRoundType) const override;
            void setTitle(const String &title) override;
            void invalidateSizingInfo(View::InvalidateReason reason) override;
            void needLayout(View::InvalidateReason reason) override;
            void childSizingInfoInvalidated(std::shared_ptr<View> child) override;
            Size calcPreferredSize(const Size &availableSpace = Size::none()) const override;
            void layout() override;
            void requestAutoSize() override;
            void requestCenter() override;
            void autoSize() override;
            void center() override;
            bool canMoveToParentView(std::shared_ptr<View> newParentView) const override;
            void moveToParentView(std::shared_ptr<View> newParentView) override;

          private:
            Rect getContentArea();
            Rect getScreenWorkArea() const;

            UIScreen *_getUIScreen() const;
            UIWindow *_window;
        };
    }
}
