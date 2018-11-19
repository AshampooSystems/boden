#ifndef BDN_ANDROID_CheckboxCore_H_
#define BDN_ANDROID_CheckboxCore_H_

#include <bdn/android/ViewCore.h>
#include <bdn/android/JCheckBox.h>
#include <bdn/ICheckboxCore.h>
#include <bdn/Checkbox.h>
#include <bdn/Toggle.h>

namespace bdn
{
    namespace android
    {

        template <class T> class CheckboxCore : public ViewCore, BDN_IMPLEMENTS ICheckboxCore
        {
          private:
            static P<JCheckBox> _createJCheckBox(T *outer)
            {
                // we need to know the context to create the view.
                // If we have a parent then we can get that from the parent's
                // core.
                P<View> parent = outer->getParentView();
                if (parent == nullptr)
                    throw ProgrammingError("CheckboxCore instance requested for a Checkbox that "
                                           "does not have a parent.");

                P<ViewCore> parentCore = cast<ViewCore>(parent->getViewCore());
                if (parentCore == nullptr)
                    throw ProgrammingError("CheckboxCore instance requested for a Checkbox with "
                                           "core-less parent.");

                JContext context = parentCore->getJView().getContext();

                return newObj<JCheckBox>(context);
            }

          public:
            CheckboxCore(T *outer) : ViewCore(outer, _createJCheckBox(outer))
            {
                _jCheckBox = cast<JCheckBox>(&getJView());

                _jCheckBox->setSingleLine(true);

                setLabel(outer->label());
                setState(outer->state());
            }

            JCheckBox &getJCheckBox() { return *_jCheckBox; }

            void setLabel(const String &label) override
            {
                _jCheckBox->setText(label);

                // we must re-layout the button - otherwise its preferred size
                // is not updated.
                _jCheckBox->requestLayout();
            }

            void setState(const TriState &state) override
            {
                _jCheckBox->setChecked(state == TriState::on);
                _state = state;
            }

            TriState getState() const { return _state; }

            void clicked() override
            {
                P<T> view = cast<T>(getOuterViewIfStillAttached());
                if (view != nullptr) {
                    ClickEvent evt(view);

                    P<Checkbox> checkbox = tryCast<Checkbox>(view);
                    P<Toggle> toggle = tryCast<Toggle>(view);

                    // User interaction cannot set the checkbox into mixed state
                    _state = _jCheckBox->isChecked() ? TriState::on : TriState::off;

                    if (checkbox)
                        checkbox->setState(_state);
                    else if (toggle)
                        toggle->setOn(_jCheckBox->isChecked());

                    view->onClick().notify(evt);
                }
            }

          protected:
            double getFontSizeDips() const override
            {
                // the text size is in pixels
                return _jCheckBox->getTextSize() / getUiScaleFactor();
            }

          private:
            P<JCheckBox> _jCheckBox;
            TriState _state;
        };
    }
}

#endif
