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

          public:
            virtual Size calcPreferredSize(const Size &availableSpace) const override;
            virtual void layout() override;

          private:
            void updateCurrentView();
            std::shared_ptr<Stack> getStack() const;

          private:
            NSView *_navigationBar;
            NSButton *_backButton;
            NSTextField *_title;
            NSView *_contentView;

            std::shared_ptr<View> _currentView;

            BdnBackButtonClickHandler *_backButtonClickHandler;
        };
    }
}
