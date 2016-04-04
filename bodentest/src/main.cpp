
#if BDN_TARGET_WEB

#include <bdn/init.h>
#include <bdn/test.h>

int main (int argc, char * argv[])
{
    std::string s[] = {"bodentest.exe", "wideLocaleEncodingConversion"};
    
    char* p[] = { (char*)s[0].c_str(), (char*)s[1].c_str() };
    
    return bdn::runTestSession( 2, p );
}


#else

#include <bdn/testMain.h>

#endif
