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
    
    /** Returns the size of 1 sem in DIPs.*/
    double getSemSizeDips() const
    {
        return _semDips;    
    }
    

private:
    double _semDips;
};

}
}


#endif
