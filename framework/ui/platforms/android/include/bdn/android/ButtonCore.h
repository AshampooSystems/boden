#pragma once

#include <bdn/android/ViewCore.h>
#include <bdn/android/JButton.h>
#include <bdn/IButtonCore.h>
#include <bdn/Button.h>

namespace bdn
{
    namespace android
    {

        class ButtonCore : public ViewCore, virtual public IButtonCore
        {
          private:
            static std::shared_ptr<JButton> _createJButton(std::shared_ptr<Button> outer);

          public:
            ButtonCore(std::shared_ptr<Button> outerButton);

            JButton &getJButton();

            void setLabel(const String &label) override;

            void clicked() override;

          protected:
            double getFontSizeDips() const override;

          private:
            std::shared_ptr<JButton> _jButton;
        };
    }
}
