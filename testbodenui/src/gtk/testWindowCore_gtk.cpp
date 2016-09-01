#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Window.h>
#include <bdn/test/TestWindowCore.h>
#include <bdn/gtk/UiProvider.h>
#include <bdn/gtk/WindowCore.h>
#include "TestGtkViewCoreMixin.h"

using namespace bdn;



class TestGtkWindowCore : public bdn::test::TestGtkViewCoreMixin< bdn::test::TestWindowCore >
{
protected:
    
    void initCore() override
    {
        bdn::test::TestGtkViewCoreMixin< bdn::test::TestWindowCore >::initCore();

        _pWindow->visible() = true;
    }

    void verifyCoreTitle() override
    {
        String expectedTitle = _pWindow->title();
        
        String title = gtk_window_get_title( GTK_WINDOW(_pGtkWidget) );
        
        REQUIRE( title == expectedTitle );
    }
    
    
        
    bool windowExists(GtkWindow* pWindow)
    {
        GList* pList = gtk_window_list_toplevels();
        
        GList* pEl = g_list_find(pList, pWindow);
        
        g_list_free(pList);
        
        return ( pEl != nullptr );
    }
    
    
    struct DestructVerificationInfo : public Base
    {
        DestructVerificationInfo(GtkWindow* pWindow)
        {
            this->pWindow = pWindow;
        }
        
        GtkWindow* pWindow;
    };
    
    P<IBase> createInfoToVerifyCoreUiElementDestruction() override
    {
        GtkWindow* pWindow = GTK_WINDOW(_pGtkWidget);
        
        // sanity check
        REQUIRE( windowExists( pWindow ) );

        return newObj<DestructVerificationInfo>( pWindow );
    }


    void verifyCoreUiElementDestruction(IBase* pVerificationInfo) override
    {
        GtkWindow* pWindow = cast<DestructVerificationInfo>( pVerificationInfo )->pWindow;

        // window should have been destroyed.
        REQUIRE( !windowExists(pWindow) );
    }
};

TEST_CASE("gtk.WindowCore")
{
    P<TestGtkWindowCore> pTest = newObj<TestGtkWindowCore>();

    pTest->runTests();
}




