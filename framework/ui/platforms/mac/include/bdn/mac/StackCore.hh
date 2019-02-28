#pragma once

#import <bdn/mac/ChildViewCore.hh>

#include <bdn/Stack.h>
#include <bdn/StackCore.h>

#include <deque>

@class BdnBackButtonClickHandler;

namespace bdn
{
    namespace mac
    {
        class StackCore : public ChildViewCore, public bdn::StackCore
        {
          public:
            StackCore(std::shared_ptr<bdn::Stack> outerStack);
            virtual ~StackCore() = default;

          public:
            virtual void pushView(std::shared_ptr<View> view, String title) override;
            virtual void popView() override;
            virtual std::list<std::shared_ptr<View>> getChildViews() override;

          public:
          private:
            void updateCurrentView();
            std::shared_ptr<Stack> getStack() const;

            void reLayout();

          private:
            NSView *_navigationBar;
            NSButton *_backButton;
            NSTextField *_title;

            std::shared_ptr<View> _currentView;

            std::shared_ptr<FixedView> _container;

            BdnBackButtonClickHandler *_backButtonClickHandler;
        };
    }
}
