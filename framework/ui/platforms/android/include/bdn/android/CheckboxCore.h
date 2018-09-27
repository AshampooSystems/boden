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

        template <class T>
        class CheckboxCore : public ViewCore, BDN_IMPLEMENTS ICheckboxCore
        {
          private:
            static P<JCheckBox> _createJCheckBox(T *pOuter)
            {
                // we need to know the context to create the view.
                // If we have a parent then we can get that from the parent's
                // core.
                P<View> pParent = pOuter->getParentView();
                if (pParent == nullptr)
                    throw ProgrammingError(
                        "CheckboxCore instance requested for a Checkbox that "
                        "does not have a parent.");

                P<ViewCore> pParentCore =
                    cast<ViewCore>(pParent->getViewCore());
                if (pParentCore == nullptr)
                    throw ProgrammingError(
                        "CheckboxCore instance requested for a Checkbox with "
                        "core-less parent.");

                JContext context = pParentCore->getJView().getContext();

                return newObj<JCheckBox>(context);
            }

          public:
            CheckboxCore(T *pOuter) : ViewCore(pOuter, _createJCheckBox(pOuter))
            {
                _pJCheckBox = cast<JCheckBox>(&getJView());

                _pJCheckBox->setSingleLine(true);

                setLabel(pOuter->label());
                setState(pOuter->state());
            }

            JCheckBox &getJCheckBox() { return *_pJCheckBox; }

            void setLabel(const String &label) override
            {
                _pJCheckBox->setText(label);

                // we must re-layout the button - otherwise its preferred size
                // is not updated.
                _pJCheckBox->requestLayout();
            }

            void setState(const TriState &state) override
            {
                _pJCheckBox->setChecked(state == TriState::on);
                _state = state;
            }

            TriState getState() const { return _state; }

            void clicked() override
            {
                P<T> pView = cast<T>(getOuterViewIfStillAttached());
                if (pView != nullptr) {
                    ClickEvent evt(pView);

                    P<Checkbox> pCheckbox = tryCast<Checkbox>(pView);
                    P<Toggle> pToggle = tryCast<Toggle>(pView);

                    // User interaction cannot set the checkbox into mixed state
                    _state =
                        _pJCheckBox->isChecked() ? TriState::on : TriState::off;

                    if (pCheckbox)
                        pCheckbox->setState(_state);
                    else if (pToggle)
                        pToggle->setOn(_pJCheckBox->isChecked());

                    pView->onClick().notify(evt);
                }
            }

          protected:
            double getFontSizeDips() const override
            {
                // the text size is in pixels
                return _pJCheckBox->getTextSize() / getUiScaleFactor();
            }

          private:
            P<JCheckBox> _pJCheckBox;
            TriState _state;
        };
    }
}

#endif
