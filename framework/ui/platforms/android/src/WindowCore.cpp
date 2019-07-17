
#include <bdn/android/WindowCore.h>
#include <bdn/android/wrapper/NativeRootView.h>

#include <bdn/android/wrapper/Activity.h>
#include <bdn/android/wrapper/Configuration.h>
#include <utility>

namespace bdn::ui::detail
{
    CORE_REGISTER(Window, bdn::ui::android::WindowCore, Window)
}

namespace bdn::ui::android
{
    bdn::android::wrapper::View WindowCore::createJNativeViewGroup()
    {
        // we need a context to create our view object.
        // To know the context we first have to determine the root view
        // that this window will be connected to.

        // We connect to the first root view that is available in the
        // root view registry.
        bdn::android::wrapper::NativeRootView rootView(getRootViewRegistryForCurrentThread().getNewestValidRootView());

        if (rootView.isNull_()) {
            throw std::logic_error("WindowCore being created but there are no native root "
                                   "views available. You must create a NativeRootActivity "
                                   "or NativeRootView instance!");
        }

        bdn::android::wrapper::NativeViewGroup viewGroup(rootView.getContext());

        bdn::android::wrapper::View view(viewGroup.getRef_());

        // add the view group to the root view. That is important so
        // that the root view we have chosen is fixed to the view group
        // instance.
        rootView.addView(view);

        return view;
    }

    WindowCore::WindowCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory)
        : ViewCore(viewCoreFactory, createJNativeViewGroup())
    {}

    WindowCore::~WindowCore()
    {
        bdn::android::wrapper::View view(getJView());
        if (!view.isNull_()) {
            // remove the view from its parent.
            bdn::android::wrapper::ViewGroup parent(view.getParent().getRef_());
            if (!parent.isNull_()) {
                parent.removeView(view);
            }
        }
    }

    void WindowCore::init()
    {
        ViewCore::init();

        title.onChange() += [=](auto &property) {
            bdn::android::wrapper::NativeRootView rootView(getJView().getParent().getRef_());
            rootView.setTitle(property.get());
        };

        bdn::android::wrapper::NativeRootView rootView(getJView().getParent().getRef_());

        _weakRootViewRef = bdn::java::WeakReference(rootView.getRef_());

        updateUIScaleFactor(rootView.getContext().getResources().getConfiguration());
        updateOrientation(rootView.getContext().getResources().getConfiguration());

        rootViewSizeChanged(rootView.getWidth(), rootView.getHeight());

        contentView.onChange() += [=](auto &property) { updateContent(property.get()); };

        allowedOrientations.onChange() += [=](auto &property) {
            bdn::android::wrapper::Activity activity(getJView().getContext().getRef_());

            if (property.get() == Window::Core::Orientation::All) {
                activity.setRequestedOrientation(bdn::android::wrapper::ActivityInfo::SCREEN_ORIENTATION_FULL_USER);
            } else if (property.get() & Window::Core::Orientation::PortraitMask) {
                activity.setRequestedOrientation(bdn::android::wrapper::ActivityInfo::SCREEN_ORIENTATION_USER_PORTRAIT);
            } else if (property.get() & Window::Core::Orientation::LandscapeMask) {
                activity.setRequestedOrientation(
                    bdn::android::wrapper::ActivityInfo::SCREEN_ORIENTATION_USER_LANDSCAPE);
            }
        };
    }

    void WindowCore::initTag()
    {
        ViewCore::initTag();

        bdn::android::wrapper::NativeRootView rootView(getJView().getParent().getRef_());
        rootView.setTag(bdn::java::wrapper::NativeWeakPointer(shared_from_this()));
    }

    void WindowCore::enableBackButton(bool enable)
    {
        bdn::android::wrapper::NativeRootView rootView(getRootViewRegistryForCurrentThread().getNewestValidRootView());
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

        for (const auto &windowCore : windowCoreList) {
            windowCore->rootViewDisposed();
        }
    }

    void WindowCore::_rootViewSizeChanged(const java::Reference &javaRef, int width, int height)
    {
        auto windowCoreList = getWindowCoreListFromRootView(javaRef);

        for (const auto &windowCore : windowCoreList) {
            windowCore->rootViewSizeChanged(width, height);
        }
    }

    void WindowCore::_rootViewConfigurationChanged(const java::Reference &javaRef,
                                                   const bdn::android::wrapper::Configuration &config)
    {
        auto windowCoreList = getWindowCoreListFromRootView(javaRef);

        for (const auto &windowCore : windowCoreList) {
            windowCore->rootViewConfigurationChanged(config);
        }
    }

    bool WindowCore::_handleBackPressed(const java::Reference &javaRef)
    {
        auto windowCoreList = getWindowCoreListFromRootView(javaRef);

        for (const auto &windowCore : windowCoreList) {
            if (windowCore->handleBackPressed()) {
                return true;
            }
        }

        return false;
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
        bdn::android::wrapper::NativeRootView rootView(getJView().getParent().getRef_());
        rootView.setChildBounds(getJView(), 0, 0, width, height);
        rootView.requestLayout();

        double scaleFactor = getUIScaleFactor();

        _currentBounds = Rect(0, 0, width / scaleFactor, height / scaleFactor);

        contentGeometry = _currentBounds;
        geometry = _currentBounds;
    }

    void WindowCore::rootViewConfigurationChanged(const bdn::android::wrapper::Configuration &config)
    {
        updateUIScaleFactor(config);
        updateOrientation(config);
    }

    void WindowCore::attachedToNewRootView(const java::Reference &javaRef)
    {
        // set the window's bounds to fill the root view completely.
        bdn::android::wrapper::NativeRootView rootView(javaRef);

        bdn::android::wrapper::Configuration config(rootView.getContext().getResources().getConfiguration());

        updateUIScaleFactor(config);

        rootViewSizeChanged(rootView.getWidth(), rootView.getHeight());
    }

    bool WindowCore::handleBackPressed() { return false; }

    void WindowCore::updateContent(const std::shared_ptr<View> &view)
    {
        bdn::android::wrapper::NativeViewGroup parentGroup(getJView().getRef_());

        parentGroup.removeAllViews();

        if (view) {
            if (auto childCore = view->core<android::ViewCore>()) {
                parentGroup.addView(childCore->getJView());
            } else {
                throw std::runtime_error("Cannot set this type of View as content");
            }
        }

        updateChildren();
    }

    Rect WindowCore::getScreenWorkArea() const
    {
        bdn::android::wrapper::NativeRootView rootView(tryGetAccessibleRootViewRef());

        if (rootView.isNull_()) {
            // don't have a root view => work area size is 0
            return Rect();
        }
        int width = rootView.getWidth();
        int height = rootView.getHeight();

        // logInfo("screen area:
        // ("+std::to_string(width)+"x"+std::to_string(height)+")");

        return Rect(0, 0, width, height);
    }

    void WindowCore::updateUIScaleFactor(const bdn::android::wrapper::Configuration &config)
    {
        int dpi = config.densityDpi;

        // the scale factor is at 1 for 160 dpi. Note that smaller DPI
        // values are also possible, so the scale factor can be <1 as
        // well.

        double scaleFactor = dpi / 160.0;

        setUIScaleFactor(scaleFactor);
    }

    void WindowCore::updateOrientation(const bdn::android::wrapper::Configuration &config)
    {
        int orientation = config.orientation;
        if (orientation == bdn::android::wrapper::Configuration::ORIENTATION_PORTRAIT) {
            currentOrientation = WindowCore::Orientation::Portrait;
        } else if (orientation == bdn::android::wrapper::Configuration::ORIENTATION_LANDSCAPE) {
            currentOrientation = WindowCore::Orientation::LandscapeLeft;
        }
    }

    std::list<std::shared_ptr<WindowCore>>
    WindowCore::getWindowCoreListFromRootView(const java::Reference &javaRootViewRef)
    {
        auto result = std::list<std::shared_ptr<WindowCore>>{};

        bdn::android::wrapper::NativeRootView rootView(javaRootViewRef);

        // enumerate all children of the root view. Those are our
        // "window" views.
        int childCount = rootView.getChildCount();
        for (int i = 0; i < childCount; i++) {
            bdn::android::wrapper::View child = rootView.getChildAt(i);

            if (auto windowCore = std::dynamic_pointer_cast<WindowCore>(viewCoreFromJavaViewRef(child.getRef_()))) {
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
                // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
                const_cast<WindowCore *>(this)->rootViewDisposed();
            }
        }

        return accessibleRef;
    }

    void WindowCore::scheduleLayout() { getJView().requestLayout(); }

    void WindowCore::visitInternalChildren(const std::function<void(std::shared_ptr<View::Core>)> &function)
    {
        if (contentView.get()) {
            function(contentView->viewCore());
        }
    }

    void WindowCore::updateGeometry() { ViewCore::updateGeometry(); }
}
