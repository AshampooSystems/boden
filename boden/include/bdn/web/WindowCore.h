#ifndef BDN_WEB_WindowCore_H_
#define BDN_WEB_WindowCore_H_

#include <bdn/IWindowCore.h>
#include <bdn/Window.h>

#include <bdn/web/ViewCore.h>


namespace bdn
{
namespace web
{

class WindowCore : public ViewCore, BDN_IMPLEMENTS IWindowCore
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

        updateOuterViewPosition();
        updateOuterViewSize();
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


    void setPosition(const Point& position) override
    {
        // we cannot modify the size or position of the window.
        // So just reset the bounds back to their original value.
        updateOuterViewPosition();
    }

    void setSize(const Size& size) override
    {
        // we cannot modify the size or position of the window.
        // So just reset the bounds back to their original value.
        updateOuterViewSize();
    }

    void setTitle(const String& title) override
    {
        // set the DOM document title
        emscripten::val docVal = emscripten::val::global("document");

        docVal.set("title", title.asUtf8() );
    }


    Rect getContentArea() override
    {
        P<View> pView = getOuterViewIfStillAttached();
        if(pView!=nullptr)
            return Rect( Point(0,0), pView->size().get() );
        else
            return Rect();
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
        emscripten::val windowVal = emscripten::val::global("window");

        int width = windowVal["innerWidth"].as<int>();
        int height = windowVal["innerHeight"].as<int>();

        return Rect(0, 0, width, height);
    }
    
private:
    void updateOuterViewPosition()
    {
        P<View> pView = getOuterViewIfStillAttached();
        if(pView!=nullptr)
            pView->position() = Point(0,0);
    }

    void updateOuterViewSize()
    {
        int width = _domObject["offsetWidth"].as<int>();
        int height = _domObject["offsetHeight"].as<int>();

        P<View> pView = getOuterViewIfStillAttached();
        if(pView!=nullptr)
        {
            pView->size() = Size(width, height);
            pView->needLayout();
        }
    }

    bool resized(int eventType, const EmscriptenUiEvent* pEvent)
    {
        updateOuterViewSize();

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


