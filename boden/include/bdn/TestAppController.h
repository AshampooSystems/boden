#ifndef BDN_TestAppController_H_
#define BDN_TestAppController_H_

#include <bdn/AppControllerBase.h>

namespace bdn
{

/** Default app controller implementation for a boden test app.
	
	Usually you do not need to use this directly. Use the macro #BDN_TEST_APP_INIT() instead.
*/
class TestAppController : public AppControllerBase
{
public:
	TestAppController();
	~TestAppController();

	void beginLaunch(const AppLaunchInfo& launchInfo) override;
	void finishLaunch(const AppLaunchInfo& launchInfo) override;

    void unhandledProblem(IUnhandledProblem& problem) override;
	
protected:
	class Impl;
	Impl* _pImpl;
};
	

}

#endif

