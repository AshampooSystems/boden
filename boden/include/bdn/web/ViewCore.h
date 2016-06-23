#ifndef BDN_WEB_ViewCore_H_
#define BDN_WEB_ViewCore_H_

#include <bdn/IViewCore.h>

#include <emscripten/val.h>


namespace bdn
{

class ViewCore : public Base, BDN_IMPLEMENTS IViewCore
{
public:
	/*
    WindowCore(Window* pParent,
           const std::string& elementName,
           const std::map<std::string,std::string>& attribMap = std::map<std::string,std::string>())
    {
        _id = IdGen::newID();

        emscripten::val docVal = emscripten::val::global("document");
        
        _pJsObj = new emscripten::val( docVal.call<emscripten::val>("createElement", elementName) );
        
        for(auto attribPair: attribMap)
            _pJsObj->set(attribPair.first, attribPair.second);
        
        _jsId = "boden_window_"+std::to_string(_id);
        
        _pJsObj->set("id", _jsId);
        
        (*_pJsObj)["style"].set("visibility", "hidden");

        if(pParent==nullptr)
            docVal.call<emscripten::val>("getElementsByTagName", std::string("body"))[0].call<void>("appendChild", *_pJsObj);
        else
            docVal.call<emscripten::val>("getElementById", "boden_window_"+std::to_string(pParent->getID()) ).call<void>("appendChild", *_pJsObj);
    }
    
    long long getID() const
    {
        return _id;
    }
    
    virtual void show(bool visible=true) override
    {
        (*_pJsObj)["style"].set("visibility", visible ? "visible" : "hidden");
    }
    
    virtual void hide() override
    {
        show(false);
    }
    
protected:
    long long           _id;
    std::string         _jsId;
    
    emscripten::val*    _pJsObj;
    */
};

}

#endif


