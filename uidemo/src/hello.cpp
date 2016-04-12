#include <bdn/init.h>
#include <bdn/Frame.h>
#include <bdn/Button.h>
#include <bdn/Switch.h>

#include <bdn/appInit.h>


using namespace bdn;


class MainViewController : public ViewControllerBase
{
public:
    MainViewController()
    {
        _pFrame = newObj<Frame>("hello");
        
        _pButton = newObj<Button>(pFrame, "Hello World");
        
        _pSwitch = newObj<Switch>(pFrame, "This is a switch/checkbox");
    }
    
protected:
    P<Frame>    _pFrame;
    P<Button>   _pButton;
    P<Switch>   _pSwitch;
};


class AppController : public AppControllerBase
{
public:
    
    void onClick(ClickEvent& evt)
    {
        _counter++;
        
        dynamic_cast<Button*>(evt.getWindow())->setLabel("Hello World: "+std::to_string(_counter));
    }
    
    void beginLaunch(const std::map<String,String>& launchInfo) override
    {
        _pMainViewController = newObj<MainViewController>();       
        
        
        pButton->getClickEventSource()->subscribeMember<UiDemoAppController>(this, &UiDemoAppController::onClick );
        
        Switch* pSwitch =
        
        pFrame->show();
    }
    
protected:
    P<MainViewController> _pMainViewController;
};


BDN_INIT_UI_APP( AppController )




