#include <bdn/init.h>
#include <bdn/appInit.h>

#include <bdn/Frame.h>
#include <bdn/Button.h>
#include <bdn/Switch.h>

#include <bdn/test.h>

#include <string>

using namespace bdn;


class TestBodenUIAppController : public AppControllerBase
{
public:
    
    void beginLaunch(const std::map<String,String>& launchInfo) override
    {
        /*
        try
        {

            _pTestSession = new bdn::Session;
            
            int exitCode = _pTestSession->applyCommandLine(argc, argv);
            if(exitCode!=0)
            {
                // invalid commandline arguments. Exit.
                return;
            }
            
            if(!_pTestSession->prepareRun())
            {
                // only showing help. Just exit.
                return;
            }
            
            _pTestRunner = new TestRunner( _pTestSession->config() );
            
            // this is just a place holder frame so that we have something visible.
            Frame* pFrame = new Frame("Running tests...");
            pFrame->show();
            
            
            // schedule an event to call our first test.
            EventDispatcher::asyncCallFromMainThread( [this]()
                                                     {
                                                         this->runNextTest();
                                                     } );
            
            
        }
        catch( std::exception& ex )
        {
            bdn::cerr() << ex.what() << std::endl;
            
            int exitCode = (std::numeric_limits<int>::max)();
            
            // we want to exit
            exitIfPossible( exitCode );
        }*/
    }
    
};


BDN_INIT_UI_APP( TestBodenUIAppController )

/*

class MyApp : public App
{
public:
    MyApp()
    {
        _pTestSession = nullptr;
        _pTestRunner = nullptr;
    }
    
    ~MyApp()
    {
        if(_pTestSession!=nullptr)
            delete _pTestSession;
        
        if(_pTestRunner!=nullptr)
            delete _pTestRunner;
    }
    
    void runNextTest()
    {
        try
        {
            if(_pTestRunner->runNextTest())
            {
                // this was not the last test. So schedule another call.
                
                // Note that we do it this way (bit by bit) to ensure that
                // normal application events are dispatched correctly.
                EventDispatcher::asyncCallFromMainThread( [this]()
                                                         {
                                                             this->runNextTest();
                                                         } );
            }
            else
            {
                // no more tests. We want to exit.
                
                int exitCode = static_cast<int>( _pTestRunner->getTotals().assertions.failed );
                
                exitIfPossible( exitCode );
            }
        }
        catch( std::exception& ex )
        {
            bdn::cerr() << ex.what() << std::endl;
            
            int exitCode = (std::numeric_limits<int>::max)();
            
            // we want to exit
            exitIfPossible( exitCode );
        }
        
    }
    
    virtual void initUI() override
    {
           }
    
    
protected:
    Session*    _pTestSession;
    TestRunner* _pTestRunner;
    
};


MyApp _app;

 */


