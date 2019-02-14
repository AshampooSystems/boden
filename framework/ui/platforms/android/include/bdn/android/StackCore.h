#pragma once

#include <bdn/Stack.h>
#include <bdn/StackCore.h>
#include <bdn/android/ContainerViewCore.h>
#include <bdn/android/WindowCore.h>

namespace bdn
{
    namespace android
    {
        class StackCore : public ViewCore,
                          public bdn::StackCore,
                          public IParentViewCore,
                          public WindowCore::IAndroidNavigationButtonHandler
        {
          public:
            StackCore(std::shared_ptr<Stack> outerStack);
            virtual ~StackCore();

            // StackCore interface
          public:
            virtual void pushView(std::shared_ptr<View> view, String title) override;
            virtual void popView() override;

            // IViewCoreExtension interface
          public:
            virtual void layout() override;

            // IParentViewCore interface
          public:
            virtual void addChildJView(JView view) override;
            virtual void removeChildJView(JView view) override;
            virtual double getUIScaleFactor() const override;

            // IAndroidNavigationButtonHandler interface
          public:
            virtual bool handleBackButton() override;

          private:
            void updateCurrentView();
            std::shared_ptr<Stack> getStack() const;

          private:
            std::shared_ptr<View> _currentView;
        };
    }
}
