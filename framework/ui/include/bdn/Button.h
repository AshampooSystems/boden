#ifndef BDN_Button_H_
#define BDN_Button_H_

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
        Button() { _pOnClick = newObj<SimpleNotifier<const ClickEvent &>>(); }

        /** The button's label.*/
        BDN_VIEW_PROPERTY(String, label, setLabel, IButtonCore,
                          influencesPreferredSize());

        ISyncNotifier<const ClickEvent &> &onClick() { return *_pOnClick; }

        /** Static function that returns the type name for #Button objects.*/
        static String getButtonCoreTypeName() { return "bdn.ButtonCore"; }

        String getCoreTypeName() const override
        {
            return getButtonCoreTypeName();
        }

      private:
        P<SimpleNotifier<const ClickEvent &>> _pOnClick;
    };
}

#endif
