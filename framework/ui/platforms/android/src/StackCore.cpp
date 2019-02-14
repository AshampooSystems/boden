#include <bdn/android/StackCore.h>
#include <bdn/android/JNativeViewGroup.h>
#include <bdn/Stack.h>

namespace bdn
{
    namespace android
    {

        std::shared_ptr<WindowCore> findWindow(std::shared_ptr<View> view)
        {

            if (auto windowCore = std::dynamic_pointer_cast<WindowCore>(view->getViewCore())) {
                return windowCore;
            }

            if (auto parent = view->getParentView()) {
                return findWindow(parent);
            }

            return nullptr;
        }

        StackCore::StackCore(std::shared_ptr<Stack> outerStack)
            : ViewCore(outerStack, ViewCore::createAndroidViewClass<JNativeViewGroup>(outerStack))
        {}

        StackCore::~StackCore() {}

        void StackCore::layout()
        {
            auto outerStack = getStack();
            if (!outerStack->stack().empty()) {
                Rect rChild = outerStack->stack().back().view->adjustAndSetBounds(
                    Rect{0, 0, outerStack->size->width, outerStack->size->height});
            }
        }

        void StackCore::pushView(std::shared_ptr<View> view, String title) { updateCurrentView(); }

        void StackCore::popView() { updateCurrentView(); }

        std::shared_ptr<Stack> StackCore::getStack() const
        {
            return std::static_pointer_cast<Stack>(getOuterViewIfStillAttached());
        }

        void StackCore::updateCurrentView()
        {
            if (auto outerStack = getStack()) {
                if (_currentView) {
                    _currentView->_setParentView(nullptr);
                }

                if (!outerStack->stack().empty()) {
                    auto newView = outerStack->stack().back().view;

                    newView->_setParentView(outerStack);

                    _currentView = newView;

                    auto windowCore = findWindow(outerStack);

                    windowCore->setAndroidNavigationButtonHandler(
                        std::dynamic_pointer_cast<WindowCore::IAndroidNavigationButtonHandler>(shared_from_this()));

                    windowCore->setTitle(outerStack->stack().back().title);

                    windowCore->enableBackButton(outerStack->stack().size() > 1);
                }
            }
        }

        void StackCore::addChildJView(JView view) { ViewCore::getJViewAS<JNativeViewGroup>().addView(view); }

        void StackCore::removeChildJView(JView view) { ViewCore::getJViewAS<JNativeViewGroup>().removeView(view); }

        double StackCore::getUIScaleFactor() const { return ViewCore::getUIScaleFactor(); }

        bool StackCore::handleBackButton()
        {
            if (auto outerStack = getStack()) {
                if (outerStack->stack().size() > 1) {
                    outerStack->popView();
                    return true;
                }
            }

            return false;
        }
    }
}
