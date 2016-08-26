#ifndef BDN_WEB_ViewCore_H_
#define BDN_WEB_ViewCore_H_

#include <bdn/IViewCore.h>
#include <bdn/IdGen.h>

#include <emscripten/val.h>


namespace bdn
{
namespace web
{

class ViewCore : public Base, BDN_IMPLEMENTS IViewCore
{
public:
    ViewCore(   View* pOuterView,
                const String& elementName,
                const std::map<String,String>& attribMap = std::map<String,String>())
    {
        _pOuterViewWeak = pOuterView;

        _elementId = "bdn_view_"+std::to_string(IdGen::newID());

        emscripten::val docVal = emscripten::val::global("document");
        
        _pJsObj = new emscripten::val( docVal.call<emscripten::val>("createElement", elementName.asUtf8() ) );

        for(auto attribPair: attribMap)
            _pJsObj->set(attribPair.first.asUtf8(), attribPair.second.asUtf8());

        // we always use absolute positioning. Set this here.
        emscripten::val styleObj = (*_pJsObj)["style"];
        styleObj.set("position", "absolute");
        
        _pJsObj->set("id", _elementId.asUtf8() );

        setVisible( pOuterView->visible() );
        setPadding( pOuterView->padding() );
        
        _addToParent( pOuterView->getParentView() );        
    }

    ~ViewCore()
    {
        dispose();
    }
    
    
    void dispose() override
    {
        _pOuterViewWeak = nullptr;
        
        if(_pJsObj!=nullptr)
        {
            delete _pJsObj;
            _pJsObj = nullptr;
        }
    }

    /** Returns the id of the DOM element that corresponds to the view.*/
    String getHtmlElementId() const
    {
        return _elementId;
    }


    const View* getOuterView() const
    {
        return _pOuterViewWeak;
    }
    
    View* getOuterView()
    {
        return _pOuterViewWeak;
    }
    


    
    void setVisible(const bool& visible) override
    {
        (*_pJsObj)["style"].set("visibility", visible ? "visible" : "hidden");
    }
    
        
    void setPadding(const Nullable<UiMargin>& padding) override
    {
        // we need to set the padding in the DOM element, so that it can adjust its
        // display and the minimum size accordingly.

        if(padding.isNull())
        {
            // we should use "default" padding.
            (*_pJsObj)["style"].set("padding", "initial");
        }
        else
        {
            String paddingString;

            UiMargin pad = padding.get();

            paddingString = UiProvider::get().uiLengthToHtmlString(pad.top)
                             + " " + UiProvider::get().uiLengthToHtmlString(pad.right)
                             + " " + UiProvider::get().uiLengthToHtmlString(pad.bottom)
                             + " " + UiProvider::get().uiLengthToHtmlString(pad.left);

            (*_pJsObj)["style"].set("padding", paddingString.asUtf8() );
        }
    }
    
    
    void setBounds(const Rect& bounds) override
    {
        emscripten::val styleObj = (*_pJsObj)["style"];

        styleObj.set("width", UiProvider::pixelsToHtmlString(bounds.width).asUtf8() );
        styleObj.set("height", UiProvider::pixelsToHtmlString(bounds.height).asUtf8() );

        styleObj.set("left", UiProvider::pixelsToHtmlString(bounds.x).asUtf8() );
        styleObj.set("top", UiProvider::pixelsToHtmlString(bounds.y).asUtf8() );        
    }
    
    
    int uiLengthToPixels(const UiLength& uiLength) const override
    {
        return UiProvider::get().uiLengthToPixels(uiLength);
    }
    
    
    Margin uiMarginToPixelMargin(const UiMargin& margin) const override
    {
        return UiProvider::get().uiMarginToPixelMargin(margin);
    }
    
    
    
    
    Size calcPreferredSize() const override
    {
        return _calcPreferredSize(-1, -1);
    }
    
    
    int calcPreferredHeightForWidth(int width) const override
    {
        return _calcPreferredSize(width, -1).height;
    }
    
    
    int calcPreferredWidthForHeight(int height) const override
    {
        return _calcPreferredSize(-1, height).width;
    }
    
    
    bool tryChangeParentView(View* pNewParent) override
    {
        _addToParent(pNewParent);
        
        return true;
    }
    
    
    
protected:
    Size _calcPreferredSize(int forWidth, int forHeight) const
    {
        emscripten::val styleObj = (*_pJsObj)["style"];

        // If we would simply look at the width/height property here, we would only
        // get the size we have last set ourselves.
        // To get the preferred size, we have to update the style to clear the previous
        // size assignment. Then the element will report its preferred size.
        // And then we restore the old style.

        emscripten::val oldWidthStyle = styleObj["width"];
        emscripten::val oldHeightStyle = styleObj["height"];

        styleObj.set("width", (forWidth==-1) ? std::string("auto") : UiProvider::pixelsToHtmlString(forWidth).asUtf8() );        
        styleObj.set("height", (forHeight==-1) ? std::string("auto") : UiProvider::pixelsToHtmlString(forHeight).asUtf8() );

        int width = (*_pJsObj)["offsetWidth"].as<int>();
        int height = (*_pJsObj)["offsetHeight"].as<int>();

        styleObj.set("width", oldWidthStyle);
        styleObj.set("height", oldHeightStyle);

        return Size(width, height);
    }

    void _addToParent(View* pParent)
    {
        emscripten::val docVal = emscripten::val::global("document");

        if(pParent==nullptr)
            docVal.call<emscripten::val>("getElementsByTagName", std::string("body"))[0].call<void>("appendChild", *_pJsObj);
        else
        {
            P<ViewCore> pParentCore = cast<ViewCore>(pParent->getViewCore());
            if(pParentCore==nullptr)
                throw ProgrammingError("Internal error: parent of bdn::web::ViewCore does not have a core.");

            docVal.call<emscripten::val>("getElementById", pParentCore->getHtmlElementId().asUtf8() ).call<void>("appendChild", *_pJsObj);
        }
    }

    View*               _pOuterViewWeak;
    String              _elementId;
    Rect                _currBounds;
    
    emscripten::val*    _pJsObj;
};

}
}

#endif


