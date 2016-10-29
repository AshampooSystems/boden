#ifndef BDN_WIN32_AppRunner_H_
#define BDN_WIN32_AppRunner_H_

#include <bdn/AppRunnerBase.h>

namespace bdn
{
namespace win32
{
    
    
class AppRunner : public AppRunnerBase
{
public:

	static int commandLineEntry(AppController* pAppController, int argc, const char** argv)
	{
		AppRunner runner;

		runner.setArgumentsInLocaleEncoding(argc, argv);

		return runner.main();
	}

	static int uiEntry(AppController* pAppController, int showCommand)
	{

	}

	void main();

protected:
	void platformSpecificInit() override;
};
  		

    
}    
}

#endif

