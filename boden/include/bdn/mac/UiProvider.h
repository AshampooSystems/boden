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
    

    
    String getName() const override;
    
    P<IViewCore> createViewCore(const String& coreTypeName, View* pView) override;


    static P<UiProvider> get()
    {
        static SafeInit<UiProvider> init;
        
        return init.get();
    }

};

}
}


#endif
