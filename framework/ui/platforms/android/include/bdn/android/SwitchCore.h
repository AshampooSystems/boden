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
            static P<JSwitch> _createJSwitch(T *pOuter)
            {
                // we need to know the context to create the view.
                // If we have a parent then we can get that from the parent's
                // core.
                P<View> pParent = pOuter->getParentView();
                if (pParent == nullptr)
                    throw ProgrammingError("SwitchCore instance requested for a Switch that does "
                                           "not have a parent.");

                P<ViewCore> pParentCore = cast<ViewCore>(pParent->getViewCore());
                if (pParentCore == nullptr)
                    throw ProgrammingError("SwitchCore instance requested for "
                                           "a Switch with core-less parent.");

                JContext context = pParentCore->getJView().getContext();

                return newObj<JSwitch>(context);
            }

          public:
            SwitchCore(T *pOuter) : ViewCore(pOuter, _createJSwitch(pOuter))
            {
                _pJSwitch = cast<JSwitch>(&getJView());

                _pJSwitch->setSingleLine(true);

                setLabel(pOuter->label());
                setOn(pOuter->on());
            }

            JSwitch &getJSwitch() { return *_pJSwitch; }

            void setLabel(const String &label) override
            {
                _pJSwitch->setText(label);

                // we must re-layout the button - otherwise its preferred size
                // is not updated.
                _pJSwitch->requestLayout();
            }

            void setOn(const bool &on) override { _pJSwitch->setChecked(on); }

            void clicked() override
            {
                P<T> pView = cast<T>(getOuterViewIfStillAttached());
                if (pView != nullptr) {
                    ClickEvent evt(pView);

                    pView->setOn(_pJSwitch->isChecked());
                    pView->onClick().notify(evt);
                }
            }

          protected:
            double getFontSizeDips() const override
            {
                // the text size is in pixels
                return _pJSwitch->getTextSize() / getUiScaleFactor();
            }

          private:
            P<JSwitch> _pJSwitch;
        };
    }
}

#endif
