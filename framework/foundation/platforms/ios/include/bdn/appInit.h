#pragma once

#include <bdn/ios/appEntry.hh>
#include <bdn/platform/iosplatform.h>

#define BDN_APP_INIT_WITH_CONTROLLER_CREATOR(appControllerCreator)                                                     \
    int main(int argc, char *argv[])                                                                                   \
    {                                                                                                                  \
        bdn::platform::IOSHooks::init();                                                                               \
        return bdn::ui::ios::appEntry(appControllerCreator, argc, argv);                                               \
    }

#define BDN_APP_INIT(appControllerClass)                                                                               \
    BDN_APP_INIT_WITH_CONTROLLER_CREATOR((([]() { return std::make_shared<appControllerClass>(); })))
