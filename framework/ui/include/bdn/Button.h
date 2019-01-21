#pragma once

#include <bdn/View.h>
#include <bdn/IButtonCore.h>
#include <bdn/ClickEvent.h>
#include <bdn/SimpleNotifier.h>

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

            label.onChange() += View::CorePropertyUpdater<String, IButtonCore>{
                this, &IButtonCore::setLabel, [](auto &inf) { inf.influencesPreferredSize(); }};
        }

        ISyncNotifier<const ClickEvent &> &onClick() { return *_onClick; }

        /** Static function that returns the type name for #Button objects.*/
        static String getButtonCoreTypeName() { return "bdn.ButtonCore"; }

        String getCoreTypeName() const override { return getButtonCoreTypeName(); }

      private:
        std::shared_ptr<SimpleNotifier<const ClickEvent &>> _onClick;
    };
}
