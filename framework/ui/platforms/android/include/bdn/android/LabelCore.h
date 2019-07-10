#pragma once

#include <bdn/ui/Label.h>

#include <bdn/android/ViewCore.h>
#include <bdn/android/wrapper/AppCompatTextView.h>
#include <bdn/android/wrapper/RStyle.h>

#include <limits>

namespace bdn::ui::android
{
    class LabelCore : public ViewCore, virtual public Label::Core
    {
      public:
        LabelCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory);

        Size sizeForSpace(Size availableSpace = Size::none()) const override;

      protected:
        bool canAdjustWidthToAvailableSpace() const override { return false; }
        void textChanged(const Text &text);

      private:
        mutable bdn::android::wrapper::TextView _jTextView;
        bool _wrap = true;
    };
}
