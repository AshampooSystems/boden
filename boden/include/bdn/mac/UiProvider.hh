#ifndef BDN_MAC_UiProvider_HH_
#define BDN_MAC_UiProvider_HH_

#include <bdn/IUiProvider.h>

namespace bdn
{
namespace mac
{

class UiProvider : public Base, BDN_IMPLEMENTS IUiProvider
{
public:
    UiProvider();
    
    String getName() const override;
    
    P<IViewCore> createViewCore(const String& coreTypeName, View* pView) override;

    static UiProvider& get();
    
    
    
    
    int				uiLengthToPixels(const UiLength& uiLength) const
    {
        if(uiLength.unit==UiLength::sem)
            return std::lround( uiLength.value * _semPixels );
        
        else if(uiLength.unit==UiLength::pixel96)
        {
            // See UiLength documentation for more information about the pixel96 unit
            // and why this is correct.
            return std::lround( uiLength.value );
        }
        
        else if(uiLength.unit==UiLength::realPixel)
            return std::lround( uiLength.value );
        
        else
            throw InvalidArgumentError("Invalid UiLength unit passed to UiProvider::uiLengthToPixels: "+std::to_string((int)uiLength.unit) );
    }
    
    
    Margin			uiMarginToPixelMargin(const UiMargin& margin) const
    {
        return Margin(
                      uiLengthToPixels(margin.top),
                      uiLengthToPixels(margin.right),
                      uiLengthToPixels(margin.bottom),
                      uiLengthToPixels(margin.left) );
    }
    
    
private:
    double _semPixels;
};



}
}


#endif
