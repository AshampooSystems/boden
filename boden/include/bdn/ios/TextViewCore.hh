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
    
    
    Size calcPreferredSize( const Size& availableSpace = Size::none() ) const override
    {
        // we want the preferred width hint to act as the wrap width where
        // the view wraps its text. To achieve that we incorporate it into
        // the available width value.
        Size          availableSpaceToUse(availableSpace);
        P<const View> pView = getOuterViewIfStillAttached();
        if(pView!=nullptr)
        {
            Size hint = pView->preferredSizeHint();
            
            // ignore the height hint - the view cannot change its width
            // to match a certain height, only the other way round.
            hint.height = Size::componentNone();
            
            availableSpaceToUse.applyMaximum( hint );
        }
        
        // now pass that to the base class implementation
        return ViewCore::calcPreferredSize( availableSpaceToUse );
    }
    
protected:

    double getFontSize() const override
    {
        return _uiLabel.font.pointSize;
    }
    
    
private:
    UILabel*    _uiLabel;
};



}
}

#endif
