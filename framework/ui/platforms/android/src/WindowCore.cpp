
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
            title.onChange() += [=](auto va) {
                JNativeRootView rootView(getRootViewRegistryForCurrentThread().getNewestValidRootView());

                rootView.setTitle(va->get());
            };

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

        void WindowCore::initTag()
        {
            ViewCore::initTag();

            JNativeRootView rootView(getJView().getParent().getRef_());
            rootView.setTag(bdn::java::NativeWeakPointer(getOuterViewIfStillAttached()));
        }

        void WindowCore::enableBackButton(bool enable)
        {
            JNativeRootView rootView(getRootViewRegistryForCurrentThread().getNewestValidRootView());
            rootView.enableBackButton(enable);
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

            auto windowCoreList = getWindowCoreListFromRootView(javaRef);

            for (auto windowCore : windowCoreList)
                windowCore->rootViewDisposed();
        }

        void WindowCore::_rootViewSizeChanged(const java::Reference &javaRef, int width, int height)
        {
            auto windowCoreList = getWindowCoreListFromRootView(javaRef);

            for (auto windowCore : windowCoreList)
                windowCore->rootViewSizeChanged(width, height);
        }

        void WindowCore::_rootViewConfigurationChanged(const java::Reference &javaRef, JConfiguration config)
        {
            auto windowCoreList = getWindowCoreListFromRootView(javaRef);

            for (auto windowCore : windowCoreList)
                windowCore->rootViewConfigurationChanged(config);
        }

        bool WindowCore::_handleBackPressed(const java::Reference &javaRef)
        {
            auto windowCoreList = getWindowCoreListFromRootView(javaRef);

            for (auto windowCore : windowCoreList) {
                if (windowCore->handleBackPressed()) {
                    return true;
                }
            }

            return false;
        }

        void WindowCore::addChildCore(ViewCore *child)
        {
            JNativeViewGroup parentGroup(getJView().getRef_());

            parentGroup.addView(child->getJView());
        }

        void WindowCore::removeChildCore(ViewCore *child)
        {
            JNativeViewGroup parentGroup(getJView().getRef_());
            parentGroup.removeView(child->getJView());
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

            contentGeometry = _currentBounds;
            geometry = _currentBounds;
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

        std::list<std::shared_ptr<WindowCore>>
        WindowCore::getWindowCoreListFromRootView(const java::Reference &javaRootViewRef)
        {
            auto result = std::list<std::shared_ptr<WindowCore>>{};

            JNativeRootView rootView(javaRootViewRef);

            // enumerate all children of the root view. Those are our
            // "window" views.
            int childCount = rootView.getChildCount();
            for (int i = 0; i < childCount; i++) {
                JView child = rootView.getChildAt(i);

                if (auto windowCore =
                        std::dynamic_pointer_cast<WindowCore>(getViewCoreFromJavaViewRef(child.getRef_()))) {
                    result.push_back(windowCore);
                }
            }

            return result;
        }

        java::Reference WindowCore::tryGetAccessibleRootViewRef() const
        {
            java::Reference accessibleRef;

            {
                std::unique_lock lock(_rootViewMutex);

                accessibleRef = _weakRootViewRef.toStrong();

                if (accessibleRef.isNull()) {
                    const_cast<WindowCore *>(this)->rootViewDisposed();
                }
            }

            return accessibleRef;
        }

        void WindowCore::scheduleLayout() { getJView().requestLayout(); }
    }
}
