#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Window.h>
#include <bdn/ColumnView.h>
#include <bdn/test/TestContainerViewCore.h>

#include <bdn/winuwp/UiProvider.h>
#include <bdn/winuwp/ContainerViewCore.h>
#include "TestWinuwpViewCoreMixin.h"

using namespace bdn;



class TestWinuwpContainerViewCore : public bdn::test::TestWinuwpViewCoreMixin< bdn::test::TestContainerViewCore >
{
protected:

    void initCore() override
    {
        TestWinuwpViewCoreMixin< TestContainerViewCore >::initCore();

        _pWinCanvas = dynamic_cast<::Windows::UI::Xaml::Controls::Canvas^>( _pWinFrameworkElement );
        REQUIRE( _pWinCanvas!=nullptr );
    }    

    bool coreCanCalculatePreferredSize() override
    {
        return false;
    }

    ::Windows::UI::Xaml::Controls::Canvas^ _pWinCanvas;
};



TEST_CASE("winuwp.ContainerViewCore")
{
    P<TestWinuwpContainerViewCore> pTest = newObj<TestWinuwpContainerViewCore>();

    pTest->runTests();
}







