#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Checkbox.h>
#include <bdn/Window.h>
#include <bdn/test/TestCheckboxCore.h>
#include <bdn/ios/CheckboxCore.hh>

#import "TestIosViewCoreMixin.hh"

using namespace bdn;

class TestIosCheckboxCore : public bdn::test::TestIosViewCoreMixin< bdn::test::TestCheckboxCore >
{
protected:
    
    void initCore() override
    {
        bdn::test::TestIosViewCoreMixin< bdn::test::TestCheckboxCore >::initCore();
        
        _composite = (BdnIosCheckboxComposite*)_pUIView;
        REQUIRE( _composite!=nullptr );
    }
    
    void verifyCoreState() override
    {
        bool expectedState = _pCheckbox->state();
        bool state = _composite.checkbox.checkboxState;
        REQUIRE( state == expectedState );
    }
    
    void verifyCoreLabel() override
    {
        String expectedLabel = _pCheckbox->label();
        String label = bdn::ios::iosStringToString( _composite.uiLabel.text );
        REQUIRE( label == expectedLabel );
    }
    
protected:
    BdnIosCheckboxComposite* _composite;
};


TEST_CASE("ios.CheckboxCore")
{
    P<TestIosCheckboxCore> pTest = newObj<TestIosCheckboxCore>();
    
    pTest->runTests();
}

