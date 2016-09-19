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


	/** Returns true if the text view implementation will never return a preferred width
		that exceeds the available width, even if the text cannot be wrapped to make it
		smaller than the available width.

		Note that the preferred behaviour is for the text view to report a bigger preferred
		size if the text cannot be wrapped to fit the available width.
		I.e. the preferred behaviour is the one that corresponds to
		clipsPreferredWidthToAvailableWidth returning false.

		The default implementation returns false.
		*/
	virtual bool clipsPreferredWidthToAvailableWidth() const
	{
		return false;
	}
    
    
    /** Returns true if the text view implementation will wrap at character boundaries
        if a single word does not fit in the available space.
        The preferred behaviour is to NOT wrap at character boundaries and instead clip the word.
        
        The default implementation returns false.
        */
    virtual bool wrapsAtCharacterBoundariesIfWordDoesNotFit() const
    {
        return false;
    }


    /** Returns true if the text view implementation uses all the available width
    	when the text is wrapped (i.e. if the availableWidth is smaller than the text view's
    	unconstrained width).

    	The preferred behaviour would be for the text view to use the width that is actually needed
    	for the wrapped	text (which usually ends up being slightly smaller than availableWidth).
    	But some implementations cannot do that and will always return exactly availableWidth if
    	the availableWidth is smaller than the unconstrained width.

		The default implementation returns false.
		*/
	virtual bool usesAllAvailableWidthWhenWrapped() const
	{
		return false;
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

	void testPreferredSize(int availableHeight)
	{
		// note that the available height should have NO influence on the result,
		// since the text cannot be made smaller in height (only in width by line-breaking)

		SECTION("wider text causes wider preferred size")
		{
			Size prefSizeBefore = _pTextView->calcPreferredSize(-1, availableHeight);

			_pTextView->text() = "helloworld";
                
			Size prefSize = _pTextView->calcPreferredSize(-1, availableHeight);

			// width must increase with a bigger text
			REQUIRE( prefSize.width > prefSizeBefore.width );

			// note that the height might or might not increase. But it cannot be smaller.
			REQUIRE( prefSize.height >= prefSizeBefore.height );

			// when we go back to the same text as before then the preferred size should
			// also be the same again
			_pTextView->text() = "";
    
			REQUIRE( _pTextView->calcPreferredSize(-1, availableHeight) == prefSizeBefore );
		}

		SECTION("linebreaks cause multiline")
		{
			Size emptyTextPreferredSize = _pTextView->calcPreferredSize(-1, availableHeight);

			_pTextView->text() = "hello";

			Size prefSizeBefore = _pTextView->calcPreferredSize(-1, availableHeight);                   

			_pTextView->text() = "hello\nhello";

			Size prefSize = _pTextView->calcPreferredSize(-1, availableHeight);                   

			// should have same width as before (since both lines have the same texts).
			REQUIRE( prefSize.width == prefSizeBefore.width);

			// should have roughly twice the height as before. Note that the exact height
			// can vary because the line spacing can be different.

			REQUIRE( prefSize.height >= prefSizeBefore.height*1.8 );
			REQUIRE( prefSize.height < prefSizeBefore.height*3 );

			// when we go back to empty text then we should get the original size
			_pTextView->text() = "";    
			REQUIRE( _pTextView->calcPreferredSize(-1, availableHeight) == emptyTextPreferredSize );
		}

		SECTION("CRLF same as LF")
		{
			_pTextView->text() = "hello world\nbla";

			Size sizeLF = _pTextView->calcPreferredSize(-1, availableHeight);                   

			_pTextView->text() = "hello world\r\nbla";

			Size sizeCRLF = _pTextView->calcPreferredSize(-1, availableHeight);                   

			REQUIRE( sizeLF == sizeCRLF);
		}

		SECTION("availableWidth has no effect if bigger than unconstrained width")
		{
			_pTextView->text() = "hello world";

			Size unconstrainedSize = _pTextView->calcPreferredSize(-1, availableHeight);

			REQUIRE( _pTextView->calcPreferredSize(unconstrainedSize.width+10, availableHeight) == unconstrainedSize );
		}

		SECTION("availableWidth has no effect if equal to unconstrained width")
		{
			_pTextView->text() = "hello world";

			Size unconstrainedSize = _pTextView->calcPreferredSize(-1, availableHeight);

			REQUIRE( _pTextView->calcPreferredSize(unconstrainedSize.width, availableHeight) == unconstrainedSize );
		}		

		SECTION("smaller availableWidth causes word wrap")
		{
			_pTextView->text() = "hellohello worldworld\nblabb";

			Size wrappedAtSecondPositionSize = _pTextView->calcPreferredSize(-1, availableHeight);

			_pTextView->text() = "hellohello\nworldworld blabb";

			Size wrappedAtFirstPositionSize = _pTextView->calcPreferredSize(-1, availableHeight);

			_pTextView->text() = "hellohello worldworld blabb";

			Size fullSize = _pTextView->calcPreferredSize(-1, availableHeight);

			REQUIRE( fullSize.width > wrappedAtSecondPositionSize.width );
			REQUIRE( fullSize.height < wrappedAtSecondPositionSize.height );
			
            // note that there might be some rounding errors with the width. So we accept 1 pixel difference
			REQUIRE_ALMOST_EQUAL( _pTextView->calcPreferredSize( wrappedAtSecondPositionSize.width+1, availableHeight ), wrappedAtSecondPositionSize, Size(1,0) );
            
			REQUIRE_ALMOST_EQUAL( _pTextView->calcPreferredSize( wrappedAtSecondPositionSize.width, availableHeight ), wrappedAtSecondPositionSize, Size(1,0) );

			if(usesAllAvailableWidthWhenWrapped())
			{
				// the implementation will return exactly the available width when text is wrapped.
				// Possibly with a small rounding difference.
				REQUIRE_ALMOST_EQUAL( _pTextView->calcPreferredSize( wrappedAtSecondPositionSize.width-1, availableHeight ),
								      Size(wrappedAtSecondPositionSize.width-1, wrappedAtFirstPositionSize.height),
								      Size(0.1, 0.1) );
			}
			else
			{
				REQUIRE_ALMOST_EQUAL( _pTextView->calcPreferredSize( wrappedAtSecondPositionSize.width-1, availableHeight ), wrappedAtFirstPositionSize, Size(1,0) );
			}

			REQUIRE_ALMOST_EQUAL( _pTextView->calcPreferredSize( wrappedAtFirstPositionSize.width, availableHeight ).width, wrappedAtFirstPositionSize.width, 1 );
		}

		SECTION("availableWidth below single word width")
		{
			_pTextView->text() = "hello";

			Size unrestrictedSize = _pTextView->calcPreferredSize(-1, availableHeight);

			// specifying an available width that is smaller than the word should not reduce the
			// preferred size.
			if(clipsPreferredWidthToAvailableWidth())
			{
				// this implementation will restrict the preferred width to the available width.
				// This is not optimal behaviour, but with some implementations it cannot be avoided.
				// so we accept it.
				REQUIRE_ALMOST_EQUAL(   _pTextView->calcPreferredSize(unrestrictedSize.width-1, availableHeight),
                                        Size(unrestrictedSize.width-1, unrestrictedSize.height),
                                        Size(1, 1) );   // the implementation might round to the nearest real pixel (which we assume is < 1 DIP)
			}
			else if(wrapsAtCharacterBoundariesIfWordDoesNotFit())
            {
                // the implementation will wrap at character boundaries.
                Size size = _pTextView->calcPreferredSize(unrestrictedSize.width-1, availableHeight);
                
                // width should be <= the specified width
                REQUIRE( size.width <= unrestrictedSize.width-1 );
                
                // should have gotten higher since wrapping occurred
                REQUIRE( size.height > unrestrictedSize.height);
            }
            else
				REQUIRE( _pTextView->calcPreferredSize(unrestrictedSize.width-1, availableHeight) == unrestrictedSize);
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

		SECTION("preferredSize")
		{
			SECTION("availableHeight=-1")
				testPreferredSize(-1);
			SECTION("availableHeight=0")
				testPreferredSize(0);
			SECTION("availableHeight=1")
				testPreferredSize(1);
			SECTION("availableHeight=1000")
				testPreferredSize(1000);		
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

