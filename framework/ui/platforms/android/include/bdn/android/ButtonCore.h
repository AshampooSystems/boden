#pragma once

#include <bdn/Button.h>
#include <bdn/IButtonCore.h>
#include <bdn/android/ViewCore.h>
#include <bdn/android/wrapper/Button.h>

namespace bdn::android
{

    class ButtonCore : public ViewCore, virtual public IButtonCore
    {
      public:
        ButtonCore(std::shared_ptr<Button> outerButton);

        wrapper::Button &getJButton();

        void setLabel(const String &label) override;

        void clicked() override;

      protected:
        double getFontSizeDips() const override;

      private:
        mutable wrapper::Button _jButton;
    };
}
