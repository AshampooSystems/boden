#pragma once

#include <bdn/android/ViewCore.h>
#include <bdn/android/JCheckBox.h>
#include <bdn/ICheckboxCore.h>
#include <bdn/Checkbox.h>
#include <bdn/Toggle.h>

namespace bdn
{
    namespace android
    {

        template <class T> class CheckboxCore : public ViewCore, virtual public ICheckboxCore
        {
          private:
            static std::shared_ptr<JCheckBox> _createJCheckBox(std::shared_ptr<T> outer)
            {
                std::shared_ptr<View> parent = outer->getParentView();
                if (parent == nullptr)
                    throw ProgrammingError("CheckboxCore instance requested for a Checkbox that "
                                           "does not have a parent.");

                std::shared_ptr<ViewCore> parentCore = std::dynamic_pointer_cast<ViewCore>(parent->getViewCore());
                if (parentCore == nullptr)
                    throw ProgrammingError("CheckboxCore instance requested for a Checkbox with "
                                           "core-less parent.");

                JContext context = parentCore->getJView().getContext();

                return std::make_shared<JCheckBox>(context);
            }

          public:
            CheckboxCore(std::shared_ptr<T> outer) : ViewCore(outer, _createJCheckBox(outer))
            {
                _jCheckBox = std::dynamic_pointer_cast<JCheckBox>(getJViewPtr());

                _jCheckBox->setSingleLine(true);

                setLabel(outer->label);
                setState(outer->state);
            }

            JCheckBox &getJCheckBox() { return *_jCheckBox; }

            void setLabel(const String &label) override
            {
                _jCheckBox->setText(label);
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
                std::shared_ptr<T> view = std::dynamic_pointer_cast<T>(getOuterViewIfStillAttached());
                if (view != nullptr) {
                    ClickEvent evt(view);

                    std::shared_ptr<Checkbox> checkbox = std::dynamic_pointer_cast<Checkbox>(view);
                    std::shared_ptr<Toggle> toggle = std::dynamic_pointer_cast<Toggle>(view);

                    // User interaction cannot set the checkbox into mixed state
                    _state = _jCheckBox->isChecked() ? TriState::on : TriState::off;

                    if (checkbox)
                        checkbox->state = (_state);
                    else if (toggle)
                        toggle->on = (_jCheckBox->isChecked());

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
            std::shared_ptr<JCheckBox> _jCheckBox;
            TriState _state;
        };
    }
}
