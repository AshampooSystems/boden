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
        ButtonCore(const ContextWrapper &ctxt);

        wrapper::Button &getJButton();

        void clicked() override;

      protected:
        double getFontSizeDips() const override;

      private:
        mutable wrapper::Button _jButton;
    };
}
