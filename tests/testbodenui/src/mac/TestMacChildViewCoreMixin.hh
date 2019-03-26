#ifndef BDN_TEST_TestMacViewCore_H_
#define BDN_TEST_TestMacViewCore_H_

#include <bdn/View.h>
#import <bdn/mac/ChildViewCore.hh>
#import <bdn/mac/ViewCoreFactory.hh>
#import <bdn/mac/WindowCore.hh>
#import <bdn/mac/util.hh>

namespace bdn
{
    namespace test
    {

        /** A mixin class that adds implementations of mac view specific
         functionality on top of the base class specified in the template
         parameter BaseClass.*/
        template <class BaseClass> class TestMacChildViewCoreMixin : public BaseClass
        {
          protected:
            void initCore() override
            {
                BaseClass::initCore();

                _macChildViewCore = std::dynamic_pointer_cast<bdn::mac::ChildViewCore>(BaseClass::_view->viewCore());
                REQUIRE(_macChildViewCore != nullptr);

                _nsView = _macChildViewCore->nsView();
                REQUIRE(_nsView != nullptr);
            }

            std::shared_ptr<ViewCoreFactory> viewCoreFactory() override { return bdn::mac::ViewCoreFactory::get(); }

            void verifyCoreVisibility() override
            {
                bool expectedVisible = BaseClass::_view->visible;

                REQUIRE(_nsView.hidden == !expectedVisible);
            }

            Rect getFrameRect() const
            {
                // we never have to flip frame rects. Container views are
                // already configured so that the origin is top-left. And top
                // level Windows have an intermediate contentWindow parent that
                // is configured the same way.
                return bdn::mac::macRectToRect(_nsView.frame, -1);
            }

            void verifyInitialDummyCoreSize() override
            {
                Rect rect = getFrameRect();

                // even the dummy rects of newly created controls will sometimes
                // have a valid size from the start.
                REQUIRE(rect.height >= 0);
                REQUIRE(rect.width >= 0);
            }

            void verifyCorePosition() override
            {
                Point position = getFrameRect().position();
                Point expectedPosition = BaseClass::_view->position;

                REQUIRE(position == expectedPosition);
            }

            void verifyCoreSize() override
            {
                Size size = getFrameRect().size();
                Size expectedSize = BaseClass::_view->size;

                REQUIRE(size == expectedSize);
            }

            void verifyCorePadding() override
            {
                // the padding is not reflected in Cocoa properties.
                // So nothing to test here.
            }

            std::shared_ptr<bdn::mac::ChildViewCore> _macChildViewCore;
            NSView *_nsView;
        };
    }
}

#endif
