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
    
       
    
    
private:
    double _semDips;
};



}
}


#endif
