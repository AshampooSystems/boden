#pragma once

#include <bdn/ui/Button.h>

#include <bdn/android/ViewCore.h>
#include <bdn/android/wrapper/NativeButton.h>

namespace bdn::ui::android
{
    class ButtonCore : public ViewCore, virtual public Button::Core
    {
      public:
        ButtonCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory);

        bdn::android::wrapper::NativeButton &getJButton();

        void clicked() override;

      private:
        void textChanged(const Text &text);
        void imageChanged(const String &url);

      private:
        mutable bdn::android::wrapper::NativeButton _jButton;
    };
}
