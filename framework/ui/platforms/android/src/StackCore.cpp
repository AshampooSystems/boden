#include <bdn/Stack.h>
#include <bdn/android/JNativeViewGroup.h>
#include <bdn/android/StackCore.h>

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
            : ViewCore(outerStack, createAndroidViewClass<JNativeViewGroup>(outerStack))
        {
            geometry.onChange() += [=](auto va) { this->reLayout(); };
        }

        StackCore::~StackCore() {}

        void StackCore::pushView(std::shared_ptr<View> view, String title) { updateCurrentView(); }

        void StackCore::popView() { updateCurrentView(); }

        std::shared_ptr<Stack> StackCore::getStack() const
        {
            return std::static_pointer_cast<Stack>(getOuterViewIfStillAttached());
        }

        std::list<std::shared_ptr<View>> StackCore::getChildViews()
        {
            if (_container) {
                return {_container};
            }
            return {};
        }

        void StackCore::updateCurrentView()
        {
            if (!_container) {
                _container = std::make_shared<FixedView>();
                _container->_setParentView(getOuterViewIfStillAttached());
            }

            if (auto outerStack = getStack()) {
                if (_currentView) {
                    _container->removeAllChildViews();
                }

                if (!outerStack->stack().empty()) {
                    auto newView = outerStack->stack().back().view;

                    _container->addChildView(newView);

                    _currentView = newView;

                    auto windowCore = findWindow(outerStack);

                    windowCore->setAndroidNavigationButtonHandler(
                        std::dynamic_pointer_cast<WindowCore::IAndroidNavigationButtonHandler>(shared_from_this()));

                    windowCore->title = (outerStack->stack().back().title);

                    windowCore->enableBackButton(outerStack->stack().size() > 1);
                }
            }

            reLayout();
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

        void StackCore::reLayout()
        {
            if (_container) {
                _container->geometry = Rect{0, 0, geometry->width, geometry->height};
            }
        }
    }
}
