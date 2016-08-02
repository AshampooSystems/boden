#ifndef BDN_TEST_MockUiProvider_H_
#define BDN_TEST_MockUiProvider_H_

#include <bdn/IUiProvider.h>
#include <bdn/test/MockWindowCore.h>
#include <bdn/test/MockButtonCore.h>

#include <bdn/test.h>

namespace bdn
{
namespace test
{


/** A Ui provider that implements a "fake" UI that does not actually show anything
    visible, but behaves otherwise like a "real" UI.

    All the UI elements created by the provider also record some statistics about the
    calls that are made on them and they allow access to their internal
    state. This is useful for UI testing.    

    The MockUiProvider is intended for UI tests. You can specify the UI provider
    to the top level #Window instance you use. Pass a MockUiProvider instance there and
    your UI code will run in a "fake" UI world that you can examine and manipulate
    for your tests. That allows you to test your application UI easily without
    having to modify the UI code (except for passing the UI provider to the top level window).

    Example:

    \code

    P<bdn::test::MockUiProvider> pUiProvider = newObj<bdn::test::MockUiProvider>();

    P<Window> pMyWindow = newObj<Window>( pUiProvider );

    // set up your UI as normal here.
    ...
    
    // You can access the mock UI core elements by calling View::getViewCore() and casting
    // to the appropriate Mock class.
    // For example:

    P<Button> pButton = newObj<Button>();
    pButton->label() == "MyLabel";
    pMyWindow->setContentView(pButton);     // this will cause the button core to be created

    P<MockButtonCore> pButtonCore = cast<MockButtonCore>( pButton->getViewCore() );

    // you can then verify the properties of the core like this:
    REQUIRE( pButtonCore->getLabel() == "MyLabel" );

    // various statistics are also available
    REQUIRE( pButtonCore->getLabelChangeCount() == 1);

    \endcore
    */
class MockUiProvider : public Base, BDN_IMPLEMENTS IUiProvider
{
public:

	String getName() const
	{
		return "mock";
	}


	/** Returns the number of window cores that the UI provider has created.*/
    int getWindowCoresCreated() const
    {
        return _windowCoresCreated;
    }

    /** Returns the number of button cores that the UI provider has created.*/
    int getButtonCoresCreated() const
    {
        return _buttonCoresCreated;
    }
    
    
	P<IViewCore> createViewCore(const String& coreTypeName, View* pView)
	{
		BDN_REQUIRE_IN_MAIN_THREAD();

		if(coreTypeName==Window::getWindowCoreTypeName())
		{
			_windowCoresCreated++;

			_pLastCreatedCore = newObj<MockWindowCore>( cast<Window>(pView) );
			return _pLastCreatedCore;
		}
		else if(coreTypeName==Button::getButtonCoreTypeName())
		{
			_buttonCoresCreated++;

			_pLastCreatedCore = newObj<MockButtonCore>( cast<Button>(pView) );
			return _pLastCreatedCore;
		}
		else
			throw ViewCoreTypeNotSupportedError(coreTypeName);
	}
    
protected:
    int _windowCoresCreated = 0;
	int _buttonCoresCreated = 0;

	P<IViewCore> _pLastCreatedCore;
};




}
}

#endif