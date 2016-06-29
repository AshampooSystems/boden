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
    

    
    String getName() const override;
    
    P<IViewCore> createViewCore(const String& coreTypeName, View* pView) override;


    static UiProvider& get();

};

}
}


#endif
