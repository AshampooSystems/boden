#ifndef BDN_WEB_ViewCore_H_
#define BDN_WEB_ViewCore_H_

#include <bdn/IViewCore.h>

#include <emscripten/val.h>


namespace bdn
{
namespace web
{

class ViewCore : public Base, BDN_IMPLEMENTS IViewCore
{
public:
    ViewCore(   View* pOuterView,
                const String& elementName
                const std::map<String,String>& attribMap = std::map<String,String>())
    {
        _pOuterViewWeak = pOuterView;

        _id = IdGen::newID();

        emscripten::val docVal = emscripten::val::global("document");
        
        _pJsObj = new emscripten::val( docVal.call<emscripten::val>("createElement", elementName) );
        
        for(auto attribPair: attribMap)
            _pJsObj->set(attribPair.first.asUtf8(), attribPair.second.asUtf8());
        
        _elementId = "bdn_view_"+std::to_string(_id);
        
        _pJsObj->set("id", _elementId);
        
        (*_pJsObj)["style"].set("visibility", "hidden");

        P<View> pParentView = pOuterView->getParentView();

        if(pParent==nullptr)
            docVal.call<emscripten::val>("getElementsByTagName", std::string("body"))[0].call<void>("appendChild", *_pJsObj);
        else
            docVal.call<emscripten::val>("getElementById", "bdn_view_"+std::to_string(pParent->getID()) ).call<void>("appendChild", *_pJsObj);

        setVisible( pOuterView->visible() );
        setPadding( pOuterView->padding() );
    }

    /** Returns the id of the DOM element that corresponds to the view.*/
    String getElementId() const
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
    
    
    void setMargin(const UiMargin& margin) override
    {
        // we don't care about OUR margin. Our parent uses it during layout.
        // So, do nothing here.
    }
    
    
    void setPadding(const UiMargin& padding) override
    {
        // we need to set the padding in the DOM element, so that it can adjust its
        // display and the minimum size accordingly.

        String paddingString;

        paddingString = getUiLengthString(padding.top)
                         + " " + getUiLengthString(padding.right)
                         + " " + getUiLengthString(padding.bottom)
                         + " " + getUiLengthString(padding.left);

        (*_pJsObj)["style"].set("padding", paddingString.asUtf8() );

        _pOuterViewWeak->needSizingInfoUpdate();
    }
    
    
    void setBounds(const Rect& bounds) override
    {
        emscripten::val& styleObj = (*_pJsObj)["style"];

        styleObj.set("width", getPixelString(bounds.width));
        styleObj.set("height", getPixelString(bounds.height));

        styleObj.set("position", "absolute");
        styleObj.set("left", getPixelString(bounds.x));
        styleObj.set("top", getPixelString(bounds.y));
        
        getOuterView()->needLayout();
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
    
    
    virtual void addChildUIView( UIView* childView )
    {
        [_view addSubview:childView];
    }

    
    virtual void show(bool visible=true) override
    {
        
    }
    
    virtual void hide() override
    {
        show(false);
    }
    
protected:
    View*               _pOuterViewWeak;
    String              _elementId;
    Rect                _currBounds;
    
    emscripten::val*    _pJsObj;
};

}
}

#endif


