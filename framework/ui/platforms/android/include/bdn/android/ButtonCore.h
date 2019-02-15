#pragma once

#include <bdn/Button.h>
#include <bdn/IButtonCore.h>
#include <bdn/android/JButton.h>
#include <bdn/android/ViewCore.h>

namespace bdn
{
    namespace android
    {

        class ButtonCore : public ViewCore, virtual public IButtonCore
        {
          public:
            ButtonCore(std::shared_ptr<Button> outerButton);

            JButton &getJButton();

            void setLabel(const String &label) override;

            void clicked() override;

          protected:
            double getFontSizeDips() const override;

          private:
            mutable JButton _jButton;
        };
    }
}
