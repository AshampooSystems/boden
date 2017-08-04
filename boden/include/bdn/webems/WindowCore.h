#ifndef BDN_WEBEMS_WindowCore_H_
#define BDN_WEBEMS_WindowCore_H_

#include <bdn/IWindowCore.h>
#include <bdn/Window.h>

#include <bdn/webems/ViewCore.h>


namespace bdn
{
namespace webems
{

class WindowCore : public ViewCore,
    BDN_IMPLEMENTS IWindowCore,
    BDN_IMPLEMENTS LayoutCoordinator::IWindowCoreExtension
{
public:
    WindowCore( Window* pOuterWindow )
        : ViewCore(pOuterWindow, "div")     // a "window" is simply a top level div
    {
        setTitle( pOuterWindow->title() );

        // the window div always has the same size as the browser window.        
        _domObject["style"].set("width", "100%");
        _domObject["style"].set("height", "100%");

        emscripten_set_resize_callback( "#window", static_cast<void*>(this), false, &WindowCore::_resizedCallback);

        // update the outer view's bounds with the current bounds from this window.
        // Do this in an async call, because the current callstack is unknown and the
        // property changes might otherwise have unexpected side effects.
        P<WindowCore> pThis = this;
        asyncCallFromMainThread(
            [pThis]()
            {
                pThis->updateOuterViewBounds();                
            });
    }

    ~WindowCore()
    {
        if(!_domObject.isNull() && !_domObject.isUndefined())
        {
            // delete the DOM object
            emscripten::val parent = _domObject["parentNode"];
            if(!parent.isNull())
                parent.call<void>("removeChild", _domObject);
        }
    }



    Rect adjustAndSetBounds(const Rect& bounds) override
    {
        // we cannot modify our size and position. Do nothing and return our current bounds.
        return _getBounds();
    }


    Rect adjustBounds(const Rect& requestedBounds, RoundType positionRoundType, RoundType sizeRoundType) const override
    {
        // we cannot modify our size and position. Do nothing and return our current bounds.
        return _getBounds();
    }
    


    void setTitle(const String& title) override
    {
        // set the DOM document title
        emscripten::val docVal = emscripten::val::global("document");

        docVal.set("title", title.asUtf8() );
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



    
private:


    Rect getScreenWorkArea() const
    {
        emscripten::val windowVal = emscripten::val::global("window");

        int width = windowVal["innerWidth"].as<int>();
        int height = windowVal["innerHeight"].as<int>();

        return Rect(0, 0, width, height);
    }

    Rect _getBounds() const
    {
        int width = _domObject["offsetWidth"].as<int>();
        int height = _domObject["offsetHeight"].as<int>();

        // we do not know the actual position of our window on the screen.
        // The web browser does not expose that.

        return Rect(0, 0, width, height);
    }

    void updateOuterViewBounds()
    {
        if(!_domObject.isNull() && !_domObject.isUndefined())
        {
            P<View> pView = getOuterViewIfStillAttached();
            if(pView!=nullptr)
                pView->adjustAndSetBounds( _getBounds() );
        }
    }


    bool resized(int eventType, const EmscriptenUiEvent* pEvent)
    {
        updateOuterViewBounds();

        return false;
    }

    static EM_BOOL _resizedCallback(int eventType, const EmscriptenUiEvent* pEvent, void* pUserData)
    {
        return static_cast<WindowCore*>(pUserData)->resized(eventType, pEvent);
    }

};


}
}

#endif


