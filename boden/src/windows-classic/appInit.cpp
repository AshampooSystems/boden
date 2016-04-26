#include <bdn/init.h>
#include <bdn/appInit.h>

#include <windows.h>

namespace bdn
{
    

int _uiAppMain( AppControllerBase* pAppController,
                int argCount,
                char* argv[],
                const std::map<String,String>& launchInfo )
{
    AppControllerBase::_set(pAppController);
        
    try
    {
        pAppController->beginLaunch( launchInfo );
        pAppController->finishLaunch( launchInfo );
    
        MSG msg;
        while(true)
        {
            int result = ::GetMessage(&msg, NULL, 0, 0);
            if(result==0)   // WM_QUIT received
                break;
            
            if(result==-1)
            {
                // error. Just exit.
                break;
            }
            
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }

    }
    catch(std::exception&)
    {
        // we will exit abnormally. Still call onTerminate.
        pAppController->onTerminate();
        
        // let error through.
        throw;
    }
    
    pAppController->onTerminate();
    
    return 0;
}
    

}



