#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Switch.h>
#include <bdn/Window.h>
#include <bdn/test/TestSwitchCore.h>

#import "TestMacChildViewCoreMixin.hh"
#import <bdn/mac/SwitchCore.hh>

using namespace bdn;

class TestMacSwitchCore : public bdn::test::TestMacChildViewCoreMixin< bdn::test::TestSwitchCore >
{
protected:

    void initCore() override
    {
        bdn::test::TestMacChildViewCoreMixin< bdn::test::TestSwitchCore >::initCore();
        
        _pSwitchComposite = (BdnMacSwitchComposite *) _pNSView;
        REQUIRE( _pSwitchComposite!=nullptr );
    }

    
    void verifyCoreLabel() override
    {
        String expectedLabel = _pSwitch->label();
        
        String label = bdn::mac::macStringToString( _pSwitchComposite.label.stringValue );
        
        REQUIRE( label == expectedLabel );
    }
    
    void verifyCoreOn() override
    {
        bool expectedOn = _pSwitch->on();
        bool on = _pSwitchComposite.bdnSwitch.on;
        REQUIRE( on == expectedOn );
    }
    
protected:
    BdnMacSwitchComposite* _pSwitchComposite;
};

TEST_CASE("mac.SwitchCore")
{
    P<TestMacSwitchCore> pTest = newObj<TestMacSwitchCore>();
    
    pTest->runTests();
}




