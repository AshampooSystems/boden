#ifndef BDN_WEBEMS_TextViewCore_H_
#define BDN_WEBEMS_TextViewCore_H_

#include <emscripten/html5.h>

#include <bdn/webems/ViewCore.h>
#include <bdn/ITextViewCore.h>
#include <bdn/TextView.h>


namespace bdn
{
namespace webems
{


class TextViewCore : public ViewCore, BDN_IMPLEMENTS ITextViewCore
{
public:
    TextViewCore( TextView* pOuterTextView )
    : ViewCore( pOuterTextView,
    			"div",
    			std::map<String, String> { {"white-space", "pre-wrap"} }  )
    {
        setText( pOuterTextView->text() );        
    }
    
    void setText(const String& text) override
    {
    	// note that we have the pre-formatted style set. So we do not
    	// need to process linebreaks.
        _domObject.set("textContent", text.asUtf8());
    }    
};


}    
}


#endif


