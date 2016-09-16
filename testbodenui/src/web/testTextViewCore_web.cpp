#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Button.h>
#include <bdn/Window.h>
#include <bdn/test/TestTextViewCore.h>
#include "TestWebViewCoreMixin.h"

using namespace bdn;


class TestWebTextViewCore : public bdn::test::TestWebViewCoreMixin< bdn::test::TestTextViewCore >
{
protected:

    void verifyCoreText() override
    {
    	String expectedText = _pTextView->text();

        String text = _domObject["textContent"].as<std::string>();
        
        REQUIRE( text == expectedText );
    }

    bool clipsPreferredWidthToAvailableWidth() const override
    {
    	// we will never get a size that exceeds availableWidth from a HTML element
    	// because the availableWidth is implemented as a max size constraint.
    	return true;
    }

    bool usesAllAvailableWidthWhenWrapped() const override
	{
		// unfortunately html elements will use up all available space when their
		// size would otherwise exceed it (even if they did text wrapping to accomodate
		// the smaller width)
		return true;
	}

};

TEST_CASE("web.TextViewCore")
{
    P<TestWebTextViewCore> pTest = newObj<TestWebTextViewCore>();

    pTest->runTests();
}








