#ifndef BDN_TEST_TestTextViewCore_H_
#define BDN_TEST_TestTextViewCore_H_

#include <bdn/TextView.h>
#include <bdn/test/TestViewCore.h>
#include <bdn/TextView.h>

namespace bdn
{
namespace test
{
    
/** Helper for tests that verify ITextViewCore implementations.*/
class TestTextViewCore : public TestViewCore
{

protected:

    P<View> createView() override
    {
        return newObj<TextView>();
    }

    void setView(View* pView) override
    {
        TestViewCore::setView(pView);

        _pTextView = cast<TextView>( pView );
    }

    void runInitTests() override
    {
        TestViewCore::runInitTests();

        SECTION("text")
        {
            _pTextView->text() = "helloworld";
            initCore();
            verifyCoreText();
        }
    }

    void runPostInitTests() override
    {
        TestViewCore::runPostInitTests();

        SECTION("text")
        {
            SECTION("value")
            {
                _pTextView->text() = "helloworld";
                verifyCoreText();
            }
        }

        SECTION("wider text causes wider preferred size")
        {
            Size prefSizeBefore = _pTextView->calcPreferredSize();

            _pTextView->text() = "helloworld";
                
            Size prefSize = _pTextView->calcPreferredSize();

            // width must increase with a bigger text
            REQUIRE( prefSize.width > prefSizeBefore.width );

            // note that the height might or might not increase. But it cannot be smaller.
            REQUIRE( prefSize.height >= prefSizeBefore.height );

            // when we go back to the same text as before then the preferred size should
            // also be the same again
            _pTextView->text() = "";
    
            REQUIRE( _pTextView->calcPreferredSize() == prefSizeBefore );
        }

        SECTION("linebreaks cause multiline")
        {
            Size emptyTextPreferredSize = _pTextView->calcPreferredSize();

            _pTextView->text() = "hello";

            Size prefSizeBefore = _pTextView->calcPreferredSize();                   

            _pTextView->text() = "hello\nhello";

            Size prefSize = _pTextView->calcPreferredSize();                   

            // should have same width as before (since both lines have the same texts).
            REQUIRE( prefSize.width == prefSizeBefore.width);

            // should have roughly twice the height as before. Note that the exact height
            // can vary because the line spacing can be different.

            REQUIRE( prefSize.height >= prefSizeBefore.height*1.8 );
            REQUIRE( prefSize.height < prefSizeBefore.height*3 );

            // when we go back to empty text then we should get the original size
            _pTextView->text() = "";    
            REQUIRE( _pTextView->calcPreferredSize() == emptyTextPreferredSize );
        }

        SECTION("CRLF same as LF")
        {
            _pTextView->text() = "hello world\nbla";

            Size sizeLF = _pTextView->calcPreferredSize();                   

            _pTextView->text() = "hello world\r\nbla";

            Size sizeCRLF = _pTextView->calcPreferredSize();                   

            REQUIRE( sizeLF == sizeCRLF);
        }
    }



    /** Verifies that the text view core's text has the expected value
        (the text set in the outer TextView object's TextView::text() property.*/
    virtual void verifyCoreText()=0;


    P<TextView> _pTextView;
};


}
}

#endif

