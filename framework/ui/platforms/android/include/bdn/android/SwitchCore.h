#pragma once

#include <bdn/android/ViewCore.h>
#include <bdn/android/JSwitch.h>
#include <bdn/ISwitchCore.h>
#include <bdn/Switch.h>
#include <bdn/Toggle.h>
#include <bdn/android/JNativeViewCoreClickListener.h>

namespace bdn
{
    namespace android
    {

        template <class T> class SwitchCore : public ViewCore, virtual public ISwitchCore
        {
          private:
            static std::shared_ptr<JSwitch> _createJSwitch(std::shared_ptr<T> outer)
            {
                // we need to know the context to create the view.
                // If we have a parent then we can get that from the parent's
                // core.
                std::shared_ptr<View> parent = outer->getParentView();
                if (parent == nullptr)
                    throw ProgrammingError("SwitchCore instance requested for a Switch that does "
                                           "not have a parent.");

                std::shared_ptr<ViewCore> parentCore = std::dynamic_pointer_cast<ViewCore>(parent->getViewCore());
                if (parentCore == nullptr)
                    throw ProgrammingError("SwitchCore instance requested for "
                                           "a Switch with core-less parent.");

                JContext context = parentCore->getJView().getContext();

                return std::make_shared<JSwitch>(context);
            }

          public:
            SwitchCore(std::shared_ptr<T> outer) : ViewCore(outer, _createJSwitch(outer))
            {
                _jSwitch = std::dynamic_pointer_cast<JSwitch>(getJViewPtr());

                _jSwitch->setSingleLine(true);

                setLabel(outer->label);
                setOn(outer->on);

                bdn::android::JNativeViewCoreClickListener listener;
                _jSwitch->setOnClickListener(listener);                
            }

            JSwitch &getJSwitch() { return *_jSwitch; }

            void setLabel(const String &label) override
            {
                _jSwitch->setText(label);

                // we must re-layout the button - otherwise its preferred size
                // is not updated.
                _jSwitch->requestLayout();
            }

            void setOn(const bool &on) override { _jSwitch->setChecked(on); }

            void clicked() override
            {
                std::shared_ptr<T> view = std::dynamic_pointer_cast<T>(getOuterViewIfStillAttached());
                if (view != nullptr) {
                    ClickEvent evt(view);

                    view->on = (_jSwitch->isChecked());
                    view->onClick().notify(evt);
                }
            }

          protected:
            double getFontSizeDips() const override
            {
                // the text size is in pixels
                return _jSwitch->getTextSize() / getUiScaleFactor();
            }

          private:
            std::shared_ptr<JSwitch> _jSwitch;
        };
    }
}
