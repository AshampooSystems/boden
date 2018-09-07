#ifndef BDN_GTK_ButtonBase_H_
#define BDN_GTK_ButtonBase_H_

#include <bdn/gtk/ViewCore.h>

namespace bdn
{
    namespace gtk
    {

        class ButtonCoreBase : public ViewCore
        {
          public:
            ButtonCoreBase(View *pOuterView, GtkWidget *pWidget)
                : ViewCore(pOuterView, pWidget)
            {}

            void setLabel(const String &label)
            {
                gtk_button_set_label(GTK_BUTTON(getGtkWidget()),
                                     label.asUtf8Ptr());
            }
        };
    }
}

#endif