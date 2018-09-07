#ifndef BDN_GTK_CheckboxCore_H
#define BDN_GTK_CheckboxCore_H

#include <bdn/gtk/ToggleCoreBase.h>
#include <bdn/IToggleCoreBase.h>
#include <bdn/Checkbox.h>

#include <gtk/gtk.h>

namespace bdn
{
    namespace gtk
    {

        template <class T>
        class CheckboxCore : public ToggleCoreBase,
                             BDN_IMPLEMENTS ICheckboxCore,
                             BDN_IMPLEMENTS ISwitchCore
        {
          public:
            CheckboxCore(T *pOuter)
                : ToggleCoreBase(pOuter, gtk_check_button_new())
            {
                setLabel(pOuter->label());
                setState(pOuter->state());

                g_signal_connect(getGtkWidget(), "toggled",
                                 G_CALLBACK(toggledCallback), this);
            }

            void setPadding(const Nullable<UiMargin> &uiPadding) override
            {
                // future: could use child-displacement-x or CSS padding
                // property in future. For now we do nothing with the padding at
                // this point. But we DO add it into the preferred size (see
                // ViewCore::_calcPreferredSize), so
            }

            void setLabel(const String &label) override
            {
                gtk_button_set_label(GTK_BUTTON(getGtkWidget()),
                                     label.asUtf8Ptr());
            }

            void setState(const TriState &state) override
            {
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(getGtkWidget()),
                                             state == TriState::on);
                gtk_toggle_button_set_inconsistent(
                    GTK_TOGGLE_BUTTON(getGtkWidget()),
                    state == TriState::mixed);
            }

            void setOn(const bool &on) override
            {
                setState(on ? TriState::on : TriState::off);
            }

            void notifyState()
            {
                P<View> pView = getOuterViewIfStillAttached();
                P<Toggle> pToggle = tryCast<Toggle>(pView);
                P<Checkbox> pCheckbox = tryCast<Checkbox>(pView);
                TriState newState = _gtkToggleButtonStateToCheckboxState();
                if (pCheckbox)
                    pCheckbox->setState(newState);
                else if (pToggle)
                    pToggle->setOn(newState == TriState::on);
            }

            void generateClick() override
            {
                P<View> pView = getOuterViewIfStillAttached();
                if (pView != nullptr) {
                    gtk_toggle_button_set_inconsistent(
                        GTK_TOGGLE_BUTTON(getGtkWidget()), false);

                    ClickEvent evt(pView);
                    cast<T>(pView)->onClick().notify(evt);
                }
            }

            TriState _gtkToggleButtonStateToCheckboxState()
            {
                TriState newCheckboxState = TriState::off;
                if (gtk_toggle_button_get_inconsistent(
                        GTK_TOGGLE_BUTTON(getGtkWidget()))) {
                    newCheckboxState = TriState::mixed;
                } else if (gtk_toggle_button_get_active(
                               GTK_TOGGLE_BUTTON(getGtkWidget()))) {
                    newCheckboxState = TriState::on;
                }
                return newCheckboxState;
            }

          protected:
            Margin getDefaultPaddingDips() const override
            {
                return uiMarginToDipMargin(
                    UiMargin(UiLength::sem(0.12), UiLength::sem(0.5)));
            }

            static void toggledCallback(GtkWidget *pWidget, gpointer pParam)
            {
                ((CheckboxCore *)pParam)->notifyState();
            }
        };
    }
}

#endif
