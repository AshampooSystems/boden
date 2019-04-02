#pragma once

#include <bdn/Label.h>

#include <bdn/android/ViewCore.h>
#include <bdn/android/wrapper/RStyle.h>
#include <bdn/android/wrapper/TextView.h>

#include <limits>

namespace bdn::android
{
    class LabelCore : public ViewCore, virtual public bdn::Label::Core
    {
      public:
        LabelCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory);

        Size sizeForSpace(Size availableSpace = Size::none()) const override;

      protected:
        bool canAdjustWidthToAvailableSpace() const override { return false; }

      private:
        mutable wrapper::TextView _jTextView;
        bool _wrap = true;
    };
}
