#ifndef BDN_IOS_UiProvider_HH_
#define BDN_IOS_UiProvider_HH_

#include <bdn/IUiProvider.h>

namespace bdn
{
namespace ios
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
        XXX
        if(uiLength.unit==UiLength::sem)
            return (int)std::lround( uiLength.value * _semPixels );
        
        else if(uiLength.unit==UiLength::dip)
        {
            // See UiLength documentation for more information about the dip unit
            // and why this is correct.
            return (int)std::lround( uiLength.value );
        }
        
        else if(uiLength.unit==UiLength::realPixel)
            return (int)std::lround( uiLength.value );
        
        else
            throw InvalidArgumentError("Invalid UiLength unit passed to UiProvider::uiLengthToPixels: "+std::to_string((int)uiLength.unit) );
    }
    
    XXX
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
