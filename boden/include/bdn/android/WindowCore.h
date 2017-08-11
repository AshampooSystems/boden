#ifndef BDN_ANDROID_WindowCore_H_
#define BDN_ANDROID_WindowCore_H_

#include <bdn/IWindowCore.h>
#include <bdn/Window.h>

#include <bdn/windowCoreUtil.h>

#include <bdn/java/WeakReference.h>
#include <bdn/android/ViewCore.h>
#include <bdn/android/JNativeRootView.h>
#include <bdn/android/JConfiguration.h>
#include <bdn/android/IParentViewCore.h>

#include <bdn/log.h>

namespace bdn
{
namespace android
{

class WindowCore : public ViewCore, BDN_IMPLEMENTS IWindowCore, BDN_IMPLEMENTS LayoutCoordinator::IWindowCoreExtension, BDN_IMPLEMENTS IParentViewCore
{
private:
    P<JNativeViewGroup> createJNativeViewGroup(Window* pOuterWindow)
    {
        // we need a context to create our view object.
        // To know the context we first have to determine the root view that
        // this window will be connected to.

        // We connect to the first root view that is available in the root view registry.
        JNativeRootView rootView( getRootViewRegistryForCurrentThread().getNewestValidRootView() );

        if(rootView.isNull_())
            throw ProgrammingError("WindowCore being created but there are no native root views available. You must create a NativeRootActivity or NativeRootView instance!");

        P<JNativeViewGroup> pViewGroup = newObj<JNativeViewGroup>( rootView.getContext() );

        // add the view group to the root view. That is important so that the root view we have chosen
        // is fixed to the view group instance.
        rootView.addView( *pViewGroup );

        return pViewGroup;
    }

public:
    WindowCore( Window* pOuterWindow )
        : ViewCore( pOuterWindow, createJNativeViewGroup(pOuterWindow) )
    {
        setTitle( pOuterWindow->title() );

        JNativeRootView rootView( getJView().getParent().getRef_() );

        _weakRootViewRef = bdn::java::WeakReference( rootView.getRef_() );

        updateUiScaleFactor( rootView.getContext().getResources().getConfiguration() );

        // update our size to fully fill the root view.
        // Do this async, so that the property change cannot have bad effects on the in-progress
        // operation.
        P<WindowCore> pThis = this;
        asyncCallFromMainThread(
            [pThis, rootView]() mutable
            {
                pThis->rootViewSizeChanged(rootView.getWidth(), rootView.getHeight() );
            } );
    }

    ~WindowCore()
    {
        JView* pView = &getJView();
        if(pView!=nullptr)
        {
            // remove the view from its parent.
            JViewGroup parent( pView->getParent().getRef_() );
            if (!parent.isNull_())
                parent.removeView(*pView);
        }
    }


    void setTitle(const String& title) override
    {
        // the title is not shown by android.
    }



    Rect adjustAndSetBounds(const Rect& requestedBounds) override
    {
        // we cannot influence our bounds. So "adjust" the bounds to the view's current bounds
        return _currentBounds;
    }

    Rect adjustBounds(const Rect& requestedBounds, RoundType positionRoundType, RoundType sizeRoundType) const override
    {
        // we cannot influence our bounds. So "adjust" the bounds to the view's current bounds
        return _currentBounds;
    }


    void setVisible(const bool& visible) override
    {
        ViewCore::setVisible( visible );
    }




    void invalidateSizingInfo(View::InvalidateReason reason) override
    {
        // nothing to do since we do not cache sizing info in the core.
    }


    void needLayout(View::InvalidateReason reason) override
    {
        P<View> pOuterView = getOuterViewIfStillAttached();
        if(pOuterView!=nullptr)
        {
            P<UiProvider> pProvider = tryCast<UiProvider>( pOuterView->getUiProvider() );
            if(pProvider!=nullptr)
                pProvider->getLayoutCoordinator()->viewNeedsLayout( pOuterView );
        }
    }

    void childSizingInfoInvalidated(View* pChild) override
    {
        P<View> pOuterView = getOuterViewIfStillAttached();
        if(pOuterView!=nullptr)
        {
            pOuterView->invalidateSizingInfo( View::InvalidateReason::childSizingInfoInvalidated );
            pOuterView->needLayout( View::InvalidateReason::childSizingInfoInvalidated );
        }
    }



    Size calcPreferredSize( const Size& availableSpace = Size::none() ) const override
    {
        P<Window> pWindow = cast<Window>( getOuterViewIfStillAttached() );
        if(pWindow!=nullptr)
            return defaultWindowCalcPreferredSizeImpl( pWindow, availableSpace, Margin(), Size() );
        else
            return Size(0,0);
    }

    void layout() override
    {
        P<Window> pWindow = cast<Window>( getOuterViewIfStillAttached() );
        if(pWindow!=nullptr)
            defaultWindowLayoutImpl( pWindow, getContentArea() );
    }




    void requestAutoSize() override
    {
        P<Window> pWindow = cast<Window>( getOuterViewIfStillAttached() );
        if(pWindow!=nullptr)
        {
            P<UiProvider> pProvider = tryCast<UiProvider>( pWindow->getUiProvider() );
            if(pProvider!=nullptr)
                pProvider->getLayoutCoordinator()->windowNeedsAutoSizing( pWindow );
        }
    }

    void requestCenter() override
    {
        P<Window> pWindow = cast<Window>( getOuterViewIfStillAttached() );
        if(pWindow!=nullptr)
        {
            P<UiProvider> pProvider = tryCast<UiProvider>( pWindow->getUiProvider() );
            if(pProvider!=nullptr)
                pProvider->getLayoutCoordinator()->windowNeedsCentering( pWindow );
        }
    }


    void autoSize() override
    {
        // we cannot change our size. So, do nothing
    }

    void center() override
    {
        // we cannot change our position. So, do nothing.
    }





    static void _rootViewCreated( const bdn::java::Reference& javaRef )
    {
        // we store only a weak referene in the registry. We do not want to
        // keep the java-side root view object alive.
        getRootViewRegistryForCurrentThread().add( bdn::java::WeakReference(javaRef) );
    }

    static void _rootViewDisposed( const bdn::java::Reference& javaRef )
    {
        // this may be called by the garbage collector, so it might be in
        // an arbitrary thread. That means that the root view registry we get
        // might not be the one that actually owns the reference. In that case
        // the object will not be removed from the registry. But that is OK: the registry
        // will notice the next time it tries to access the root view and then it will be removed.
        getRootViewRegistryForCurrentThread().remove( javaRef );

        std::list< P<WindowCore> > windowCoreList;
        getWindowCoreListFromRootView( javaRef, windowCoreList );

        for( P<WindowCore>& pWindowCore: windowCoreList)
            pWindowCore->rootViewDisposed();
    }

    static void _rootViewSizeChanged( const bdn::java::Reference& javaRef, int width, int height )
    {
        std::list< P<WindowCore> > windowCoreList;

        getWindowCoreListFromRootView( javaRef, windowCoreList );

        for( P<WindowCore>& pWindowCore: windowCoreList)
            pWindowCore->rootViewSizeChanged(width, height);
    }

    static void _rootViewConfigurationChanged( const bdn::java::Reference& javaRef, JConfiguration config )
    {
        std::list< P<WindowCore> > windowCoreList;

        getWindowCoreListFromRootView( javaRef, windowCoreList );

        for( P<WindowCore>& pWindowCore: windowCoreList)
            pWindowCore->rootViewConfigurationChanged(config);
    }


    double getUiScaleFactor() const override
    {
        return ViewCore::getUiScaleFactor();
    }

    void addChildJView( JView childJView ) override
    {
        JNativeViewGroup parentGroup( getJView().getRef_() );

        parentGroup.addView( childJView );
    }


protected:

    Rect getContentArea()
    {
        // content area = bounds (there are no borders)
        return _currentBounds;
    }

    /** Called when the window core's root view was garbage collected or disposed.*/
    virtual void rootViewDisposed()
    {
        // this may be called by the garbage collector, so it might be in
        // an arbitrary thread

        MutexLock lock(_rootViewMutex);

        _weakRootViewRef = bdn::java::WeakReference();
    }

    /** Called when the root view that this window is attached to has changed
     *  its size.
     *
     *  The default implementation updates the Window's size to match the new root dimensions.
     *  */
    virtual void rootViewSizeChanged(int width, int height)
    {
        //logInfo("rootViewSizeChanged("+std::to_string(width)+"x"+std::to_string(height));

        // set our container view to the same size as the root.
        // Note that this is necessary because the root view does not have a bdn::View associated with it.
        // So there is not automatic layout happening.
        JNativeRootView rootView( getJView().getParent().getRef_() );
        rootView.setChildBounds( getJView(), 0, 0, width, height);
        rootView.requestLayout();

        double scaleFactor = getUiScaleFactor();

        _currentBounds = Rect(0, 0, width / scaleFactor, height / scaleFactor);

        P<View> pView = getOuterViewIfStillAttached();
        if(pView!=nullptr)
            pView->adjustAndSetBounds( _currentBounds );
    }

    /** Called when the configuration changed for this window core.
     *
     *  The default implementation updates the Window's size to match the new root dimensions.
     *  */
    virtual void rootViewConfigurationChanged(JConfiguration config)
    {
        updateUiScaleFactor(config);
    }


    virtual void attachedToNewRootView(const bdn::java::Reference& javaRef)
    {
        // set the window's bounds to fill the root view completely.
        JNativeRootView rootView(javaRef);

        JConfiguration config( rootView.getContext().getResources().getConfiguration() );

        updateUiScaleFactor( config );

        rootViewSizeChanged(rootView.getWidth(), rootView.getHeight() );
    }


private:

    Rect getScreenWorkArea() const
    {
        JNativeRootView rootView( tryGetAccessibleRootViewRef() );

        if(rootView.isNull_())
        {
            // don't have a root view => work area size is 0
            return Rect();
        }
        else
        {
            int width = rootView.getWidth();
            int height = rootView.getHeight();

            // logInfo("screen area: ("+std::to_string(width)+"x"+std::to_string(height)+")");

            return Rect(0, 0, width, height );
        }
    }

    void updateUiScaleFactor( JConfiguration config )
    {
        int dpi = config.densityDpi();

        // the scale factor is at 1 for 160 dpi. Note that smaller DPI values
        // are also possible, so the scale factor can be <1 as well.

        double scaleFactor = dpi / 160.0;

        setUiScaleFactor(scaleFactor);
    }

    static void getWindowCoreListFromRootView( const bdn::java::Reference& javaRootViewRef, std::list< P<WindowCore> >& windowCoreList )
    {
        JNativeRootView rootView(javaRootViewRef);

        // enumerate all children of the root view. Those are our
        // "window" views.
        int childCount = rootView.getChildCount();
        for(int i=0; i<childCount; i++)
        {
            JView child = rootView.getChildAt(i);

            P<WindowCore> pWindowCore = cast<WindowCore>( ViewCore::getViewCoreFromJavaViewRef(child.getRef_()) );
            if(pWindowCore!=nullptr)
                windowCoreList.push_back( pWindowCore );
        }
    }


    /** Returns an accessible reference to the window's root view.
     *
     *  Note that we only hold a weak reference to the root view, so the root view may
     *  have been garbage collected. If it was then this function will return a null reference.*/
    bdn::java::Reference tryGetAccessibleRootViewRef() const
    {
        bdn::java::Reference accessibleRef;

        {
            MutexLock lock(_rootViewMutex);

            accessibleRef = _weakRootViewRef.toStrong();

            if(accessibleRef.isNull())
                const_cast<WindowCore*>(this)->rootViewDisposed();
        }

        return accessibleRef;
    }



    class RootViewRegistry : public Base
    {
    public:
        void add( const bdn::java::WeakReference& javaRef )
        {
            _rootViewList.push_back( javaRef );
        }

        void remove( const bdn::java::Reference& javaRef )
        {
            auto it = std::find( _rootViewList.begin(), _rootViewList.end(), javaRef );
            if(it!=_rootViewList.end())
                _rootViewList.erase( it );
        }


        /** Returns the a strong java reference to the most recently created root view that is still valid.
         *
         *  The function automatically checks if the Java side object of a registered root view
         *  has already been garbage collected.
         *  If it has then the root view is considered invalid and will not be returned.
         *
         *  Returns a null reference if not valid root view is found.
         *
         * */
        bdn::java::Reference getNewestValidRootView()
        {
            // So we can simply return the first root view from the list.
            while( !_rootViewList.empty() )
            {
                bdn::java::Reference javaRef = _rootViewList.back().toStrong();

                if(!javaRef.isNull())
                    return javaRef;


                // java-side object has been disposed or garbage collected.
                // Remove the entry from the list.
                _rootViewList.pop_back();
            }

            return bdn::java::Reference();
        }

    private:
        std::list<bdn::java::WeakReference>  _rootViewList;
    };


    // we make the root view registry thread-local because Android can sometimes be weird.
    // Activities from different applications can sometimes run in the same process.
    // By making it thread-local we ensure that we really can access the most recently created
    // root view from "our" activity.
    BDN_SAFE_STATIC_THREAD_LOCAL( RootViewRegistry, getRootViewRegistryForCurrentThread );


    mutable Mutex               _rootViewMutex;
    bdn::java::WeakReference    _weakRootViewRef;

    Rect                        _currentBounds;
};


}
}

#endif


