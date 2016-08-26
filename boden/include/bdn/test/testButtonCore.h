#ifndef BDN_TEST_testButtonCore_H_
#define BDN_TEST_testButtonCore_H_

#include <bdn/test/testViewCore.h>
#include <bdn/Button.h>

namespace bdn
{
namespace test
{


/** Performs generic tests for the button core that is currently associated with the specified Button.

    Note that these tests cannot test the effects of some of the functions on the actual UI element
    implementation that the core accesses. So the unit tests for the concrete implementation should verify these
    effects in addition to executing these generic tests.
*/
inline void testButtonCore(P<Window> pWindow, P<Button> pButton)
{
    P<IButtonCore> pCore = cast<IButtonCore>( pButton->getViewCore() );
    REQUIRE( pCore!=nullptr );

    SECTION("ViewCore-base")
        testViewCore(pWindow, pButton, true);
    
    SECTION("label")
    {
        // we cannot test the effects of the label change on the actual UI implementation here.
        // But we can test that it affects the preferred size and that it does not cause a crash or exception.

        Size prefSizeBefore = pButton->calcPreferredSize();

        pButton->label() = "helloworld";

        Size prefSize = pButton->calcPreferredSize();

        // width must increase with a bigger label
        REQUIRE( prefSize.width > prefSizeBefore.width );

        // note that the height might or might not increase. But it cannot be smaller.
        REQUIRE( prefSize.height >= prefSizeBefore.height );

        // when we go back to the same label as before then the preferred size should
        // also be the same again
        pButton->label() = "";

        REQUIRE( pButton->calcPreferredSize() == prefSizeBefore );
    }
}

}
}

#endif

