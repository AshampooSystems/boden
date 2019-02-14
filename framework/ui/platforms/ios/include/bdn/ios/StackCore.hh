#pragma once

#import <bdn/ios/ViewCore.hh>
#include <bdn/StackCore.h>
#include <bdn/Stack.h>

namespace bdn
{
    namespace ios
    {
        class StackCore : public ViewCore, virtual public bdn::StackCore
        {
          private:
            StackCore(std::shared_ptr<Stack> outerStack, UINavigationController *navigationController);

          public:
            StackCore(std::shared_ptr<Stack> outerStack);

          public:
            virtual void layout() override;
            virtual Size calcPreferredSize(const Size &availableSpace) const override;

          public:
            virtual void pushView(std::shared_ptr<View> view, String title) override;
            virtual void popView() override;

          private:
            std::shared_ptr<bdn::Stack> stack()
            {
                return std::static_pointer_cast<bdn::Stack>(getOuterViewIfStillAttached());
            }

          private:
            UINavigationController *_navigationController;
            std::shared_ptr<View> _currentView;
        };
    }
}
