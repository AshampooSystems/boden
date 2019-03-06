#pragma once

#include <bdn/FixedView.h>
#include <bdn/Stack.h>

#include <bdn/StackCore.h>
#include <bdn/android/ContainerViewCore.h>
#include <bdn/android/WindowCore.h>

namespace bdn::android
{
    class NavButtonHandler;

    class StackCore : public ViewCore,
                      public bdn::StackCore,
                      public IParentViewCore,
                      public WindowCore::AndroidNavigationButtonHandler
    {
      public:
        StackCore(std::shared_ptr<Stack> outerStack);
        virtual ~StackCore();

        // StackCore interface
      public:
        virtual void pushView(std::shared_ptr<View> view, String title) override;
        virtual void popView() override;
        virtual std::list<std::shared_ptr<View>> childViews() override;

        // IParentViewCore interface
      public:
        virtual void addChildCore(ViewCore *child) override;
        virtual void removeChildCore(ViewCore *view) override;
        virtual double getUIScaleFactor() const override;

        // IAndroidNavigationButtonHandler interface
      public:
        virtual bool handleBackButton() override;

      private:
        void updateCurrentView();
        std::shared_ptr<Stack> getStack() const;
        void reLayout();

      private:
        std::shared_ptr<View> _currentView;
        std::shared_ptr<FixedView> _container;
        std::shared_ptr<NavButtonHandler> _navHandler;
    };
}
