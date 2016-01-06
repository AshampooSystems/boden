
#include <bdn/App.h>

int main()
{
    bdn::App* pApp = bdn::App::get();
    
    pApp->init();
    
    // just exit. The emscripten compile flags we use ensure that the app will remain
    // and continue executing.
    
    return 0;
}

