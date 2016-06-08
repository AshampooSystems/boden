#ifndef BDN_CommandLineAppController_H_
#define BDN_CommandLineAppController_H_

#include <bdn/AppControllerBase.h>

namespace bdn
{


/** A default implementation for an app controller for commandline applications.

	The app controller handles graceful termination requests by setting a simple flag.
	It is the responsibility of the app itself to check this flag periodically and
	exit when it is set (see shouldExit() ).

	Example for checking for exit:

	\code

	int myAppFunc(const std::vector<String>& params)
	{
		while( !CommandLineAppController::shouldExit() )
		{
			// do stuff.
		}

		return 0;
	}

	\endcode
*/
class CommandLineAppController : public AppControllerBase
{
public:
	CommandLineAppController()
	{
		// ensure that the static variable for shouldExit is initialized.
        volatile bool& r = _shouldExitRef();
	}


	/** A static function that returns true if the app should exit as soon as possible.*/
	static bool shouldExit()
	{
		return _shouldExitRef();
	}

	virtual void onTerminate() override
    {
		_shouldExitRef() = true;
    }

	virtual void closeAtNextOpportunityIfPossible(int exitCode) override
	{
		_shouldExitRef() = true;
	}

protected:
	static volatile bool& _shouldExitRef()
	{
		static volatile bool shouldExit = false;

		return shouldExit;
	}
};

}

#endif

