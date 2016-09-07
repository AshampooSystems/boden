#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Button.h>
#include <bdn/Window.h>
#include <bdn/test/TestTextViewCore.h>
#include "TestAndroidViewCoreMixin.h"

using namespace bdn;


class TestAndroidTextViewCore : public bdn::test::TestAndroidViewCoreMixin< bdn::test::TestTextViewCore >
{
protected:

    void initCore() override
    {
        bdn::test::TestAndroidViewCoreMixin<bdn::test::TestTextViewCore>::initCore();

        _jTextView = bdn::android::JTextView( _jView.getRef_() );
    }

    void verifyCoreText() override
    {
        String expectedText = _pTextView->text();

        String text = _jTextView.getText();
        
        REQUIRE( text == expectedText );
    }


protected:
    bdn::android::JTextView _jTextView;
};

TEST_CASE("android.TextViewCore")
{
    P<TestAndroidTextViewCore> pTest = newObj<TestAndroidTextViewCore>();

    pTest->runTests();
}








