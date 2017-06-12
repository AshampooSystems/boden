#include <bdn/init.h>
#include <bdn/Window.h>
#include <bdn/ColumnView.h>
#include <bdn/Button.h>
#include <bdn/TextView.h>
#include <bdn/ScrollView.h>

#include <bdn/appInit.h>
#include <bdn/AppControllerBase.h>

#include <bdn/DefaultProperty.h>

using namespace bdn;



class Model : public Base
{
public:
    Model()
    {
        _morphingTextCounter = -1;
        changeMorphingText();
    }
    
    Property<int>& helloCounter()
    {
        return _helloCounter;
    }

    
    /** A text that changes when changeMorphingText is called.
        The different text variations are significantly different in their length.*/
    Property<String>& morphingText()
    {
        return _morphingText;
    }

    void changeMorphingText()
    {
        _morphingTextCounter++;

        int sel = _morphingTextCounter % 3;
        if(sel==0)
            _morphingText = "Short text";
        else if (sel==1)
            _morphingText = "This is a single-line, medium sized text. Lorem ipsum.";
        else
            _morphingText = "This is a long text that spans multiple lines and also has some free lines.\nLorem ipsum dolor sit amet, consectetuer adipiscing elit.\nAenean commodo ligula eget dolor.\n\nAenean massa. Cum sociis natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus.";
    }
    
protected:
    DefaultProperty<int> _helloCounter;
    
    DefaultProperty<String> _morphingText;
    int                     _morphingTextCounter;
};


class ViewModel : public Base
{
public:
    ViewModel(Model* pModel)
    {
        _pModel = pModel;

		_pModel->helloCounter().onChange().subscribeParamless( weakMethod(this, &ViewModel::updateHelloMessage) );
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


    void changeMorphingText()
    {
        _pModel->changeMorphingText();
    }

    ReadProperty<String>& morphingText()
    {
        return _pModel->morphingText();
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
};


class MainViewController : public Base //ViewControllerBase
{
public:
    MainViewController(ViewModel* pViewModel)
    {
		_pViewModel = pViewModel;

        _pWindow = newObj<Window>();
		_pWindow->title() = "hello";

		_pWindow->sizingInfo().onChange().subscribeParamless( weakMethod(this, &MainViewController::windowSizingInfoChanged) );

		P<ColumnView> pColumnView = newObj<ColumnView>();

		_pHelloMessageButton = newObj<Button>();
        _pHelloMessageButton->label().bind( _pViewModel->helloMessage() );
		_pHelloMessageButton->margin() = UiMargin( UiLength::sem(2) );
		_pHelloMessageButton->horizontalAlignment() = View::HorizontalAlignment::center;
		pColumnView->addChildView( _pHelloMessageButton );                
        _pHelloMessageButton->onClick().subscribeParamless( weakMethod(this, &MainViewController::buttonClicked) );

        _pMorphingTextView = newObj<TextView>();
        _pMorphingTextView->text().bind( _pViewModel->morphingText() );
        _pMorphingTextView->margin() = UiMargin(UiLength::sem(0), UiLength::sem(2), UiLength::sem(2), UiLength::sem(2) );
        pColumnView->addChildView( _pMorphingTextView );

        _pScrollView = newObj<ScrollView>();

        // limit the maximum size. We simply want the scroll view to fill the available width
        // and have a height of 100 dips.
        _pScrollView->preferredSizeMinimum() = Size( 0, 100);
        _pScrollView->preferredSizeMaximum() = Size( 0, 100);
        _pScrollView->horizontalAlignment() = View::HorizontalAlignment::expand;

        
        _pScrolledTextView = newObj<TextView>();        

        _pScrolledTextView->text() = "Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet.";
        _pScrollView->setContentView(_pScrolledTextView);

        pColumnView->addChildView( _pScrollView );
        
		_pWindow->setContentView( pColumnView );

		_pWindow->requestAutoSize();
		_pWindow->requestCenter();

		_pWindow->visible() = true;
    }
    
    
protected:
	
	void windowSizingInfoChanged()
	{
		_pWindow->requestAutoSize();
	}

    void buttonClicked()
    {
        _pViewModel->increaseHelloCounter();
        _pViewModel->changeMorphingText();
    }

    P<ViewModel> _pViewModel;
    
    P<Window>   _pWindow;
    P<Button>	_pHelloMessageButton;
    
    P<TextView> _pMorphingTextView;
    P<ScrollView> _pScrollView;
    P<TextView>   _pScrolledTextView;
};


class AppController : public AppControllerBase
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


BDN_APP_INIT( AppController )



