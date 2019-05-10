#pragma once

#include <bdn/ui/Button.h>

#include <bdn/android/ViewCore.h>
#include <bdn/android/wrapper/Button.h>

namespace bdn::ui::android
{
    class ButtonCore : public ViewCore, virtual public Button::Core
    {
      public:
        ButtonCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory);

        bdn::android::wrapper::Button &getJButton();

        void clicked() override;

      private:
        mutable bdn::android::wrapper::Button _jButton;
    };
}
