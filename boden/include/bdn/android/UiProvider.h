#ifndef BDN_ANDROID_UiProvider_H_
#define BDN_ANDROID_UiProvider_H_

namespace bdn
{
namespace android
{

class UiProvider;

}
}


#include <bdn/IUiProvider.h>

#include <bdn/android/ViewCore.h>
#include <bdn/android/JTextView.h>

#include <cmath>

namespace bdn
{
namespace android
{

class UiProvider : public Base, BDN_IMPLEMENTS IUiProvider
{
public:
    UiProvider()
    {
        _semDips = -1;
    }   

    
    String getName() const override;
    
    P<IViewCore> createViewCore(const String& coreTypeName, View* pView) override;


    double getSemSizeDips(ViewCore& viewCore);

    static UiProvider& get();


private:
    double _semDips;

};

}
}


#endif
