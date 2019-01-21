#pragma once

#include <bdn/AppControllerBase.h>
#include <bdn/UiAppControllerBase.h>

namespace bdn
{

    /** Default app controller implementation for a boden test app.

        Usually you do not need to use this directly. Use the macro
       #BDN_TEST_APP_INIT() instead.
    */
    class TestAppController : public UiAppControllerBase
    {
      public:
        TestAppController();
        ~TestAppController();

        void beginLaunch(const AppLaunchInfo &launchInfo) override;
        void finishLaunch(const AppLaunchInfo &launchInfo) override;

        void unhandledProblem(IUnhandledProblem &problem) override;

      protected:
        class Impl;
        Impl *_impl;
    };
}

#define BDN_TEST_APP_INIT() BDN_APP_INIT(bdn::TestAppController)
