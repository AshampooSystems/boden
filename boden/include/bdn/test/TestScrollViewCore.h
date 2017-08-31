#ifndef BDN_TEST_TestScrollViewCore_H_
#define BDN_TEST_TestScrollViewCore_H_

#include <bdn/test/TestViewCore.h>
#include <bdn/ScrollView.h>
#include <bdn/IScrollViewCore.h>

#include <bdn/test/ScrollViewLayoutTesterBase.h>

namespace bdn
{
namespace test
{


        

/** Helper for tests that verify IScrollViewCore implementations.*/
class TestScrollViewCore : public bdn::test::ScrollViewLayoutTesterBase< TestViewCore<Window> >
{
protected:


    P<View> createView() override
    {
        P<ScrollView> pScrollView = newObj<ScrollView>();
                
        return pScrollView;
    }

    void initCore() override
    {
        TestViewCore::initCore();
        
        _pScrollViewCore = cast<IScrollViewCore>( _pCore );
    }

    void setView(View* pView) override
    {
        TestViewCore::setView(pView);

        _pScrollView = cast<ScrollView>( pView );
    }

    void runInitTests() override
    {
        TestViewCore::runInitTests();
    }

    void runPostInitTests() override
    {
        TestViewCore::runPostInitTests();

        SECTION("preferred size and layout")
        {
            this->doPreferredSizeAndLayoutTests();
        }

        SECTION("scrollClientRectToVisible and visibleClientRect")
        {
            this->testScrollClientRectToVisible();
        }
    }



    
    P<ScrollView> getScrollView() override
    {
        return _pScrollView;
    }


    Size callCalcPreferredSize( const Size& availableSpace = Size::none() ) override
    {
        return _pScrollViewCore->calcPreferredSize( availableSpace );
    }  

    void prepareCalcLayout(const Size& viewPortSize) override
    {
        // we must force the viewport to have the requested size.
        initiateScrollViewResizeToHaveViewPortSize(viewPortSize);

        _viewPortSizeRequestedInPrepare = viewPortSize;
    }

    void calcLayoutAfterPreparation() override
    {
        // verify that the scroll view has a plausible size.
        Size scrollViewSize = _pScrollView->size();

        Rect expectedBounds( _pScrollView->position(), _viewPortSizeRequestedInPrepare );
        expectedBounds = _pScrollView->adjustBounds(expectedBounds, RoundType::nearest, RoundType::nearest);

        Size expectedSize = expectedBounds.getSize();

        REQUIRE( scrollViewSize>=expectedSize );

        // we do not know how big the viewport border is, but we assume that
        // it is less than 50 DIPs. If the following fails then it might be the
        // case that the border is actually bigger - in that case this test must be
        // adapted.
        REQUIRE( scrollViewSize <= expectedSize+Size(50,50) );
        
        // request layout explicitly again. Usually the resizing will have caused one,
        // but we want to make sure.
        return _pScrollView->needLayout( View::InvalidateReason::customDataChanged );
    }

    
    /** Causes the scroll view to have the specified viewport size when no scroll bars are shown.

        It is ok if the resizing happens asynchronously after the function has already returned.
    */
    virtual void initiateScrollViewResizeToHaveViewPortSize( const Size& viewPortSize)=0;


    virtual void testScrollClientRectToVisible()
    {
        P<ScrollView>   pScrollView = getScrollView();
        P<Button>       pButton = newObj<Button>();        

        // make the button bigger than the scroll view so that
        // it will scroll
        pButton->preferredSizeMinimum() = Size(1000, 1000);
        pButton->preferredSizeMaximum() = Size(1000, 1000);

        pScrollView->horizontalScrollingEnabled() = true;
        pScrollView->setContentView(pButton);

        initiateScrollViewResizeToHaveViewPortSize( Size(500, 500) );

        P<TestScrollViewCore> pThis = this;
                        
        CONTINUE_SECTION_WHEN_IDLE(pThis, pButton, pScrollView)
        {
            Size scrollViewSize = pScrollView->size();
            Size viewPortSize = pScrollView->visibleClientRect().get().getSize();
            Size clientSize = pButton->size();

            // verify that the scroll view initialization was successful
            REQUIRE( viewPortSize > Size(400, 400) );
            REQUIRE( viewPortSize < Size(600, 600) );

            REQUIRE( clientSize > Size(900, 900) );
            REQUIRE( clientSize < Size(1100, 1100) );

            SECTION("start to end")
            {
                SECTION("zero target area size")
                {
                    subTestScrollClientRectToVisible(
                        pThis,
                        pScrollView,
                        0, Size(),
                        0, clientSize,
                        0, Size(),
                        0, clientSize-viewPortSize);
                }

                SECTION("nonzero target area size")
                {
                    subTestScrollClientRectToVisible(
                        pThis,
                        pScrollView,
                        0, Size(),
                        -5, clientSize,
                        5, Size(),
                        0, clientSize-viewPortSize);
                }
            }

            SECTION("end to start")
            {
                SECTION("zero target area size")
                {
                    subTestScrollClientRectToVisible(
                        pThis,
                        pScrollView,
                        0, clientSize-viewPortSize,
                        0, Size(),
                        0, Size(),
                        0, Size() );
                }

                SECTION("nonzero target area size")
                {
                    subTestScrollClientRectToVisible(
                        pThis,
                        pScrollView,
                        0, clientSize-viewPortSize,
                        0, Size(),
                        5, Size(),
                        0, Size() );
                }
            }

            SECTION("start to almost end")
            {
                SECTION("zero target area size")
                {
                    subTestScrollClientRectToVisible(
                        pThis,
                        pScrollView,
                        0, Size(),
                        -5, clientSize,
                        0, Size(),
                        -5, clientSize-viewPortSize );
                }

                SECTION("nonzero target area size")
                {
                    subTestScrollClientRectToVisible(
                        pThis,
                        pScrollView,
                        0, Size(),
                        -10, clientSize,
                        5, Size(),
                        -5, clientSize-viewPortSize );
                }
            }

            SECTION("end to almost start")
            {
                SECTION("zero target area size")
                {
                    subTestScrollClientRectToVisible(
                        pThis,
                        pScrollView,
                        0, clientSize-viewPortSize,
                        5, Size(),
                        0, Size(),
                        5, Size() );
                }

                SECTION("nonzero target area size")
                {
                    subTestScrollClientRectToVisible(
                        pThis,
                        pScrollView,
                        0, clientSize-viewPortSize,
                        5, Size(),
                        5, Size(),
                        5, Size() );
                }
            }

            SECTION("area already visible")
            {
                SECTION("start of viewport")
                {
                    SECTION("zero target area size")
                    {
                        subTestScrollClientRectToVisible(
                            pThis,
                            pScrollView,
                            10, Size(),
                            10, Size(),
                            0, Size(),
                            10, Size() );
                    }

                    SECTION("nonzero target area size")
                    {
                        subTestScrollClientRectToVisible(
                            pThis,
                            pScrollView,
                            10, Size(),
                            10, Size(),
                            5, Size(),
                            10, Size() );
                    }
                }

                SECTION("end of viewport")
                {
                    SECTION("zero target area size")
                    {
                        subTestScrollClientRectToVisible(
                            pThis,
                            pScrollView,
                            10, Size(),
                            10, viewPortSize,
                            0, Size(),
                            10, Size() );
                    }

                    SECTION("nonzero target area size")
                    {
                        subTestScrollClientRectToVisible(
                            pThis,
                            pScrollView,
                            10, Size(),
                            5, viewPortSize,
                            5, Size(),
                            10, Size() );
                    }
                }
            }

            SECTION("part of target area barely not visible")
            {
                SECTION("start of viewport")
                {
                    SECTION("zero target area size")
                    {
                        subTestScrollClientRectToVisible(
                            pThis,
                            pScrollView,
                            10, Size(),
                            10-1, Size(),
                            0, Size(),
                            10-1, Size() );
                    }

                    SECTION("nonzero target area size")
                    {
                        subTestScrollClientRectToVisible(
                            pThis,
                            pScrollView,
                            10, Size(),
                            10-1, Size(),
                            5, Size(),
                            10-1, Size() );
                    }
                }

                SECTION("end of viewport")
                {
                    SECTION("zero target area size")
                    {
                        subTestScrollClientRectToVisible(
                            pThis,
                            pScrollView,
                            10, Size(),
                            10+1, viewPortSize,
                            0, Size(),
                            10+1, Size() );
                    }

                    SECTION("nonzero target area size")
                    {
                        subTestScrollClientRectToVisible(
                            pThis,
                            pScrollView,
                            10, Size(),
                            10+1-5, viewPortSize,
                            5, Size(),
                            10+1, Size() );
                    }
                }
            }


            SECTION("target position outside client area")
            {
                SECTION("negative position")
                {
                    SECTION("target area ends before 0")
                    {
                        SECTION("zero target area size")
                        {
                            subTestScrollClientRectToVisible(
                                pThis,
                                pScrollView,
                                10, Size(),
                                -100, Size(),
                                0, Size(),
                                0, Size() );
                        }

                        SECTION("nonzero target area size")
                        {
                            subTestScrollClientRectToVisible(
                                pThis,
                                pScrollView,
                                10, Size(),
                                -100, Size(),
                                5, Size(),
                                0, Size() );
                        }
                    }

                    SECTION("target area crosses 0")
                    {
                        subTestScrollClientRectToVisible(
                            pThis,
                            pScrollView,
                            10, Size(),
                            -100, Size(),
                            150, Size(),
                            0, Size() );
                    }

                    SECTION("target area crosses and exceeds viewport")
                    {
                        subTestScrollClientRectToVisible(
                            pThis,
                            pScrollView,
                            10, Size(),
                            -100, Size(),
                            150, viewPortSize,
                                    
                            // the target rect is bigger than the viewport, so we cannot
                            // make all of it visible.
                            // The left/top side of the target rect should get preference.
                            0, Size() );
                    }
                }

                SECTION("exceeds end")
                {
                    SECTION("target area starts after client area")
                    {
                        SECTION("zero target area size")
                        {
                            subTestScrollClientRectToVisible(
                                pThis,
                                pScrollView,
                                10, Size(),
                                1, clientSize,
                                0, Size(),
                                0, clientSize-viewPortSize );
                        }

                        SECTION("nonzero target area size")
                        {
                            subTestScrollClientRectToVisible(
                                pThis,
                                pScrollView,
                                10, Size(),
                                1, clientSize,
                                5, Size(),
                                0, clientSize-viewPortSize );
                        }
                    }

                    SECTION("target area crosses end")
                    {
                        subTestScrollClientRectToVisible(
                            pThis,
                            pScrollView,
                            10, Size(),
                            -10, clientSize,
                            20, Size(),
                            0, clientSize-viewPortSize );
                    }

                    SECTION("target area crosses end and exceeds viewport")
                    {
                        subTestScrollClientRectToVisible(
                            pThis,
                            pScrollView,
                            10, Size(),
                            -10, clientSize,
                            20, viewPortSize,
                            0, clientSize-viewPortSize );
                    }
                }

                SECTION("positive infinity")
                {
                    SECTION("zero target area size")
                    {
                        subTestScrollClientRectToVisible(
                            pThis,
                            pScrollView,
                            10, Size(),
                            std::numeric_limits<double>::infinity(), Size(),
                            0, Size(),
                            0, clientSize-viewPortSize );
                    }

                    SECTION("nonzero target area size")
                    {
                        subTestScrollClientRectToVisible(
                            pThis,
                            pScrollView,
                            10, Size(),
                            std::numeric_limits<double>::infinity(), Size(),
                            5, Size(),
                            0, clientSize-viewPortSize );
                    }
                }

                SECTION("negative infinity")
                {
                    SECTION("zero target area size")
                    {
                        subTestScrollClientRectToVisible(
                            pThis,
                            pScrollView,
                            10, Size(),
                            - std::numeric_limits<double>::infinity(), Size(),
                            0, Size(),
                            0, Size() );
                    }

                    SECTION("nonzero target area size")
                    {
                        subTestScrollClientRectToVisible(
                            pThis,
                            pScrollView,
                            10, Size(),
                            - std::numeric_limits<double>::infinity(), Size(),
                            5, Size(),
                            0, Size() );
                    }
                }
            }
        };
    }

    

    P<ScrollView>       _pScrollView;
    P<IScrollViewCore>  _pScrollViewCore;


private:
    
    enum class TestDir_
    {
        horz,
        vert
    };

    static double& comp(Size& s, TestDir_ dir)
    {
        if(dir==TestDir_::horz)
            return s.width;
        else
            return s.height;
    }

    static double& comp(Point& p, TestDir_ dir)
    {
        if(dir==TestDir_::horz)
            return p.x;
        else
            return p.y;
    }

    static Point compToPoint(double c, TestDir_ dir)
    {
        if(dir==TestDir_::horz)
            return Point(c, 0);
        else
            return Point(0, c);
    }

    static Size compToSize(double s, TestDir_ dir)
    {
        if(dir==TestDir_::horz)
            return Size(s, 0);
        else
            return Size(0, s);
    }

    static void subTestScrollClientRectToVisible_Dir(
        TestDir_ dir,
        P<IBase> pKeepAliveDuringTest,
        P<ScrollView> pScrollView,    
        double initialPos,
        Size initialPosAdd,
        double targetPos,
        Size targetPosAdd,
        double targetSize,
        Size targetSizeAdd,
        double expectedPos,
        Size expectedPosAdd )
    {
        Size scrolledAreaSize = pScrollView->getContentView()->size();

        initialPos += comp(initialPosAdd, dir);
        targetPos += comp(targetPosAdd, dir);
        targetSize += comp(targetSizeAdd, dir);
        expectedPos += comp(expectedPosAdd, dir);

        pScrollView->scrollClientRectToVisible( Rect( compToPoint(initialPos, dir), pScrollView->visibleClientRect().get().getSize() ) );

        BDN_CONTINUE_SECTION_WHEN_IDLE( pKeepAliveDuringTest, pScrollView, initialPos, targetPos, targetSize, expectedPos, dir)
        {
            Rect visibleRectBefore = pScrollView->visibleClientRect();

            Rect expectedInitialRect( compToPoint( initialPos, dir), visibleRectBefore.getSize() );
            Rect adjustedExpectedInitialRect_down = pScrollView->getContentView()->adjustBounds( expectedInitialRect, RoundType::down, RoundType::nearest );
            Rect adjustedExpectedInitialRect_up = pScrollView->getContentView()->adjustBounds( expectedInitialRect, RoundType::up, RoundType::nearest );

            // check if the initial position is as expected
            REQUIRE_ALMOST_EQUAL( visibleRectBefore.x, expectedInitialRect.x, std::fabs(adjustedExpectedInitialRect_up.x - adjustedExpectedInitialRect_down.x) );
            REQUIRE_ALMOST_EQUAL( visibleRectBefore.y, expectedInitialRect.y, std::fabs(adjustedExpectedInitialRect_up.y - adjustedExpectedInitialRect_down.y) );        
            
            pScrollView->scrollClientRectToVisible( Rect( compToPoint(targetPos, dir), compToSize(targetSize, dir) ) );

            BDN_CONTINUE_SECTION_WHEN_IDLE( pKeepAliveDuringTest, pScrollView, initialPos, targetPos, targetSize, expectedPos, dir, visibleRectBefore)
            {
                Rect visibleRect = pScrollView->visibleClientRect();

                Rect expectedRect( compToPoint( expectedPos, dir), visibleRectBefore.getSize() );
                Rect adjustedExpectedRect_down = pScrollView->getContentView()->adjustBounds( expectedRect, RoundType::down, RoundType::nearest );
                Rect adjustedExpectedRect_up = pScrollView->getContentView()->adjustBounds( expectedRect, RoundType::up, RoundType::nearest );

                // allow the visible rect to be adjusted just like a view bounds rect would be.                
                REQUIRE_ALMOST_EQUAL( visibleRect.x, expectedRect.x, std::fabs(adjustedExpectedRect_up.x - adjustedExpectedRect_down.x) );
                REQUIRE_ALMOST_EQUAL( visibleRect.y, expectedRect.y, std::fabs(adjustedExpectedRect_up.y - adjustedExpectedRect_down.y) );

                // Size should not have changed
                REQUIRE( visibleRect.getSize() == visibleRectBefore.getSize() );
            };
        };
    }

    static void subTestScrollClientRectToVisible(
        P<IBase> pKeepAliveDuringTest,
        P<ScrollView> pScrollView,
        double initialPos,
        Size initialPosAdd,
        double targetPos,
        Size targetPosAdd,
        double targetSize,
        Size targetSizeAdd,
        double expectedPos,
        Size expectedPosAdd )
    {
        SECTION("vertical")
        {
            subTestScrollClientRectToVisible_Dir(
                TestDir_::vert,
                pKeepAliveDuringTest,
                pScrollView,
                initialPos,
                initialPosAdd,
                targetPos,
                targetPosAdd,
                targetSize,
                targetSizeAdd,
                expectedPos,
                expectedPosAdd );
        }

        SECTION("horizontal")
        {
            subTestScrollClientRectToVisible_Dir(
                TestDir_::horz,
                pKeepAliveDuringTest,
                pScrollView,
                initialPos,
                initialPosAdd,
                targetPos,
                targetPosAdd,
                targetSize,
                targetSizeAdd,
                expectedPos,
                expectedPosAdd );
        }
    }



    Size _viewPortSizeRequestedInPrepare;
};




}
}

#endif

