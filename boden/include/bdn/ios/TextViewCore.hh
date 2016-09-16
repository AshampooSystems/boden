#ifndef BDN_IOS_TextViewCore_HH_
#define BDN_IOS_TextViewCore_HH_

#include <bdn/ITextViewCore.h>
#include <bdn/ClickEvent.h>
#include <bdn/TextView.h>

#import <bdn/ios/ViewCore.hh>

namespace bdn
{
namespace ios
{
    
class TextViewCore : public ViewCore, BDN_IMPLEMENTS ITextViewCore
{
private:
    static UILabel* _createUILabel(TextView* pOuterTextView)
    {
        UILabel* label = [[UILabel alloc] initWithFrame:CGRectMake(0, 0, 0, 0)];
        
        label.numberOfLines = 0;
        
        return label;

    }
    
public:
    TextViewCore(TextView* pOuterTextView)
    : ViewCore(pOuterTextView, _createUILabel(pOuterTextView) )
    {
        _uiLabel = (UILabel*)getUIView();
        
        setText( pOuterTextView->text() );
    }
    
    
    UILabel* getUILabel()
    {
        return _uiLabel;
    }
    
    void setText(const String& text) override
    {
        _uiLabel.text = stringToIosString(text);
    }
    
private:
    UILabel*    _uiLabel;
};



}
}

#endif
