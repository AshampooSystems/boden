#pragma once

#include <bdn/ClickEvent.h>
#include <bdn/IButtonCore.h>
#include <bdn/SimpleNotifier.h>
#include <bdn/View.h>

namespace bdn
{

    /** A simple button with a text label.*/
    class Button : public View
    {
      public:
        Property<String> label;

      public:
        Button()
        {
            _onClick = std::make_shared<SimpleNotifier<const ClickEvent &>>();

            label.onChange() += View::CorePropertyUpdater<String, IButtonCore>{this, &IButtonCore::setLabel};
        }

        ISyncNotifier<const ClickEvent &> &onClick() { return *_onClick; }

        String viewCoreTypeName() const override { return coreTypeName; }

      public:
        static constexpr char coreTypeName[] = "bdn.ButtonCore";

      private:
        std::shared_ptr<SimpleNotifier<const ClickEvent &>> _onClick;
    };
}
