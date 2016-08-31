#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Window.h>
#include <bdn/test/TestWindowCore.h>
#include <bdn/win32/UiProvider.h>
#include <bdn/win32/WindowCore.h>
#include "TestWin32ViewCoreMixin.h"

using namespace bdn;


class TestWin32WindowCore : public bdn::test::TestWin32ViewCoreMixin< bdn::test::TestWindowCore >
{
protected:
    
    void initCore() override
    {
        bdn::test::TestWin32ViewCoreMixin< bdn::test::TestWindowCore >::initCore();

        _pWindow->visible() = true;
    }

    void verifyCoreTitle() override
    {
        String expectedTitle = _pWindow->title();

        String title = bdn::win32::Win32Window::getWindowText(_hwnd);
        
        REQUIRE( title == expectedTitle );
    }
    
    struct DestructVerificationInfo : public Base
    {
        DestructVerificationInfo(HWND hwnd)
        {
            this->hwnd = hwnd;
        }
        
        HWND hwnd;
    };
    
    P<IBase> createInfoToVerifyCoreUiElementDestruction() override
    {
        // sanity check
        REQUIRE( ::IsWindow(_hwnd) );

        return newObj<DestructVerificationInfo>( _hwnd );
    }


    void verifyCoreUiElementDestruction(IBase* pVerificationInfo) override
    {
        HWND hwnd = cast<DestructVerificationInfo>( pVerificationInfo )->hwnd;

        // window should have been destroyed.
        REQUIRE( !::IsWindow(hwnd) );
    }
};

TEST_CASE("win32.WindowCore")
{
    P<TestWin32WindowCore> pTest = newObj<TestWin32WindowCore>();

    pTest->runTests();
}


