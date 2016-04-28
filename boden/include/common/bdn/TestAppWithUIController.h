#ifndef BDN_TestAppWithUIController_H_
#define BDN_TestAppWithUIController_H_

#include <bdn/UiAppControllerBase.h>

namespace bdn
{

/** Default app controller implementation for a boden test app that uses UI elements.

	Usually you do not need to use this directly. Use the macro #BDN_INIT_UI_TEST_APP() instead.
*/
class TestAppWithUiController : public UiAppControllerBase
{
public:
	TestAppWithUiController();
	~TestAppWithUiController();

	virtual void setArguments(const std::vector<String>& args) override;
		
	virtual void beginLaunch() override;
	virtual void finishLaunch() override;

	virtual void onTerminate() override;

protected:
	std::vector<String> _args;

	class Impl;
	Impl* _pImpl;
};
	

}

#endif

