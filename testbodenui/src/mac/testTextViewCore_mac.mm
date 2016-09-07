#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Button.h>
#include <bdn/Window.h>
#include <bdn/test/TestTextViewCore.h>
#import "TestMacChildViewCoreMixin.hh"

using namespace bdn;


class TestMacTextViewCore : public bdn::test::TestMacChildViewCoreMixin< bdn::test::TestTextViewCore >
{
protected:

    void initCore() override
    {
        TestMacChildViewCoreMixin< bdn::test::TestTextViewCore >::initCore();
        
        _pNSTextView = (NSTextView*) _pNSView;
        REQUIRE( _pNSTextView!=nullptr );
    }

   
    
    void runPostInitTests() override
    {
        TestMacChildViewCoreMixin< bdn::test::TestTextViewCore >::runPostInitTests();
        
        SECTION("not editable")
        {
            REQUIRE( _pNSTextView.editable==false );
        }
        
        SECTION("not selectable")
        {
            REQUIRE( _pNSTextView.selectable==false );
        }
        
        SECTION("not rich text")
        {
            REQUIRE( _pNSTextView.richText==false );
        }

        
    }



    void verifyCoreText() override
    {
        String expectedText = _pTextView->text();

        String text = bdn::mac::macStringToString( _pNSTextView.string );
        
        REQUIRE( text == expectedText );
    }
    
protected:
    NSTextView* _pNSTextView;
};

TEST_CASE("mac.TextViewCore")
{
    P<TestMacTextViewCore> pTest = newObj<TestMacTextViewCore>();

    pTest->runTests();
}








