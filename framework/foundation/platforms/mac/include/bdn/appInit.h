#pragma once

#include <bdn/ApplicationController.h>
#include <bdn/platform/macplatform.h>

#ifdef BDN_COMPILING_COMMANDLINE_APP

#include <bdn/genericAppEntry.h>

#define BDN_APP_INIT_WITH_CONTROLLER_CREATOR(appControllerCreator)                                                     \
    int main(int argc, char *argv[])                                                                                   \
    {                                                                                                                  \
        bdn::platform::MacHooks::init();                                                                               \
        return bdn::genericCommandLineAppEntry(appControllerCreator, argc, argv);                                      \
    }

#else

#include <bdn/mac/appEntry.h>

#define BDN_APP_INIT_WITH_CONTROLLER_CREATOR(appControllerCreator)                                                     \
    int main(int argc, char *argv[])                                                                                   \
    {                                                                                                                  \
        bdn::platform::MacHooks::init();                                                                               \
        return bdn::mac::uiAppEntry(appControllerCreator, argc, argv);                                                 \
    }

#endif

#define BDN_APP_INIT(appControllerClass)                                                                               \
    BDN_APP_INIT_WITH_CONTROLLER_CREATOR((([]() { return std::make_shared<appControllerClass>(); })))