#pragma once

#include <bdn/ApplicationController.h>
#include <bdn/UIApplicationController.h>

namespace bdn
{

    /** Default app controller implementation for a boden test app.

        Usually you do not need to use this directly. Use the macro
       #BDN_TEST_APP_INIT() instead.
    */
    class TestAppController : public UIApplicationController
    {
      public:
        TestAppController();
        ~TestAppController();

        void beginLaunch(const AppLaunchInfo &launchInfo) override;
        void finishLaunch(const AppLaunchInfo &launchInfo) override;

      protected:
        class Impl;
        Impl *_impl;
    };
}

#define BDN_TEST_APP_INIT() BDN_APP_INIT(bdn::TestAppController)
