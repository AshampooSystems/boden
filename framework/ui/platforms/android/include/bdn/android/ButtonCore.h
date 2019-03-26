#pragma once

#include <bdn/Button.h>
#include <bdn/ButtonCore.h>
#include <bdn/android/ViewCore.h>
#include <bdn/android/wrapper/Button.h>

namespace bdn::android
{

    class ButtonCore : public ViewCore, virtual public bdn::ButtonCore
    {
      public:
        ButtonCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory);

        wrapper::Button &getJButton();

        void clicked() override;

      private:
        mutable wrapper::Button _jButton;
    };
}
