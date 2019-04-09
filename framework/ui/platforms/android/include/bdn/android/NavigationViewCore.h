#pragma once

#include <bdn/NavigationView.h>

#include <bdn/android/ContainerViewCore.h>
#include <bdn/android/WindowCore.h>

namespace bdn::android
{
    class NavButtonHandler;

    class NavigationViewCore : public ViewCore, public bdn::NavigationView::Core
    {
      public:
        NavigationViewCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory);
        ~NavigationViewCore() override;

      public:
        void pushView(std::shared_ptr<View> view, String title) override;
        void popView() override;
        std::list<std::shared_ptr<View>> childViews() override;

      public:
        void visitInternalChildren(const std::function<void(std::shared_ptr<bdn::View::Core>)> &function) override;

        bool handleBackButton();

      private:
        void updateCurrentView(bool first, bool enter);
        void reLayout();

      private:
        struct StackEntry
        {
            std::shared_ptr<ContainerView> container;
            std::shared_ptr<View> view;
            String title;
        };

        std::deque<StackEntry> _stack;
    };
}
