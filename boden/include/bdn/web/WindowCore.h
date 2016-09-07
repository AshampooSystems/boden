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

        updateOuterViewBounds();
    }

    void dispose() override
    {
        if(!_domObject.isNull() && !_domObject.isUndefined())
        {
            // delete the DOM object
            emscripten::val parent = _domObject["parentNode"];
            if(!parent.isNull())
                parent.call<void>("removeChild", _domObject);
        }

        ViewCore::dispose();
    }


    void setBounds(const Rect& bounds) override
    {
        // we cannot modify the size or position of the window.
        // So just reset the bounds back to their original value.
        updateOuterViewBounds();
    }

    void setTitle(const String& title) override
    {
        // set the DOM document title
        emscripten::val docVal = emscripten::val::global("document");

        docVal.set("title", title.asUtf8() );
    }


    Rect getContentArea() override
    {
        return Rect( Point(0,0), getOuterView()->bounds().get().getSize() );
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
    void updateOuterViewBounds()
    {
        int width = _domObject["offsetWidth"].as<int>();
        int height = _domObject["offsetHeight"].as<int>();
        getOuterView()->bounds() = Rect(0, 0, width, height);

        getOuterView()->needLayout();
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


