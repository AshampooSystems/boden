#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Button.h>
#include <bdn/Window.h>
#include <bdn/test/testButtonCore.h>
#include <bdn/android/UiProvider.h>
#include <bdn/android/ButtonCore.h>
#include "testAndroidViewCore.h"

using namespace bdn;

TEST_CASE("ButtonCore-android")
{
    P<Window> pWindow = newObj<Window>( &bdn::android::UiProvider::get() );

    P<Button> pButton = newObj<Button>();

    pWindow->setContentView(pButton);

    SECTION("generic")
        bdn::test::testButtonCore(pWindow, pButton );        

    SECTION("android-view")
        bdn::android::test::testAndroidViewCore(pWindow, pButton, true);

    SECTION("android-button")
    {
        P<bdn::android::ButtonCore> pCore = cast<bdn::android::ButtonCore>( pButton->getViewCore() );
        REQUIRE( pCore!=nullptr );

        bdn::android::JButton jb = pCore->getJButton();
        REQUIRE( !jb.isNull_() );

        SECTION("label")
        {
            // setLabel should change the window test
            pButton->label() = "hello world";

            String text = jb.getText();

            REQUIRE( text == "hello world" );
        }                
    }
}




