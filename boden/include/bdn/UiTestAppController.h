#ifndef BDN_UiTestAppController_H_
#define BDN_UiTestAppController_H_

#include <bdn/AppControllerBase.h>

namespace bdn
{

/** Default app controller implementation for a boden test app with a normal UI.
	See CommandLineTestAppController for a similar implementation for commandline apps.

	Usually you do not need to use this directly. Use the macro #BDN_TEST_APP_INIT() instead.
*/
class UiTestAppController : public AppControllerBase
{
public:
	UiTestAppController();
	~UiTestAppController();

	void beginLaunch(const AppLaunchInfo& launchInfo) override;
	void finishLaunch(const AppLaunchInfo& launchInfo) override;

	void onTerminate() override;
	
protected:

	class Impl;
	Impl* _pImpl;
};
	

}

#endif

