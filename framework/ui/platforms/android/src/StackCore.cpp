#include <bdn/Stack.h>
#include <bdn/android/JNativeViewGroup.h>
#include <bdn/android/StackCore.h>

namespace bdn
{
    namespace android
    {
        class NavButtonHandler : public WindowCore::IAndroidNavigationButtonHandler
        {
          public:
            NavButtonHandler(StackCore *core) : _stackCore(core) {}
            virtual ~NavButtonHandler() = default;

            void unsetCore() { _stackCore = nullptr; }

            virtual bool handleBackButton() override
            {
                if (_stackCore) {
                    return _stackCore->handleBackButton();
                }
                return false;
            }

          private:
            StackCore *_stackCore;
        };

        std::shared_ptr<WindowCore> findWindow(std::shared_ptr<View> view)
        {
            if (auto windowCore = std::dynamic_pointer_cast<WindowCore>(view->viewCore())) {
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

        StackCore::~StackCore()
        {
            if (_navHandler) {
                _navHandler->unsetCore();
            }
        }

        void StackCore::pushView(std::shared_ptr<View> view, String title) { updateCurrentView(); }

        void StackCore::popView() { updateCurrentView(); }

        std::shared_ptr<Stack> StackCore::getStack() const { return std::static_pointer_cast<Stack>(outerView()); }

        std::list<std::shared_ptr<View>> StackCore::childViews()
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
                _container->_setParentView(outerView());
            }

            if (auto outerStack = getStack()) {
                if (_currentView) {
                    _container->removeAllChildViews();
                }

                if (!outerStack->stack().empty()) {
                    auto newView = outerStack->stack().back().view;

                    _container->addChildView(newView);

                    _currentView = newView;

                    if (auto windowCore = findWindow(outerStack)) {

                        if (!_navHandler) {
                            _navHandler = std::make_shared<NavButtonHandler>(this);
                            windowCore->setAndroidNavigationButtonHandler(_navHandler);
                        }

                        windowCore->title = (outerStack->stack().back().title);
                        windowCore->enableBackButton(outerStack->stack().size() > 1);
                    }
                }
            }

            reLayout();
        }

        void StackCore::addChildCore(ViewCore *child)
        {
            ViewCore::getJViewAS<JNativeViewGroup>().addView(child->getJView());
        }

        void StackCore::removeChildCore(ViewCore *child)
        {
            ViewCore::getJViewAS<JNativeViewGroup>().removeView(child->getJView());
        }

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
