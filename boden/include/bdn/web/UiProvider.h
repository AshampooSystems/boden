#ifndef BDN_WEB_UiProvider_H_
#define BDN_WEB_UiProvider_H_

#include <bdn/IUiProvider.h>

namespace bdn
{
namespace web
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
