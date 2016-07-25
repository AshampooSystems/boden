#ifndef BDN_ANDROID_WindowCore_H_
#define BDN_ANDROID_WindowCore_H_

#include <bdn/IWindowCore.h>
#include <bdn/Window.h>

#include <bdn/android/ViewCore.h>
#include <bdn/android/JNativeRootView.h>

namespace bdn
{
namespace android
{

class WindowCore : public ViewCore, BDN_IMPLEMENTS IWindowCore
{
private:
    P<JNativeViewGroup> createView(Window* pOuterWindow)
    {
        return newObj<JNativeViewGroup>();
    }

public:
    WindowCore( Window* pOuterWindow )
        : ViewCore( pOuterWindow, createView(pOuterWindow) )
    {
        setTitle( pOuterWindow->title() );

        // try to attach the window to a root view
        bdn::java::Reference rootViewRef;
        tryGetAccessibleRootViewRef(rootViewRef);
    }


    void setTitle(const String& title) override
    {
        // the title is not shown by android.
    }

    void setBounds(const Rect& bounds) override
    {
        // we are always exactly the size of the root view. So we undo
        // any changes we get from the outside world.

        if(bounds!=_currBounds)
            getOuterView()->bounds() = _currBounds;
    }

    void setVisible(const bool& visible) override
    {
        ViewCore::setVisible( visible );

        if(visible)
        {
            // ensure that the window is attached to a root view (if one is available)
            bdn::java::Reference rootViewRef;
            tryGetAccessibleRootViewRef(rootViewRef);
        }
    }


    Rect getContentArea() override
    {
        // content area = bounds (there are no borders)
        return _currentBounds;
    }


    Size calcWindowSizeFromContentAreaSize(const Size& contentSize) override
    {
        // our "window" is simply the size of the div. It as no borders.
        // So "window" size = content size.
        return contentSize;
    }


    Size calcContentAreaSizeFromWindowSize(const Size& windowSize) override
    {
        // our "window" is simply the size of the div. It as no borders.
        // So "window" size = content size.
        return windowSize;
    }


    Size calcMinimumSize() const override
    {
        // don't have a minimum size since the title bar is not connected to our window div.
        // (the title is displayed in the browser tab bar).
        return Size(0, 0);
    }
    

    Rect getScreenWorkArea() const override
    {
        bdn::java::Reference temporaryStrongJavaReference;
        P<JNativeRootView> pRootView = updateAndGetRootView(temporaryStrongJavaReference);

        if(pRootView==nullptr)
        {
            // don't have a root view => work area size is 0
            return Rect();
        }
        else
        {
            int width = pRootView->getWidth();
            int height = pRootView->getHeight();

            return Rect(0, 0, width, height );
        }
    }


    static void _rootViewCreated( const bdn::java::Reference& javaRef )
    {
        // we store only a weak referene in the registry. We do not want to
        // keep the java-side root view object alive.
        getRootViewRegistry()->add( javaRef.toWeak() );
    }

    static void _rootViewDisposed( const bdn::java::Reference& javaRef )
    {
        getRootViewRegistry()->remove( javaRef );

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

    static void _rootViewConfigurationChanged( const JavaConfiguration& config )
    {
        std::list< P<WindowCore> > windowCoreList;

        getWindowCoreListFromRootView( javaRef, windowCoreList );

        for( P<WindowCore>& pWindowCore: windowCoreList)
            pWindowCore->rootViewSizeChanged(width, height);
    }

protected:

    /** Called when the window core's root view was garbage collected or disposed.*/
    virtual void rootViewDisposed()
    {
        _weakRootViewRef = bdn::java::Reference();
    }

    /** Called when the root view that this window is attached to has changed
     *  its size.
     *
     *  The default implementation updates the Window's size to match the new root dimensions.
     *  */
    virtual void rootViewSizeChanged(int width, int height)
    {
        _currentBounds = Rect(0, 0, width, height);

        getOuterView()->bounds() = _currentBounds;
    }

    virtual void attachedToNewRootView(const bdn::java::Reference& javaRef)
    {
        // set the window's bounds to fill the root view completely.
        JNativeRootView rootView(javaRef);

        rootViewSizeChanged(rootView.getWidth(), rootView.getHeight() );
    }


private:

    void getWindowCoreListFromRootView( const Reference& javaRootViewRef, std::list< P<WindowCore> >& windowCoreList )
    {
        JNativeRootView rootView(javaRootViewRef);

        // enumerate all children of the root view. Those are our
        // "window" views.
        int childCount = rootView.getChildCount();
        for(int i=0; i<childCount; i++)
        {
            bdn::java::Reference childRef = rootView.getChildAt(i);

            P<WindowCore> pWindowCore = cast<WindowCore>( ViewCore::getViewCoreFromJavaView(childRef) );

            windowCoreList.push_back( pWindowCore );
        }
    }


    /** Stores an accessible reference to the window's root view in accessibleJavaRef.
     *  Returns true if a valid non-null root view reference was stored there.
     *
     *  If the window core is currently associated with a root view then the function
     *  checks if it is still valid (i.e. if the java-side object was not garbage collected
     *  or disposed). If that is the case then a strong reference to the root view is returned
     *  that can be used to access it.
     *
     *  If the window core is not currently associated with a root view or if the previously
     *  associated root view has been invalidated then the function tries to find a new valid root
     *  view and attaches the window core to that.
     *
     *  If not valid root view could be returned then a null reference is stored in
     *  accessibleJavaRef and false is returned.*/
    bool tryGetAccessibleRootViewRef(bdn::java::Reference& accessibleJavaRef)
    {
        accessibleJavaRef = _weakRootViewRef.toAccessible();

        if(!accessibleJavaRef.isNull())
            return true;
        else
        {
            // did not have a root view or the root view has disappeared.
            _weakRootViewRef = bdn::java::Reference();

            // see if we have another root view that we can attach to.
            accessibleJavaRef = RootViewRegistry::get()->getFirstValidRootView();

            if(!accessibleJavaRef.isNull())
            {
                attachedToNewRootView(accessibleJavaRef);

                _weakRootViewRef = accessibleJavaRef.toWeak();
                return true;
            }
            else
                return false;
        }
    }


    class RootViewRegistry : public Base
    {
    public:
        void add( const bdn::java::Reference& javaRef, context, config )
        {
            MutexLock lock( _mutex );

            _rootViewList.push_back( javaRef );
        }

        void remove( const bdn::java::Reference& javaRef )
        {
            MutexLock lock( _mutex );

            _rootViewList.erase( javaRef );
        }


        /** Returns the a strong java reference to the first valid root view in the list.
         *
         *  The function automatically checks if the Java side object of a registered root view
         *  has already been garbage collected.
         *  If it has then the root view is considered invalid and will not be returned.
         *
         *  Returns a null reference if not valid root view is found.
         *
         * */
        bdn::java::Reference getFirstValidRootView()
        {
            MutexLock lock( _mutex );

            // So we can simply return the first root view from the list.
            while( !_rootViewList.isEmpty() )
            {
                bdn::java::Reference javaRef = _rootViewList.front().toStrong();

                if(!javaRef.isNull())
                    return javaRef;


                // java-side object has been disposed or garbage collected.
                // Remove the entry from the list.
                _rootViewList.erase( _rootViewList.begin() );
            }

            return bdn::java::Reference();
        }

    private:
        Mutex _mutex;
        std::list<bdn::java::Reference>  _rootViewList;
    };

    BDN_SAFE_STATIC( RootViewRegistry, getRootViewRegistry );


    bdn::java::Reference    _weakRootViewRef;
    Rect                    _currentBounds;
};


}
}

#endif


