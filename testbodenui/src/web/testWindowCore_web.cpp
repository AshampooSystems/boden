#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Window.h>
#include <bdn/test/TestWindowCore.h>
#include <bdn/web/UiProvider.h>
#include <bdn/web/WindowCore.h>
#include "TestWebViewCoreMixin.h"

using namespace bdn;


class TestWebWindowCore : public bdn::test::TestWebViewCoreMixin< bdn::test::TestWindowCore >
{
protected:
    
    void initCore() override
    {
        bdn::test::TestWebViewCoreMixin< bdn::test::TestWindowCore >::initCore();
    }

    bool canManuallyChangeBounds() const override
    {
        return false;
    }

    Rect getViewRect() override
    {
        int width = _domObject["offsetWidth"].as<int>();
        int height = _domObject["offsetHeight"].as<int>();
        
        return Rect(0, 0, width, height);
    }

    void verifyCoreTitle() override
    {
        String expectedTitle = _pWindow->title();

        // the title is the DOM document title
        emscripten::val docVal = emscripten::val::global("document");

        String title = docVal["title"].as<std::string>();
        
        REQUIRE( title == expectedTitle );
    }
    
    struct DestructVerificationInfo : public Base
    {
        DestructVerificationInfo(const String& elementId)
        {
            this->elementId = elementId;
        }
        
        String elementId;
    };
    
    P<IBase> createInfoToVerifyCoreUiElementDestruction() override
    {
        // sanity check
        String elementId = _pWebCore->getHtmlElementId();

        emscripten::val doc = emscripten::val::global("document");

        emscripten::val el = doc.call<emscripten::val>("getElementById", elementId.asUtf8() );
        REQUIRE( !el.isNull() );
        REQUIRE( !el.isUndefined() );

        return newObj<DestructVerificationInfo>( elementId );
    }


    void verifyCoreUiElementDestruction(IBase* pVerificationInfo) override
    {
        String elementId = cast<DestructVerificationInfo>( pVerificationInfo )->elementId;

        // window should have been destroyed.
        emscripten::val doc = emscripten::val::global("document");
        
        emscripten::val el = doc.call<emscripten::val>("getElementById", elementId.asUtf8() );
        REQUIRE( el.isNull() );
    }
};

TEST_CASE("web.WindowCore")
{
    P<TestWebWindowCore> pTest = newObj<TestWebWindowCore>();

    pTest->runTests();
}






