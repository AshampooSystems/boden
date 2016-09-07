#ifndef BDN_WEB_TextViewCore_H_
#define BDN_WEB_TextViewCore_H_

#include <emscripten/html5.h>

#include <bdn/web/ViewCore.h>
#include <bdn/ITextViewCore.h>
#include <bdn/TextView.h>


namespace bdn
{
namespace web
{


class TextViewCore : public ViewCore, BDN_IMPLEMENTS ITextViewCore
{
public:
    TextViewCore( TextView* pOuterTextView )
    : ViewCore( pOuterTextView, "div" )
    {
        setText( pOuterTextView->text() );        
    }
    
    void setText(const String& text) override
    {
        _domObject.set("textContent", text.asUtf8());
    }    
};


}    
}


#endif


