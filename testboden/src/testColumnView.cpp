#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/ColumnView.h>
#include <bdn/Button.h>
#include <bdn/test/testView.h>
#include <bdn/test/MockViewCore.h>
#include <bdn/test/MockContainerViewCore.h>

using namespace bdn;


void testChildAlignment(
    P<bdn::test::ViewTestPreparer<ColumnView> > pPreparer,
    P< bdn::test::ViewWithTestExtensions<ColumnView> > pColumnView,
    P<Button> pButton,
    View::HorizontalAlignment horzAlign,
    View::VerticalAlignment vertAlign)
{
    // add a second button that is considerably bigger.
    // That will cause the column view to become bigger.
    P<Button> pButton2 = newObj<Button>();
    pButton2->padding() = UiMargin(500, 500 );

    pColumnView->addChildView(pButton2);

    if(pButton->horizontalAlignment()==horzAlign)
    {
        // change to another horizontal alignment, so that the setting
        // of the requested alignment is registered as a change
        pButton->horizontalAlignment() = (horzAlign==View::HorizontalAlignment::left) ? View::HorizontalAlignment::right : View::HorizontalAlignment::left;
    }

    if(pButton->verticalAlignment()==vertAlign)
    {
        // change to another vertical alignment, so that the setting
        // of the requested alignment is registered as a change
        pButton->verticalAlignment() = (vertAlign==View::VerticalAlignment::top) ? View::VerticalAlignment::bottom : View::VerticalAlignment::top;
    }

    CONTINUE_SECTION_WHEN_IDLE(pPreparer, pColumnView, pButton, horzAlign, vertAlign)
    {
        int layoutCountBefore = cast<bdn::test::MockViewCore>(pColumnView->getViewCore())->getLayoutCount();

        Rect buttonBoundsBefore = Rect( pButton->position(), pButton->size() );

        SECTION("horizontal")
        {
            pButton->horizontalAlignment() = horzAlign;

            CONTINUE_SECTION_WHEN_IDLE(pPreparer, pColumnView, pButton, horzAlign, layoutCountBefore)
            {
                // but layout should have happened
                REQUIRE( cast<bdn::test::MockViewCore>(pColumnView->getViewCore())->getLayoutCount() == layoutCountBefore+1 );

                Margin margin = pButton->uiMarginToDipMargin( pButton->margin());

                Rect bounds = Rect( pButton->position(), pButton->size() );
                Rect containerBounds = Rect( pColumnView->position(), pColumnView->size() );

                // sanity check: the button should be smaller than the columnview
                // unless the alignment is "expand"
                if(horzAlign!=View::HorizontalAlignment::expand)
                    REQUIRE( bounds.width < containerBounds.width );
                
                // and the view should now be aligned accordingly.
                if(horzAlign==View::HorizontalAlignment::left)
                {
                    REQUIRE( bounds.x==margin.left );
                }
                else if(horzAlign==View::HorizontalAlignment::center)
                {
                    REQUIRE( bounds.x == margin.left + (containerBounds.width - (bounds.width+margin.left+margin.right) )/2  );
                }
                else if(horzAlign==View::HorizontalAlignment::right)
                {
                    REQUIRE( bounds.x == containerBounds.width - margin.right - bounds.width  );
                }
                else if(horzAlign==View::HorizontalAlignment::expand)
                {
                    REQUIRE( bounds.x == margin.left);
                    REQUIRE( bounds.width == containerBounds.width - margin.left - margin.right );
                }
            };
        }
        
        SECTION("vertical")
        {
            pButton->verticalAlignment() = vertAlign;

            CONTINUE_SECTION_WHEN_IDLE(pPreparer, pColumnView, pButton, vertAlign, layoutCountBefore, buttonBoundsBefore)
            {
                // layout should have been invalidated
                REQUIRE( cast<bdn::test::MockViewCore>(pColumnView->getViewCore())->getLayoutCount() == layoutCountBefore+1 );

                // vertical alignment should have NO effect in a column view.
                Rect bounds = Rect( pButton->position(), pButton->size() );

                REQUIRE( bounds == buttonBoundsBefore );
            };
        }
    };
}


void verifyPixelMultiple(double val)
{
    // the mock view we use simulates 3 physical pixels per DIP.
    double physicalPixels = val * 3;

    // so the value should be reasonably close to an integer value
    REQUIRE_ALMOST_EQUAL( physicalPixels, std::round(physicalPixels), 0.0000001 );
}

void verifyPixelMultiple(Size size)
{
    verifyPixelMultiple(size.width);
    verifyPixelMultiple(size.height);
}

void verifyPixelMultiple(Point point)
{
    verifyPixelMultiple(point.x);
    verifyPixelMultiple(point.y);
}


TEST_CASE("ColumnView")
{
    // test the generic view properties of Button
    SECTION("View-base")
        bdn::test::testView<ColumnView>();

    SECTION("ColumnView-specific")
	{
		P<bdn::test::ViewTestPreparer<ColumnView> >         pPreparer = newObj< bdn::test::ViewTestPreparer<ColumnView> >();
		P< bdn::test::ViewWithTestExtensions<ColumnView> >  pColumnView = pPreparer->createView();
		P<bdn::test::MockContainerViewCore>                 pCore = cast<bdn::test::MockContainerViewCore>( pColumnView->getViewCore() );

		REQUIRE( pCore!=nullptr );

        P<Button> pButton = newObj<Button>();

        pButton->adjustAndSetBounds( Rect(10, 10, 10, 10) );

        SECTION("no child view")
        {   
            SECTION("getChildList")
            {
                std::list< P<View> > childList;
                pColumnView->getChildViews(childList);

                REQUIRE( childList.empty() );
            }

            SECTION("removeAllChildViews")
            {
                pColumnView->removeAllChildViews();

                std::list< P<View> > childList;
                pColumnView->getChildViews(childList);

                REQUIRE( childList.empty() );
            }

            SECTION("addChildView")
            {
                CONTINUE_SECTION_WHEN_IDLE(pPreparer, pColumnView, pButton, pCore)
                {
                    int layoutCountBefore = cast<bdn::test::MockViewCore>(pColumnView->getViewCore())->getLayoutCount();

                    pColumnView->addChildView(pButton);

                    // let scheduled property updates propagate
                    CONTINUE_SECTION_WHEN_IDLE(pPreparer, pColumnView, pButton, pCore, layoutCountBefore)
                    {
                        // should cause a layout update.
                        REQUIRE( cast<bdn::test::MockViewCore>(pColumnView->getViewCore())->getLayoutCount()==layoutCountBefore+1 );                

                        Size preferredSize = pColumnView->calcPreferredSize();

                        Size buttonPreferredSize = pButton->calcPreferredSize();

                        REQUIRE( preferredSize!=Size(0,0) );

                        // the column view must ensure that the button gets a valid size for our mock display.
                        // So the button's preferred size must be rounded up to full mock pixels. We have 3 mock
                        // pixels per DIP, so that is what we should get
                        Rect buttonBounds( Point(), buttonPreferredSize );
                        Rect adjustedButtonBounds = pCore->adjustBounds(buttonBounds, RoundType::nearest, RoundType::up);

                        REQUIRE( preferredSize == adjustedButtonBounds.getSize()  );
                    };            
                };
            }
        }

        SECTION("with child view")
        {
            pColumnView->addChildView(pButton);

            pPreparer->getWindow()->requestAutoSize();

            CONTINUE_SECTION_WHEN_IDLE( pPreparer, pColumnView, pButton, pCore)
            {
                SECTION("child margins")
                {
                    Size preferredSizeBefore = pColumnView->calcPreferredSize();
                    int layoutCountBefore = cast<bdn::test::MockViewCore>(pColumnView->getViewCore())->getLayoutCount();

                    pButton->margin() = UiMargin(1, 2, 3, 4);

                    CONTINUE_SECTION_WHEN_IDLE(pPreparer, pColumnView, pButton, pCore, preferredSizeBefore, layoutCountBefore)
                    {
                        // should cause a layout update for the column view
                        REQUIRE( cast<bdn::test::MockViewCore>(pColumnView->getViewCore())->getLayoutCount()==layoutCountBefore+1 );                

                        Size preferredSize = pColumnView->calcPreferredSize();

                        REQUIRE( preferredSize == preferredSizeBefore+Margin(1,2,3,4) );
                    };         
                }

                SECTION("child alignment")
                {
                    for(int horzAlign = (int)View::HorizontalAlignment::left; horzAlign<=(int)View::HorizontalAlignment::expand; horzAlign++)
                    {
                        for(int vertAlign = (int)View::VerticalAlignment::top; vertAlign<=(int)View::VerticalAlignment::expand; vertAlign++)
                        {
                            SECTION( toString(horzAlign)+", "+toString(vertAlign) )
                            {
                                SECTION("no margin")
                                    testChildAlignment(pPreparer, pColumnView, pButton, (View::HorizontalAlignment) horzAlign, (View::VerticalAlignment)vertAlign );

                                SECTION("with margin")
                                {
                                    pButton->margin() = UiMargin( 10, 20, 30, 40);

                                    testChildAlignment(pPreparer, pColumnView, pButton, (View::HorizontalAlignment) horzAlign, (View::VerticalAlignment)vertAlign );
                                }
                            }
                        }
                    }
                }
                
                
                SECTION("position and size pixel aligned")
                {
                    // add a weird margin to the button to bring everything out of pixel alignment
                    pButton->margin() = UiMargin( 0.1234567 );

                    CONTINUE_SECTION_WHEN_IDLE( pPreparer, pColumnView, pButton, pCore )
                    {
                        verifyPixelMultiple( pButton->position() );
                        verifyPixelMultiple( pButton->size() );
                    };
                }

                
                SECTION("calcContainerPreferredSize")
                {
                    SECTION("availableWidth bigger than needed")
                    {
                        Size unrestrictedSize = pColumnView->calcContainerPreferredSize();

                        Size size = pColumnView->calcContainerPreferredSize( Size(unrestrictedSize.width+1, Size::componentNone()) );
                
                        // should be the same as the unresctricted size
                        REQUIRE_ALMOST_EQUAL( size, unrestrictedSize, Size(0.0000001, 0.0000001) );
                    }

                    SECTION("availableWidth exactly same as needed")
                    {
                        Size unrestrictedSize = pColumnView->calcContainerPreferredSize();

                        Size size = pColumnView->calcContainerPreferredSize( Size(unrestrictedSize.width, Size::componentNone()) );
                
                        // should be the same as the unresctricted size
                        REQUIRE_ALMOST_EQUAL( size, unrestrictedSize, Size(0.0000001, 0.0000001) );
                    }

                    SECTION("availableWidth smaller than needed")
                    {
                        Size unrestrictedSize = pColumnView->calcContainerPreferredSize();
                        Size size = pColumnView->calcContainerPreferredSize( Size(unrestrictedSize.width / 2, Size::componentNone()) );

                        // should still report almost the unrestricted size since none of the child views can be shrunk.
                        REQUIRE_ALMOST_EQUAL( size, unrestrictedSize, Size(0.0000001, 0.0000001) );                        
                    }        
                }

                
                SECTION("getChildList")
                {
                    std::list< P<View> > childList;
                    pColumnView->getChildViews(childList);

                    REQUIRE( childList.size() == 1);
                    REQUIRE( childList.front() == cast<View>(pButton) );
                }

                SECTION("removeAllChildViews")
                {
                    pColumnView->removeAllChildViews();

                    std::list< P<View> > childList;
                    pColumnView->getChildViews(childList);

                    REQUIRE( childList.empty() );

                    REQUIRE( pButton->getParentView() == nullptr );
                }
                
                SECTION("child views detached before destruction begins")
                {            
                    struct LocalTestData_ : public Base
                    {
                        bool destructorRun = false;
                        int childParentStillSet = -1;
                        int childListEmpty = -1;
                    };

                    P<LocalTestData_> pData = newObj<LocalTestData_>();

            
                    pColumnView->setDestructFunc(
                        [pData, pButton]( bdn::test::ViewWithTestExtensions<ColumnView>* pColView )
                        {
                            pData->destructorRun = true;
                            pData->childParentStillSet = (pButton->getParentView()!=nullptr) ? 1 : 0;

                            std::list< P<View> > childList;
                            pColView->getChildViews(childList);
                            pData->childListEmpty = (childList.empty() ? 1 : 0);
                        } );

                    BDN_CONTINUE_SECTION_WHEN_IDLE(pData, pButton)
                    {
                        // All test objects should have been destroyed by now.
                        // First verify that the destructor was even called.
                        REQUIRE( pData->destructorRun );

                        // now verify what we actually want to test: that the
                        // content view's parent was set to null before the destructor
                        // of the parent was called.
                        REQUIRE( pData->childParentStillSet == 0 );

                        // the child should also not be a child of the parent
                        // from the parent's perspective anymore.
                        REQUIRE( pData->childListEmpty == 1 );
                    };
                }

            };
        }

        SECTION("multiple child views properly arranged")
        {
            pColumnView->addChildView(pButton);

            P<Button> pButton2 = newObj<Button>();
            pColumnView->addChildView(pButton2);
            
            pPreparer->getWindow()->requestAutoSize();

            Margin m;
            Margin m2;

            SECTION("no margins")
            {
                m = Margin(0);
                m2 = Margin(0);
            }

            SECTION("with margins")
            {
                m = Margin(10, 20, 30, 40);
                m2 = Margin(11, 22, 33, 44);
            }

            pButton->margin() = UiMargin(m.top, m.right, m.bottom, m.left );
            pButton2->margin() = UiMargin(m2.top, m2.right, m2.bottom, m2.left );

            CONTINUE_SECTION_WHEN_IDLE( pPreparer, pColumnView, pButton, pButton2, pCore, m, m2)
            {
                Rect bounds = Rect( pButton->position(), pButton->size());
                Rect bounds2 = Rect( pButton2->position(), pButton2->size());

                REQUIRE( bounds.x == m.left);
                REQUIRE( bounds.y == m.top);
                // width and height should have been rounded up to full pixels.
                // Since our mock view has 3 pixels per DIP, we need to round up accordingly.
                REQUIRE( bounds.width == stableScaledRoundUp(pButton->calcPreferredSize().width, 3) );
                REQUIRE( bounds.height == stableScaledRoundUp(pButton->calcPreferredSize().height,3) );

                REQUIRE( bounds2.x == m2.left );
                REQUIRE( bounds2.y == bounds.y + bounds.height + m.bottom + m2.top );
                REQUIRE( bounds2.width == stableScaledRoundUp( pButton2->calcPreferredSize().width, 3) );
                REQUIRE( bounds2.height == stableScaledRoundUp( pButton2->calcPreferredSize().height, 3) );
            };
        }
        
	}	
}


