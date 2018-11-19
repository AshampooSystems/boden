#ifndef BDN_ANDROID_ButtonCore_H_
#define BDN_ANDROID_ButtonCore_H_

#include <bdn/android/ViewCore.h>
#include <bdn/android/JButton.h>
#include <bdn/IButtonCore.h>
#include <bdn/Button.h>

namespace bdn
{
    namespace android
    {

        class ButtonCore : public ViewCore, BDN_IMPLEMENTS IButtonCore
        {
          private:
            static P<JButton> _createJButton(Button *outer)
            {
                // we need to know the context to create the view.
                // If we have a parent then we can get that from the parent's
                // core.
                P<View> parent = outer->getParentView();
                if (parent == nullptr)
                    throw ProgrammingError("ButtonCore instance requested for a Button that does "
                                           "not have a parent.");

                P<ViewCore> parentCore = cast<ViewCore>(parent->getViewCore());
                if (parentCore == nullptr)
                    throw ProgrammingError("ButtonCore instance requested for "
                                           "a Button with core-less parent.");

                JContext context = parentCore->getJView().getContext();

                return newObj<JButton>(context);
            }

          public:
            ButtonCore(Button *outerButton) : ViewCore(outerButton, _createJButton(outerButton))
            {
                _jButton = cast<JButton>(&getJView());

                _jButton->setSingleLine(true);

                setLabel(outerButton->label());
            }

            JButton &getJButton() { return *_jButton; }

            void setLabel(const String &label) override
            {
                _jButton->setText(label);

                // we must re-layout the button - otherwise its preferred size
                // is not updated.
                _jButton->requestLayout();
            }

            void clicked() override
            {
                P<View> view = getOuterViewIfStillAttached();
                if (view != nullptr) {
                    ClickEvent evt(view);

                    cast<Button>(view)->onClick().notify(evt);
                }
            }

          protected:
            double getFontSizeDips() const override
            {
                // the text size is in pixels
                return _jButton->getTextSize() / getUiScaleFactor();
            }

          private:
            P<JButton> _jButton;
        };
    }
}

#endif
