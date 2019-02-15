
#include "../include/bdn/android/JNativeRootView.h"
#include <bdn/android/WindowCore.h>

namespace bdn
{
    namespace android
    {

        JView WindowCore::createJNativeViewGroup(std::shared_ptr<Window> outerWindow)
        {
            // we need a context to create our view object.
            // To know the context we first have to determine the root view
            // that this window will be connected to.

            // We connect to the first root view that is available in the
            // root view registry.
            JNativeRootView rootView(getRootViewRegistryForCurrentThread().getNewestValidRootView());

            if (rootView.isNull_())
                throw ProgrammingError("WindowCore being created but there are no native root "
                                       "views available. You must create a NativeRootActivity "
                                       "or NativeRootView instance!");

            JNativeViewGroup viewGroup(rootView.getContext());

            JView view(viewGroup.getRef_());

            // add the view group to the root view. That is important so
            // that the root view we have chosen is fixed to the view group
            // instance.
            rootView.addView(view);

            return view;
        }

        WindowCore::WindowCore(std::shared_ptr<Window> outerWindow)
            : ViewCore(outerWindow, createJNativeViewGroup(outerWindow))
        {
            setTitle(outerWindow->title);

            JNativeRootView rootView(getJView().getParent().getRef_());

            _weakRootViewRef = bdn::java::WeakReference(rootView.getRef_());

            updateUIScaleFactor(rootView.getContext().getResources().getConfiguration());

            rootViewSizeChanged(rootView.getWidth(), rootView.getHeight());
        }

        WindowCore::~WindowCore()
        {
            JView view(getJView());
            if (!view.isNull_()) {
                // remove the view from its parent.
                JViewGroup parent(view.getParent().getRef_());
                if (!parent.isNull_()) {
                    parent.removeView(view);
                }
            }
        }

        void WindowCore::setTitle(const String &title)
        {
            JNativeRootView rootView(getRootViewRegistryForCurrentThread().getNewestValidRootView());

            rootView.setTitle(title);
        }

        void WindowCore::enableBackButton(bool enable)
        {
            JNativeRootView rootView(getRootViewRegistryForCurrentThread().getNewestValidRootView());
            rootView.enableBackButton(enable);
        }

        Rect WindowCore::adjustAndSetBounds(const Rect &requestedBounds)
        {
            // we cannot influence our bounds. So "adjust" the bounds to the
            // view's current bounds
            return _currentBounds;
        }

        Rect WindowCore::adjustBounds(const Rect &requestedBounds, RoundType positionRoundType,
                                      RoundType sizeRoundType) const
        {
            // we cannot influence our bounds. So "adjust" the bounds to the
            // view's current bounds
            return _currentBounds;
        }

        void WindowCore::setVisible(const bool &visible) { ViewCore::setVisible(visible); }

        void WindowCore::invalidateSizingInfo(View::InvalidateReason reason)
        {
            // nothing to do since we do not cache sizing info in the core.
        }

        void WindowCore::needLayout(View::InvalidateReason reason)
        {
            std::shared_ptr<View> outerView = getOuterViewIfStillAttached();
            if (outerView != nullptr) {
                std::shared_ptr<UIProvider> provider =
                    std::dynamic_pointer_cast<UIProvider>(outerView->getUIProvider());
                if (provider != nullptr)
                    provider->getLayoutCoordinator()->viewNeedsLayout(outerView);
            }
        }

        void WindowCore::childSizingInfoInvalidated(std::shared_ptr<View> child)
        {
            std::shared_ptr<View> outerView = getOuterViewIfStillAttached();
            if (outerView != nullptr) {
                outerView->invalidateSizingInfo(View::InvalidateReason::childSizingInfoInvalidated);
                outerView->needLayout(View::InvalidateReason::childSizingInfoInvalidated);
            }
        }

        Size WindowCore::calcPreferredSize(const Size &availableSpace) const
        {
            std::shared_ptr<Window> window = std::dynamic_pointer_cast<Window>(getOuterViewIfStillAttached());
            if (window != nullptr)
                return defaultWindowCalcPreferredSizeImpl(window, availableSpace, Margin(), Size());
            else
                return Size(0, 0);
        }

        void WindowCore::layout()
        {
            std::shared_ptr<Window> window = std::dynamic_pointer_cast<Window>(getOuterViewIfStillAttached());
            if (window != nullptr)
                defaultWindowLayoutImpl(window, getContentArea());
        }

        void WindowCore::requestAutoSize()
        {
            std::shared_ptr<Window> window = std::dynamic_pointer_cast<Window>(getOuterViewIfStillAttached());
            if (window != nullptr) {
                std::shared_ptr<UIProvider> provider = std::dynamic_pointer_cast<UIProvider>(window->getUIProvider());
                if (provider != nullptr)
                    provider->getLayoutCoordinator()->windowNeedsAutoSizing(window);
            }
        }

        void WindowCore::requestCenter()
        {
            std::shared_ptr<Window> window = std::dynamic_pointer_cast<Window>(getOuterViewIfStillAttached());
            if (window != nullptr) {
                std::shared_ptr<UIProvider> provider = std::dynamic_pointer_cast<UIProvider>(window->getUIProvider());
                if (provider != nullptr)
                    provider->getLayoutCoordinator()->windowNeedsCentering(window);
            }
        }

        void WindowCore::autoSize()
        {
            // we cannot change our size. So, do nothing
        }

        void WindowCore::center()
        {
            // we cannot change our position. So, do nothing.
        }

        void WindowCore::_rootViewCreated(const java::Reference &javaRef)
        {
            // we store only a weak referene in the registry. We do not want
            // to keep the java-side root view object alive.
            getRootViewRegistryForCurrentThread().add(bdn::java::WeakReference(javaRef));
        }

        void WindowCore::_rootViewDisposed(const java::Reference &javaRef)
        {
            // this may be called by the garbage collector, so it might be
            // in an arbitrary thread. That means that the root view
            // registry we get might not be the one that actually owns the
            // reference. In that case the object will not be removed from
            // the registry. But that is OK: the registry will notice the
            // next time it tries to access the root view and then it will
            // be removed.
            getRootViewRegistryForCurrentThread().remove(javaRef);

            std::list<std::shared_ptr<WindowCore>> windowCoreList;
            getWindowCoreListFromRootView(javaRef, windowCoreList);

            for (std::shared_ptr<WindowCore> &windowCore : windowCoreList)
                windowCore->rootViewDisposed();
        }

        void WindowCore::_rootViewSizeChanged(const java::Reference &javaRef, int width, int height)
        {
            std::list<std::shared_ptr<WindowCore>> windowCoreList;

            getWindowCoreListFromRootView(javaRef, windowCoreList);

            for (std::shared_ptr<WindowCore> &windowCore : windowCoreList)
                windowCore->rootViewSizeChanged(width, height);
        }

        void WindowCore::_rootViewConfigurationChanged(const java::Reference &javaRef, JConfiguration config)
        {
            std::list<std::shared_ptr<WindowCore>> windowCoreList;

            getWindowCoreListFromRootView(javaRef, windowCoreList);

            for (std::shared_ptr<WindowCore> &windowCore : windowCoreList)
                windowCore->rootViewConfigurationChanged(config);
        }

        bool WindowCore::_handleBackPressed(const java::Reference &javaRef)
        {
            std::list<std::shared_ptr<WindowCore>> windowCoreList;

            getWindowCoreListFromRootView(javaRef, windowCoreList);

            for (std::shared_ptr<WindowCore> &windowCore : windowCoreList) {
                if (windowCore->handleBackPressed()) {
                    return true;
                }
            }

            return false;
        }

        void WindowCore::addChildJView(JView childJView)
        {
            JNativeViewGroup parentGroup(getJView().getRef_());

            parentGroup.addView(childJView);
        }

        void WindowCore::removeChildJView(JView childJView)
        {
            JNativeViewGroup parentGroup(getJView().getRef_());
            parentGroup.removeView(childJView);
        }

        void WindowCore::setAndroidNavigationButtonHandler(
            std::shared_ptr<WindowCore::IAndroidNavigationButtonHandler> handler)
        {
            _navButtonHandler = handler;
        }

        Rect WindowCore::getContentArea()
        {
            // content area = bounds (there are no borders)
            return _currentBounds;
        }

        void WindowCore::rootViewDisposed()
        {
            // this may be called by the garbage collector, so it might be
            // in an arbitrary thread

            std::unique_lock lock(_rootViewMutex);

            _weakRootViewRef = bdn::java::WeakReference();
        }

        void WindowCore::rootViewSizeChanged(int width, int height)
        {
            // logInfo("rootViewSizeChanged("+std::to_string(width)+"x"+std::to_string(height));

            // set our container view to the same size as the root.
            // Note that this is necessary because the root view does not
            // have a bdn::View associated with it. So there is not
            // automatic layout happening.
            JNativeRootView rootView(getJView().getParent().getRef_());
            rootView.setChildBounds(getJView(), 0, 0, width, height);
            rootView.requestLayout();

            double scaleFactor = getUIScaleFactor();

            _currentBounds = Rect(0, 0, width / scaleFactor, height / scaleFactor);

            std::shared_ptr<View> view = getOuterViewIfStillAttached();
            if (view != nullptr)
                view->adjustAndSetBounds(_currentBounds);
        }

        void WindowCore::rootViewConfigurationChanged(JConfiguration config) { updateUIScaleFactor(config); }

        void WindowCore::attachedToNewRootView(const java::Reference &javaRef)
        {
            // set the window's bounds to fill the root view completely.
            JNativeRootView rootView(javaRef);

            JConfiguration config(rootView.getContext().getResources().getConfiguration());

            updateUIScaleFactor(config);

            rootViewSizeChanged(rootView.getWidth(), rootView.getHeight());
        }

        bool WindowCore::handleBackPressed()
        {
            if (_navButtonHandler && _navButtonHandler->handleBackButton()) {
                return true;
            }

            return false;
        }

        Rect WindowCore::getScreenWorkArea() const
        {
            JNativeRootView rootView(tryGetAccessibleRootViewRef());

            if (rootView.isNull_()) {
                // don't have a root view => work area size is 0
                return Rect();
            } else {
                int width = rootView.getWidth();
                int height = rootView.getHeight();

                // logInfo("screen area:
                // ("+std::to_string(width)+"x"+std::to_string(height)+")");

                return Rect(0, 0, width, height);
            }
        }

        void WindowCore::updateUIScaleFactor(JConfiguration config)
        {
            int dpi = config.densityDpi();

            // the scale factor is at 1 for 160 dpi. Note that smaller DPI
            // values are also possible, so the scale factor can be <1 as
            // well.

            double scaleFactor = dpi / 160.0;

            setUIScaleFactor(scaleFactor);
        }

        void WindowCore::getWindowCoreListFromRootView(const java::Reference &javaRootViewRef,
                                                       std::list<std::shared_ptr<WindowCore>> &windowCoreList)
        {
            JNativeRootView rootView(javaRootViewRef);

            // enumerate all children of the root view. Those are our
            // "window" views.
            int childCount = rootView.getChildCount();
            for (int i = 0; i < childCount; i++) {
                JView child = rootView.getChildAt(i);

                std::shared_ptr<WindowCore> windowCore =
                    std::dynamic_pointer_cast<WindowCore>(ViewCore::getViewCoreFromJavaViewRef(child.getRef_()));
                if (windowCore != nullptr)
                    windowCoreList.push_back(windowCore);
            }
        }

        java::Reference WindowCore::tryGetAccessibleRootViewRef() const
        {
            java::Reference accessibleRef;

            {
                std::unique_lock lock(_rootViewMutex);

                accessibleRef = _weakRootViewRef.toStrong();

                if (accessibleRef.isNull())
                    const_cast<WindowCore *>(this)->rootViewDisposed();
            }

            return accessibleRef;
        }
    }
}
