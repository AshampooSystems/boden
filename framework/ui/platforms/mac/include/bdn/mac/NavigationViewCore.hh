#pragma once

#import <bdn/mac/ViewCore.hh>
#include <bdn/ui/NavigationView.h>
#include <deque>
#include <memory>

@class BdnBackButtonClickHandler;

namespace bdn::ui::mac
{
    class NavigationViewCore : public ViewCore, public NavigationView::Core
    {
      public:
        NavigationViewCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory);
        ~NavigationViewCore();

        void init() override;

      public:
        void pushView(std::shared_ptr<View> view, std::string title) override;
        void popView() override;
        std::vector<std::shared_ptr<View>> childViews() const override;

      public:
        void setLayout(std::shared_ptr<Layout> layout) override;

      private:
        void updateCurrentView();
        void reLayout();

      private:
        NSView *_navigationBar;
        NSButton *_backButton;
        NSTextField *_title;

        std::shared_ptr<View> _currentView;
        std::shared_ptr<ContainerView> _container;

        struct StackEntry
        {
            std::shared_ptr<View> view;
            std::string title;
        };

        std::deque<StackEntry> _stack;

        BdnBackButtonClickHandler *_backButtonClickHandler;
    };
}
