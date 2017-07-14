#ifndef BDN_TEST_testCalcPreferredSize_H_
#define BDN_TEST_testCalcPreferredSize_H_

#include <bdn/View.h>
#include <bdn/TextView.h>

namespace bdn
{
namespace test
{


template<class ViewType>
inline bool shouldPrefererredWidthHintHaveAnEffect()
{
    // for most views the hint has no effect
    return false;
}

template<>
inline bool shouldPrefererredWidthHintHaveAnEffect<TextView>()
{
    // Text views can adjust their text wrapping. So the size hint should have an effect
    return true;
}


template<class ViewType>
inline bool shouldPrefererredHeightHintHaveAnEffect()
{
    // for most views the hint has no effect
    return false;
}



template<class ViewType, class ObjectType>
inline void _testCalcPreferredSize(P<View> pView, P<ObjectType> pObject, P<IBase> pKeepAliveDuringContinuations)
{
	SECTION("plausible")	
    {
        // we check elsewhere that padding is properly included in the preferred size
        // So here we only check that the preferred size is "plausible"

        Size prefSize = pObject->calcPreferredSize();

        REQUIRE( prefSize.width>=0 );
        REQUIRE( prefSize.height>=0 );
    }

            
    SECTION("padding influence")	
    {
        pView->padding() = UiMargin(0);

        Size zeroPaddingSize = pObject->calcPreferredSize();

        // increasing the padding
        pView->padding() = UiMargin( 100, 200, 300, 400);

        CONTINUE_SECTION_WHEN_IDLE(pView, pObject, zeroPaddingSize, pKeepAliveDuringContinuations)
        {
            Size sizeWithPadding = pObject->calcPreferredSize();

            // the increased padding should have increased the preferred size.
            // Note that we cannot really know by how much because there can be
            // hidden minimum padding values for certain views. I.e. the padding
            // we apply might be rounded up to that minimum.
            // But since we applied a rather large padding we can expect that 
            // this will make the view bigger
            REQUIRE( sizeWithPadding > zeroPaddingSize);
        };
    }
                
    SECTION("availableSize = preferredSize")	
    {
        SECTION("no padding")
        {
            // do nothing
        }
                    
        SECTION("with padding")
        {
            pView->padding() = UiMargin( 10, 20, 30, 40);
        }
                    
        Size prefSize = pObject->calcPreferredSize();
                    
        Size prefSizeRestricted = pObject->calcPreferredSize( prefSize );
                    
        REQUIRE( prefSize == prefSizeRestricted);
    }

    SECTION("preferredSizeMinimum influence")	
    {
        Size prefSizeBefore = pObject->calcPreferredSize();

        SECTION("smaller than preferred size")
        {
            pView->preferredSizeMinimum() = prefSizeBefore-Size(1,1);

            // must wait until change has propagated and caches have been invalidated
            CONTINUE_SECTION_WHEN_IDLE(pView, pObject, prefSizeBefore, pKeepAliveDuringContinuations)
            {
                Size prefSize = pObject->calcPreferredSize();

                REQUIRE( prefSize == prefSizeBefore);
            };
        }

        SECTION("same as preferred size")
        {
            pView->preferredSizeMinimum() = prefSizeBefore;

            // must wait until change has propagated and caches have been invalidated
            CONTINUE_SECTION_WHEN_IDLE(pView, pObject, prefSizeBefore, pKeepAliveDuringContinuations)
            {
                Size prefSize = pObject->calcPreferredSize();
    
                REQUIRE( prefSize == prefSizeBefore);
            };
        }
                    
        SECTION("width bigger than preferred width")
        {
            pView->preferredSizeMinimum() = Size( prefSizeBefore.width+1, -1 );

            // must wait until change has propagated and caches have been invalidated
            CONTINUE_SECTION_WHEN_IDLE(pView, pObject, prefSizeBefore, pKeepAliveDuringContinuations)
            {
                Size prefSize = pObject->calcPreferredSize();

                REQUIRE( prefSize == prefSizeBefore+Size(1, 0));
            };
        }

        SECTION("height bigger than preferred height")
        {
            pView->preferredSizeMinimum() = Size( -1, prefSizeBefore.height+1 );

            // must wait until change has propagated and caches have been invalidated
            CONTINUE_SECTION_WHEN_IDLE(pView, pObject, prefSizeBefore, pKeepAliveDuringContinuations)
            {
                Size prefSize = pObject->calcPreferredSize();

                REQUIRE( prefSize == prefSizeBefore+Size(0, 1));
            };
        }
    }

    SECTION("preferredSizeMaximum influence")	
    {
        // some views (for example some top level windows) have an intrinsic minimum size.
        // The result of calcPreferredSize will not go below that.
        // So to ensure that we are in a position to verify maxSize, we first have to ensure
        // that we are above that limit. We do that by artificially enlarging the preferred
        // size with a big padding.

        pView->padding() = UiMargin(300);

        Size prefSizeBefore = pObject->calcPreferredSize();

        SECTION("bigger than preferred size")
        {
            pView->preferredSizeMaximum() = prefSizeBefore+Size(1,1);

            // must wait until change has propagated and caches have been invalidated
            CONTINUE_SECTION_WHEN_IDLE(pView, pObject, prefSizeBefore, pKeepAliveDuringContinuations)
            {
                Size prefSize = pObject->calcPreferredSize();

                REQUIRE( prefSize == prefSizeBefore);
            };
        }

        SECTION("same as preferred size")
        {
            pView->preferredSizeMaximum() = prefSizeBefore;

            // must wait until change has propagated and caches have been invalidated
            CONTINUE_SECTION_WHEN_IDLE(pView, pObject, prefSizeBefore, pKeepAliveDuringContinuations)
            {
                Size prefSize = pObject->calcPreferredSize();

                REQUIRE( prefSize == prefSizeBefore);
            };
        }
                    
        SECTION("width smaller than preferred width")
        {
            pView->preferredSizeMaximum() = Size( prefSizeBefore.width-1, Size::componentNone() );

            // must wait until change has propagated and caches have been invalidated
            CONTINUE_SECTION_WHEN_IDLE(pView, pObject, prefSizeBefore, pKeepAliveDuringContinuations)
            {
                Size prefSize = pObject->calcPreferredSize();

                REQUIRE( prefSize.width < prefSizeBefore.width);
            };
        }

        SECTION("height smaller than preferred height")
        {
            pView->preferredSizeMaximum() = Size( Size::componentNone(), prefSizeBefore.height-1 );

            // must wait until change has propagated and caches have been invalidated
            CONTINUE_SECTION_WHEN_IDLE(pView, pObject, prefSizeBefore, pKeepAliveDuringContinuations)
            {
                Size prefSize = pObject->calcPreferredSize();

                REQUIRE( prefSize.height < prefSizeBefore.height );
            };
        }
    }
                                
    SECTION("with constrained width plausible")	
    {
        // this is difficult to test, since it depends heavily on what kind of view
        // we actually work with. Also, it is perfectly normal for different core implementations
        // to have different preferred size values for the same inputs.

        // So we can only test rough plausibility here.
        Size prefSize = pObject->calcPreferredSize();

        SECTION("unconditionalWidth")
        {
            // When we specify exactly the unconditional preferred width then we should get exactly the unconditional preferred height
            REQUIRE( pObject->calcPreferredSize( Size(prefSize.width, Size::componentNone()) ).height == prefSize.height );
        }

        SECTION("unconditionalWidth/2")
        {
            REQUIRE( pObject->calcPreferredSize( Size(prefSize.width/2, Size::componentNone()) ).height >= prefSize.height );
        }

        SECTION("zero")
        {
            REQUIRE( pObject->calcPreferredSize( Size(0, Size::componentNone()) ).height >= prefSize.height );
        }
    }

    SECTION("with constrained height plausible")	
    {
        Size prefSize = pObject->calcPreferredSize();

        SECTION("unconditionalHeight")
            REQUIRE( pObject->calcPreferredSize( Size(Size::componentNone(), prefSize.height) ).width == prefSize.width );

        SECTION("unconditionalHeight/2")
            REQUIRE( pObject->calcPreferredSize( Size(Size::componentNone(), prefSize.height/2) ).width >= prefSize.width );
        
        SECTION("zero")
            REQUIRE( pObject->calcPreferredSize( Size(Size::componentNone(), 0) ).width >= prefSize.width );
    }





    SECTION("preferredSizeHint influence")	
    {
        // some views (for example some top level windows) have an intrinsic minimum size.
        // The result of calcPreferredSize will not go below that.
        // So to ensure that we are in a position to verify maxSize, we first have to ensure
        // that we are above that limit. We do that by artificially enlarging the preferred
        // size with a big padding.

        pView->padding() = UiMargin(300);

        Size prefSizeBefore = pObject->calcPreferredSize();

        SECTION("bigger than preferred size hint")
        {
            pView->preferredSizeHint() = prefSizeBefore+Size(1,1);

            CONTINUE_SECTION_WHEN_IDLE(pView, pObject, pKeepAliveDuringContinuations, prefSizeBefore)
            {
                Size prefSize = pObject->calcPreferredSize();

                REQUIRE( prefSize == prefSizeBefore);
            };
        }

        SECTION("same as preferred size hint")
        {
            pView->preferredSizeHint() = prefSizeBefore;

            CONTINUE_SECTION_WHEN_IDLE(pView, pObject, pKeepAliveDuringContinuations, prefSizeBefore)
            {
                Size prefSize = pObject->calcPreferredSize();

                REQUIRE( prefSize == prefSizeBefore);
            };
        }
                    
        SECTION("width smaller than preferred width hint")
        {
            pView->preferredSizeHint() = Size( prefSizeBefore.width-1, Size::componentNone() );

            CONTINUE_SECTION_WHEN_IDLE(pView, pObject, pKeepAliveDuringContinuations, prefSizeBefore)
            {
                Size prefSize = pObject->calcPreferredSize();

                // it depends on the view whether or not the width hint has an effect
                if(shouldPrefererredWidthHintHaveAnEffect<ViewType>() )
                {
                    REQUIRE( prefSize.width < prefSizeBefore.width);
                
                    // the height may have increase as a result of the width being reduced.
                    // It may also have shrunk (for example, if an image is shrunk as the result
                    // of the hint.
                    // So we cannot test anything here, other than that we get a height that
                    // is >0
                    REQUIRE( prefSize.height > 0);
                }
                else
                    REQUIRE( prefSize == prefSizeBefore );
            };
        }

        SECTION("height smaller than preferred height hint")
        {
            pView->preferredSizeHint() = Size( Size::componentNone(), prefSizeBefore.height-1 );

            CONTINUE_SECTION_WHEN_IDLE(pView, pObject, pKeepAliveDuringContinuations, prefSizeBefore)
            {
                Size prefSize = pObject->calcPreferredSize();

                if( shouldPrefererredHeightHintHaveAnEffect<ViewType>() )
                {
                    REQUIRE( prefSize.height < prefSizeBefore.height);
                
                    REQUIRE( prefSize.width > 0);
                }
                else
                    REQUIRE( prefSize == prefSizeBefore );
            };
        }
    }


}


}
}

#endif
