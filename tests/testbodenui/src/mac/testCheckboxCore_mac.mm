#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Window.h>
#include <bdn/Checkbox.h>
#include <bdn/test/TestCheckboxCore.h>
#include <bdn/mac/CheckboxCore.hh>
#include <bdn/mac/util.hh>

#import "TestMacChildViewCoreMixin.hh"

using namespace bdn;


class TestMacCheckboxCore : public bdn::test::TestMacChildViewCoreMixin< bdn::test::TestCheckboxCore >
{
protected:

    void initCore() override
    {
        bdn::test::TestMacChildViewCoreMixin< bdn::test::TestCheckboxCore >::initCore();
        
        _pNSButton = (NSButton*) _pNSView;
        REQUIRE( _pNSButton!=nullptr );
    }

    
    void verifyCoreLabel() override
    {
        String expectedLabel = _pCheckbox->label();
        
        String label = bdn::mac::macStringToString( _pNSButton.title );
        
        REQUIRE( label == expectedLabel );
    }
    
    void verifyCoreState() override
    {
        TriState expectedState = _pCheckbox->state();
        TriState state = bdn::mac::nsControlStateValueToTriState(_pNSButton.state);
        REQUIRE( state == expectedState );
    }

protected:
    NSButton* _pNSButton;
};

TEST_CASE("mac.CheckboxCore")
{
    P<TestMacCheckboxCore> pTest = newObj<TestMacCheckboxCore>();
    
    pTest->runTests();
}




