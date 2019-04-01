#pragma once

#include <bdn/Button.h>

#include <bdn/android/ViewCore.h>
#include <bdn/android/wrapper/Button.h>

namespace bdn::android
{

    class ButtonCore : public ViewCore, virtual public bdn::Button::Core
    {
      public:
        ButtonCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory);

        wrapper::Button &getJButton();

        void clicked() override;

      private:
        mutable wrapper::Button _jButton;
    };
}
