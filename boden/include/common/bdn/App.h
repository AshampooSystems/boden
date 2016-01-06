#ifndef _BDN_APP_H_
#define _BDN_APP_H_

#include <bdn/Base.h>
#include <bdn/AppNotInstantiatedError.h>

namespace bdn
{

class App : public Base
{
public:
    App()
    {
        set(this);
    }
    
    virtual void init()
    {
        initUI();
    }
    
    virtual void initUI()=0;
    
    
    static App* get()
    {
        App* pApp = getGlobalInstanceRef();
        if(pApp==nullptr)
            throw AppNotInstantiatedError();
        return pApp;
    }
    
    static void set(App* pInstance)
    {
        getGlobalInstanceRef()=pInstance;
    }    
    
    
protected:
    static App*& getGlobalInstanceRef()
    {
        static App* globalInstance;
        return globalInstance;
    }
        

};
    
   
}



#endif


