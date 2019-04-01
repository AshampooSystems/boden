#pragma once

#include <bdn/FixedView.h>
#include <bdn/Stack.h>

#include <bdn/android/ContainerViewCore.h>
#include <bdn/android/WindowCore.h>

namespace bdn::android
{
    class NavButtonHandler;

    class StackCore : public ViewCore, public bdn::Stack::Core
    {
      public:
        StackCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory);
        ~StackCore() override;

        // StackCore interface
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
            std::shared_ptr<FixedView> container;
            std::shared_ptr<View> view;
            String title;
        };

        std::deque<StackEntry> _stack;
    };
}
