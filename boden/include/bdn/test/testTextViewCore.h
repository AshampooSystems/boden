#ifndef BDN_TEST_testTextViewCore_H_
#define BDN_TEST_testTextViewCore_H_

#include <bdn/test/testViewCore.h>
#include <bdn/TextView.h>

namespace bdn
{
namespace test
{


/** Performs generic tests for the text view core that is currently associated with the specified TextView.

    Note that these tests cannot test the effects of some of the functions on the actual UI element
    implementation that the core accesses. So the unit tests for the concrete implementation should verify these
    effects in addition to executing these generic tests.
*/
inline void testTextViewCore(P<Window> pWindow, P<TextView> pTextView)
{
    P<ITextViewCore> pCore = cast<ITextViewCore>( pTextView->getViewCore() );
    REQUIRE( pCore!=nullptr );

    SECTION("ViewCore-base")
        testViewCore(pWindow, pTextView, true);
    
    SECTION("text")
    {
        // we cannot test the effects of the text change on the actual UI implementation here.
        // But we can test that it affects the preferred size and that it does not cause a crash or exception.

        Size prefSizeBefore = pTextView->calcPreferredSize();

        pTextView->text() = "helloworld";

        Size prefSize = pTextView->calcPreferredSize();

        // width must increase with a bigger text
        REQUIRE( prefSize.width > prefSizeBefore.width );

        // note that the height might or might not increase. But it cannot be smaller.
        REQUIRE( prefSize.height >= prefSizeBefore.height );

        // when we go back to the same label as before then the preferred size should
        // also be the same again
        pTextView->text() = "";

        REQUIRE( pTextView->calcPreferredSize() == prefSizeBefore );
    }
}

}
}

#endif

