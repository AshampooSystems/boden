#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/TextField.h>
#include <bdn/Window.h>
#include <bdn/test/TestTextFieldCore.h>

#import "TestMacChildViewCoreMixin.hh"

using namespace bdn;


class TestMacTextFieldCore : public bdn::test::TestMacChildViewCoreMixin< bdn::test::TestTextFieldCore >
{
protected:

    void initCore() override
    {
        bdn::test::TestMacChildViewCoreMixin< bdn::test::TestTextFieldCore >::initCore();
        
        _pNSTextField = (NSTextField*) _pNSView;
        REQUIRE(_pNSTextField!=nullptr);
    }

    
    void verifyCoreText() override
    {
        String expectedText = _pTextField->text();
        String text = bdn::mac::macStringToString(_pNSTextField.stringValue);
        REQUIRE(text == expectedText);
    }
    
protected:
    NSTextField* _pNSTextField;
};

TEST_CASE("mac.TextFieldCore")
{
    P<TestMacTextFieldCore> pTest = newObj<TestMacTextFieldCore>();
    
    pTest->runTests();
}



