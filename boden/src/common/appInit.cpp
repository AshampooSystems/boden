#include <bdn/init.h>
#include <bdn/appInit.h>

#include <bdn/test.h>

#include <iostream>

namespace bdn
{
    
	
int _commandLineAppMain(	std::function< int(const std::vector<String>&) > appFunc,
							AppControllerBase* pAppController,
							int argCount,
							char* argv[] )
{
    AppControllerBase::_set(pAppController);

	int result=0;
        
    try
    {
		std::vector<String> args;

		for(int i=0; i<argCount; i++)
			args.push_back( String::fromLocaleEncoding(argv[i]) );
		if(argCount==0)
			args.push_back("");	// always add the first entry.

		pAppController->setArguments(args);

		// launchInfo is empty
		std::map<String,String> launchInfo;
		pAppController->setLaunchInfo(launchInfo);
		
        pAppController->beginLaunch();
        pAppController->finishLaunch();			

		result = appFunc(args);
    }
    catch(std::exception& e)
    {
		std::cerr << e.what() << std::endl;

        // we will exit abnormally. Still call onTerminate.
        pAppController->onTerminate();
        
        // let error through.
        throw;
    }
    
    pAppController->onTerminate();
    
    return result;
}


int _commandLineTestAppFunc( const std::vector<String>& params )
{
	String programName = "";
	
	std::vector<const char*> args;
	args.push_back( programName.asUtf8Ptr() );

	for(auto p: params)
		args.push_back( p.asUtf8Ptr() );

	return bdn::runTestSession( args.size(), &args[0] );
}


}



