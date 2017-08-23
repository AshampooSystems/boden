#ifndef BDN_TEST_MockUiProvider_H_
#define BDN_TEST_MockUiProvider_H_

#include <bdn/IUiProvider.h>
#include <bdn/LayoutCoordinator.h>

#include <bdn/test/MockTextUi.h>

#include <bdn/ViewCoreTypeNotSupportedError.h>

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
    MockUiProvider()
    {
        _pLayoutCoordinator = newObj<LayoutCoordinator>();
        _pTextUi = newObj<MockTextUi>();
    }
    
	String getName() const
	{
		return "mock";
	}


	/** Returns the number of cores that the UI provider has created.*/
    int getCoresCreated() const
    {
        return _coresCreated;
    }


    /** Returns the layout coordinator object that all UI objects created by this
        provider share.*/
    P<LayoutCoordinator> getLayoutCoordinator()
    {
        return _pLayoutCoordinator;
    }

    P<ITextUi> getTextUi() override
    {
        return _pTextUi;
    }

    
	P<IViewCore> createViewCore(const String& coreTypeName, View* pView);
    
protected:
    int _coresCreated = 0;

    P<LayoutCoordinator> _pLayoutCoordinator;
    P<MockTextUi>        _pTextUi;
};




}
}

#endif