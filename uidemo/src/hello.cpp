#include <bdn/init.h>
#include <bdn/Window.h>
#include <bdn/ColumnView.h>
#include <bdn/Button.h>
#include <bdn/TextView.h>
#include <bdn/ScrollView.h>

#include <bdn/appInit.h>
#include <bdn/AppControllerBase.h>

#include <bdn/property.h>

using namespace bdn;


class Model : public Base
{
public:
    Model()
    {
        _morphingTextCounter = -1;
        changeMorphingText();
    }
    
    BDN_PROPERTY(int, helloCounter, setHelloCounter );

    /** A text that changes when changeMorphingText is called.
        The different text variations are significantly different in their length.*/
    BDN_PROPERTY( String, morphingText, setMorphingText );

    
    void changeMorphingText()
    {
        _morphingTextCounter++;

        int sel = _morphingTextCounter % 3;
        if(sel==0)
            setMorphingText( "Short text" );
        else if (sel==1)
            setMorphingText( "This is a single-line, medium sized text. Lorem ipsum." );
        else
            setMorphingText( "This is a long text that spans multiple lines and also has some free lines.\nLorem ipsum dolor sit amet, consectetuer adipiscing elit.\nAenean commodo ligula eget dolor.\n\nAenean massa. Cum sociis natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus." );
    }
    
protected:
    int _morphingTextCounter;
};


class ViewModel : public Base
{
public:
    ViewModel(Model* pModel)
    {
        _pModel = pModel;
        
        // Connect our read-only morphingText property to the
        // model's read-write morphingText property
        BDN_BIND_TO_PROPERTY( *this, setMorphingText, *_pModel, morphingText );
        
        // connect our helloMessage to the helloCounter.
        // Note that we use a filter here to transform the integer counter
        // to a string for our property.
        auto helloMessageFilter = [](int newCounter)
        {
            String  message = "Hello World";
            if(newCounter>0)
                message += " "+std::to_string(newCounter);
            
            return message;
        };
        BDN_BIND_TO_PROPERTY_WITH_FILTER( *this, setHelloMessage, *_pModel, helloCounter, helloMessageFilter );
    }
    
    BDN_PROPERTY_WITH_CUSTOM_ACCESS( String, public, helloMessage, protected, setHelloMessage );
    BDN_PROPERTY_WITH_CUSTOM_ACCESS( String, public, morphingText, protected, setMorphingText );
    
    void increaseHelloCounter()
    {
        _pModel->setHelloCounter( _pModel->helloCounter() + 1);
    }
    
    void changeMorphingText()
    {
        _pModel->changeMorphingText();
    }

protected:
    P<Model>            _pModel;
};


class MainViewController : public Base //ViewControllerBase
{
public:
    MainViewController(ViewModel* pViewModel)
    {
		_pViewModel = pViewModel;

        _pWindow = newObj<Window>();
		_pWindow->setTitle( "hello" );
        
       
		P<ColumnView> pColumnView = newObj<ColumnView>();

		_pHelloMessageButton = newObj<Button>();

        // we want the hello message on the button
        BDN_BIND_TO_PROPERTY( *_pHelloMessageButton, setLabel, *_pViewModel, helloMessage );

        _pHelloMessageButton->setMargin( UiMargin( 10, 10, 10, 10) );
		_pHelloMessageButton->setHorizontalAlignment( View::HorizontalAlignment::center );

		pColumnView->addChildView( _pHelloMessageButton );
        _pHelloMessageButton->onClick().subscribeParamless( weakMethod(this, &MainViewController::buttonClicked) );

        _pMorphingTextView = newObj<TextView>();

        // show the morphing text in the text view
        BDN_BIND_TO_PROPERTY( *_pMorphingTextView, setText, *_pViewModel, morphingText );
        _pMorphingTextView->setMargin( UiMargin( 10, 10, 10, 10) );

        pColumnView->addChildView( _pMorphingTextView );

        _pScrollView = newObj<ScrollView>();

        // limit the maximum size. We simply want the scroll view to fill the available width
        // and have a height of 100 dips.
        _pScrollView->setPreferredSizeMinimum( Size( 0, 100) );
        _pScrollView->setPreferredSizeMaximum( Size( 0, 100) );
        _pScrollView->setHorizontalAlignment( View::HorizontalAlignment::expand );
        

        _pScrollView->setPadding( UiMargin( 5, 5, 5, 5 ) );
        _pScrollView->setMargin( UiMargin( 10, 10, 10, 10) );
        
        _pScrolledTextView = newObj<TextView>();        

        _pScrolledTextView->setText( "Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet." );
        _pScrollView->setContentView(_pScrolledTextView);

        pColumnView->addChildView( _pScrollView );
        
		_pWindow->setContentView( pColumnView );

		_pWindow->requestAutoSize();
		_pWindow->requestCenter();

		_pWindow->setVisible( true );
    }

protected:
	
    void buttonClicked()
    {
        _pViewModel->increaseHelloCounter();
        _pViewModel->changeMorphingText();

        // wait until the model changes have propagated to the UI, then autosize
        P<Window> pWindow = _pWindow;
        asyncCallFromMainThreadWhenIdle(
            [pWindow]()
            {
                pWindow->requestAutoSize();
            } );

    }

    P<ViewModel>    _pViewModel;
    
    P<Window>       _pWindow;
    P<Button>	    _pHelloMessageButton;
    
    P<TextView>     _pMorphingTextView;
    P<ScrollView>   _pScrollView;
    P<TextView>     _pScrolledTextView;
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



