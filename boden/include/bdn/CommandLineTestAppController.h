#ifndef BDN_CommandLineTestAppController_H_
#define BDN_CommandLineTestAppController_H_

#include <bdn/AppControllerBase.h>

namespace bdn
{

/** Default app controller implementation for a commandline boden test app.
	See UiTestAppController for a similar implementation for UI apps.

	Usually you do not need to use this directly. Use the macro #BDN_TEST_APP_INIT() instead.
*/
class CommandLineTestAppController : public AppControllerBase
{
public:
	CommandLineTestAppController();
	~CommandLineTestAppController();

	void beginLaunch(const AppLaunchInfo& launchInfo) override;
	void finishLaunch(const AppLaunchInfo& launchInfo) override;

	void onTerminate() override;

    void unhandledProblem(IUnhandledProblem& problem) override;
	
protected:

	class Impl;
	Impl* _pImpl;
};
	

}

#endif

