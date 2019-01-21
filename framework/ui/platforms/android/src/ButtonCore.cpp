
#include <bdn/android/ButtonCore.h>

namespace bdn
{
    namespace android
    {

        std::shared_ptr<JButton> ButtonCore::_createJButton(std::shared_ptr<Button> outer)
        {
            std::shared_ptr<View> parent = outer->getParentView();
            if (parent == nullptr)
                throw ProgrammingError("ButtonCore instance requested for a Button that does "
                                       "not have a parent.");

            std::shared_ptr<ViewCore> parentCore = std::dynamic_pointer_cast<ViewCore>(parent->getViewCore());
            if (parentCore == nullptr)
                throw ProgrammingError("ButtonCore instance requested for "
                                       "a Button with core-less parent.");

            JContext context = parentCore->getJView().getContext();

            return std::make_shared<JButton>(context);
        }

        ButtonCore::ButtonCore(std::shared_ptr<Button> outerButton) : ViewCore(outerButton, _createJButton(outerButton))
        {
            _jButton = std::dynamic_pointer_cast<JButton>(getJViewPtr());

            _jButton->setSingleLine(true);

            setLabel(outerButton->label);
        }

        JButton &ButtonCore::getJButton() { return *_jButton; }

        void ButtonCore::setLabel(const String &label)
        {
            _jButton->setText(label);
            _jButton->requestLayout();
        }

        void ButtonCore::clicked()
        {
            std::shared_ptr<View> view = getOuterViewIfStillAttached();
            if (view != nullptr) {
                ClickEvent evt(view);

                std::dynamic_pointer_cast<Button>(view)->onClick().notify(evt);
            }
        }

        double ButtonCore::getFontSizeDips() const
        {
            // the text size is in pixels
            return _jButton->getTextSize() / getUiScaleFactor();
        }
    }
}
