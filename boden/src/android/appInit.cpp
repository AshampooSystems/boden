#include <bdn/init.h>
#include <bdn/appInit.h>


namespace bdn
{
    

int _uiAppMain( AppControllerBase* pAppController,
                int argCount,
                char* argv[],
                const std::map<String,String>& launchInfo )
{
    AppControllerBase::_set(pAppController);
    
#if BDN_PLATFORM_IOS
    return _iosUiAppMain(pAppController, argCount, argv);
    
#elif BDN_PLATFORM_OSX
    return _osxUiAppMain(pAppController, argCount, argv);
    
#else
    
    try
    {
#if BDN_PLATFORM_LINUX
        gtk_init (&argc, &argv);
#endif
        
        pAppController->beginLaunch( launchInfo );
        pAppController->finishLaunch( launchInfo );
    
#if BDN_PLATFORM_WINDOWS_CLASSIC
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
    
#elif BDN_PLATFORM_WEB
    
        // just exit. The emscripten compile flags we use ensure that the app keeps
        // running.
        return;
        
#elif BDN_PLATFORM_LINUX
        gtk_main ();
        
#else
        
#pragma UI app main loop not implemented. Invalid BDN_PLATFORM_XYZ definitions???
        
#endif
        
    }
    catch(std::exception& e)
    {
        // we will exit abnormally. Still call onTerminate.
        pAppController->onTerminate();
        
        // let error through.
        throw;
    }
    
    pAppController->onTerminate();
    
    return 0;
    
#endif
}
    

}



