#ifndef BDN_ANDROID_SwitchCore_H_
#define BDN_ANDROID_SwitchCore_H_

#include <bdn/android/ViewCore.h>
#include <bdn/android/JSwitch.h>
#include <bdn/ISwitchCore.h>
#include <bdn/Switch.h>
#include <bdn/Toggle.h>

namespace bdn
{
    namespace android
    {

        template <class T> class SwitchCore : public ViewCore, BDN_IMPLEMENTS ISwitchCore
        {
          private:
            static P<JSwitch> _createJSwitch(T *outer)
            {
                // we need to know the context to create the view.
                // If we have a parent then we can get that from the parent's
                // core.
                P<View> parent = outer->getParentView();
                if (parent == nullptr)
                    throw ProgrammingError("SwitchCore instance requested for a Switch that does "
                                           "not have a parent.");

                P<ViewCore> parentCore = cast<ViewCore>(parent->getViewCore());
                if (parentCore == nullptr)
                    throw ProgrammingError("SwitchCore instance requested for "
                                           "a Switch with core-less parent.");

                JContext context = parentCore->getJView().getContext();

                return newObj<JSwitch>(context);
            }

          public:
            SwitchCore(T *outer) : ViewCore(outer, _createJSwitch(outer))
            {
                _jSwitch = cast<JSwitch>(&getJView());

                _jSwitch->setSingleLine(true);

                setLabel(outer->label());
                setOn(outer->on());
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
                P<T> view = cast<T>(getOuterViewIfStillAttached());
                if (view != nullptr) {
                    ClickEvent evt(view);

                    view->setOn(_jSwitch->isChecked());
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
            P<JSwitch> _jSwitch;
        };
    }
}

#endif
