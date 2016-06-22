#include <bdn/init.h>
#include <bdn/Window.h>
#include <bdn/ColumnView.h>
#include <bdn/Button.h>

#include <bdn/appInit.h>
#include <bdn/AppControllerBase.h>

#include <bdn/DefaultProperty.h>

using namespace bdn;


class Model : public Base
{
public:
    Model()
    {
    }
    
    Property<int>& helloCounter()
    {
        return _helloCounter;
    }
    
protected:
    DefaultProperty<int> _helloCounter;
};


class ViewModel : public Base
{
public:
    ViewModel(Model* pModel)
    {
        _pModel = pModel;

		_pModel->helloCounter().onChange().subscribeVoidMember<ViewModel>(_pHelloCounterSub, this, &ViewModel::updateHelloMessage);
		updateHelloMessage();
    }
    
    void increaseHelloCounter()
    {
        _pModel->helloCounter().set( _pModel->helloCounter().get()+1 );
    }
    
    ReadProperty<String>& helloMessage()
    {
        return _helloMessage;
    }
    
protected:
	void updateHelloMessage()
	{		
        String  message = "Hello World";
        
		int     counter = _pModel->helloCounter();        
        if(counter>0)
            message += " "+std::to_string(counter);
        
		_helloMessage = message;
    }
    
    P<Model>                    _pModel;
    DefaultProperty<String>     _helloMessage;

	P<IBase>					_pHelloCounterSub;
};


class MainViewController : public Base //ViewControllerBase
{
public:
    MainViewController(ViewModel* pViewModel)
    {
		_pViewModel = pViewModel;

        _pWindow = newObj<Window>();
		_pWindow->title() = "hello";

		_pWindow->sizingInfo().onChange().subscribeVoidMember<MainViewController>(_pWindowSizingSub, this, &MainViewController::windowSizingInfoChanged);

		P<ColumnView> pColumnView = newObj<ColumnView>();
		
		_pButton = newObj<Button>();
        _pButton->label().bind( _pViewModel->helloMessage() );

		_pButton->margin() = UiMargin(UiLength::sem, 2);
		_pButton->horizontalAlignment() = View::HorizontalAlignment::center;

		pColumnView->addChildView( _pButton );
                
        _pButton->onClick().subscribeVoidMember<MainViewController>(_pButtonClickSub, this, &MainViewController::buttonClicked);

		_pWindow->setContentView( pColumnView );

		_pWindow->requestAutoSize();
		_pWindow->requestCenter();

		_pWindow->visible() = true;
    }
    
    
protected:
	
	void windowSizingInfoChanged()
	{
		_pWindow->requestAutoSize();
		_pWindow->requestCenter();
	}

    void buttonClicked()
    {
        _pViewModel->increaseHelloCounter();
    }

    P<ViewModel> _pViewModel;
    
    P<Window>   _pWindow;
    P<Button>	_pButton;

	P<IBase>	_pButtonClickSub;
	P<IBase>	_pWindowSizingSub;
};


class AppController : public UiAppControllerBase
{
public:
   
    
    void beginLaunch(const AppLaunchInfo& launchInfo) override
    {
		_pModel = newObj<Model>();
		_pViewModel = newObj<ViewModel>(_pModel);

        _pMainViewController = newObj<MainViewController>(_pViewModel);
    }
    
protected:
    P<MainViewController>	_pMainViewController;
	P<ViewModel>			_pViewModel;
	P<Model>				_pModel;
};


BDN_INIT_UI_APP( AppController )



