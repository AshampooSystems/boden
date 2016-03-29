
#include <bdn/init.h>
#include <bdn/App.h>


int main (int argc, char *argv[])
{
    bdn::App* pApp = bdn::App::get();
    pApp->init();

    return 0;
}


