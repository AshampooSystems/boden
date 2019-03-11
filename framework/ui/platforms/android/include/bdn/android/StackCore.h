#pragma once

#include <bdn/FixedView.h>
#include <bdn/Stack.h>

#include <bdn/StackCore.h>
#include <bdn/android/ContainerViewCore.h>
#include <bdn/android/WindowCore.h>

namespace bdn::android
{
    class NavButtonHandler;

    class StackCore : public ViewCore, public bdn::StackCore
    {
      public:
        StackCore(const ContextWrapper &ctxt);
        virtual ~StackCore();

        // StackCore interface
      public:
        virtual void pushView(std::shared_ptr<View> view, String title) override;
        virtual void popView() override;
        virtual std::list<std::shared_ptr<View>> childViews() override;

      public:
        virtual void visitInternalChildren(std::function<void(std::shared_ptr<bdn::ViewCore>)> function) override;

        bool handleBackButton();

      private:
        void updateCurrentView();
        std::shared_ptr<Stack> getStack() const;
        void reLayout();

      private:
        std::shared_ptr<View> _currentView;
        std::shared_ptr<FixedView> _container;

        struct StackEntry
        {
            std::shared_ptr<View> view;
            String title;
        };

        std::deque<StackEntry> _stack;
    };
}
