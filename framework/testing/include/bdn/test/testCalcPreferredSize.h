#pragma once

#include <bdn/View.h>
#include <bdn/TextView.h>

namespace bdn
{
    namespace test
    {

        template <class ViewType> inline bool shouldPrefererredWidthHintHaveAnEffect()
        {
            // for most views the hint has no effect
            return false;
        }

        template <> inline bool shouldPrefererredWidthHintHaveAnEffect<TextView>()
        {
            // Text views can adjust their text wrapping. So the size hint
            // should have an effect
            return true;
        }

        template <class ViewType> inline bool shouldPrefererredHeightHintHaveAnEffect()
        {
            // for most views the hint has no effect
            return false;
        }

        template <class ViewType, class ObjectType>
        inline void _testCalcPreferredSize(std::shared_ptr<View> view, std::shared_ptr<ObjectType> object,
                                           std::shared_ptr<Base> keepAliveDuringContinuations)
        {
            SECTION("plausible")
            {
                // we check elsewhere that padding is properly included in the
                // preferred size So here we only check that the preferred size
                // is "plausible"

                Size prefSize = object->calcPreferredSize();

                REQUIRE(prefSize.width >= 0);
                REQUIRE(prefSize.height >= 0);
            }

            SECTION("padding influence")
            {
                view->padding = (UiMargin(0));

                Size zeroPaddingSize = object->calcPreferredSize();

                // increasing the padding
                view->padding = (UiMargin(100, 200, 300, 400));

                Size sizeWithPadding = object->calcPreferredSize();

                // the increased padding should have increased the preferred
                // size. Note that we cannot really know by how much because
                // there can be hidden minimum padding values for certain views.
                // I.e. the padding we apply might be rounded up to that
                // minimum. But since we applied a rather large padding we can
                // expect that this will make the view bigger
                REQUIRE(sizeWithPadding > zeroPaddingSize);
            }

            SECTION("availableSize = preferredSize")
            {
                SECTION("no padding")
                {
                    // do nothing
                }

                SECTION("with padding") { view->padding = (UiMargin(10, 20, 30, 40)); }

                Size prefSize = object->calcPreferredSize();

                Size prefSizeRestricted = object->calcPreferredSize(prefSize);

                REQUIRE(prefSize == prefSizeRestricted);
            }

            SECTION("preferredSizeMinimum influence")
            {
                Size prefSizeBefore = object->calcPreferredSize();

                SECTION("smaller than preferred size")
                {
                    view->preferredSizeMinimum = (prefSizeBefore - Size(1, 1));

                    Size prefSize = object->calcPreferredSize();

                    REQUIRE(prefSize == prefSizeBefore);
                }

                SECTION("same as preferred size")
                {
                    view->preferredSizeMinimum = (prefSizeBefore);

                    Size prefSize = object->calcPreferredSize();

                    REQUIRE(prefSize == prefSizeBefore);
                }

                SECTION("width bigger than preferred width")
                {
                    view->preferredSizeMinimum = (Size(prefSizeBefore.width + 1, -1));

                    Size prefSize = object->calcPreferredSize();

                    REQUIRE(prefSize == prefSizeBefore + Size(1, 0));
                }

                SECTION("height bigger than preferred height")
                {
                    view->preferredSizeMinimum = (Size(-1, prefSizeBefore.height + 1));

                    Size prefSize = object->calcPreferredSize();

                    REQUIRE(prefSize == prefSizeBefore + Size(0, 1));
                }
            }

            SECTION("preferredSizeMaximum influence")
            {
                // some views (for example some top level windows) have an
                // intrinsic minimum size. The result of calcPreferredSize will
                // not go below that. So to ensure that we are in a position to
                // verify maxSize, we first have to ensure that we are above
                // that limit. We do that by artificially enlarging the
                // preferred size with a big padding.

                view->padding = (UiMargin(300));

                CONTINUE_SECTION_WHEN_IDLE(view, object, keepAliveDuringContinuations)
                {
                    Size prefSizeBefore = object->calcPreferredSize();

                    SECTION("bigger than preferred size")
                    {
                        view->preferredSizeMaximum = (prefSizeBefore + Size(1, 1));

                        Size prefSize = object->calcPreferredSize();

                        REQUIRE(prefSize == prefSizeBefore);
                    }

                    SECTION("same as preferred size")
                    {
                        view->preferredSizeMaximum = (prefSizeBefore);

                        Size prefSize = object->calcPreferredSize();

                        REQUIRE(prefSize == prefSizeBefore);
                    }

                    SECTION("width smaller than preferred width")
                    {
                        view->preferredSizeMaximum = (Size(prefSizeBefore.width - 1, Size::componentNone()));

                        Size prefSize = object->calcPreferredSize();

                        REQUIRE(prefSize.width < prefSizeBefore.width);
                    }

                    SECTION("height smaller than preferred height")
                    {
                        view->preferredSizeMaximum = (Size(Size::componentNone(), prefSizeBefore.height - 1));

                        Size prefSize = object->calcPreferredSize();

                        REQUIRE(prefSize.height < prefSizeBefore.height);
                    }
                };
            }

            SECTION("with constrained width plausible")
            {
                // this is difficult to test, since it depends heavily on what
                // kind of view we actually work with. Also, it is perfectly
                // normal for different core implementations to have different
                // preferred size values for the same inputs.

                // So we can only test rough plausibility here.
                Size prefSize = object->calcPreferredSize();

                SECTION("unconditionalWidth")
                {
                    // When we specify exactly the unconditional preferred width
                    // then we should get exactly the unconditional preferred
                    // height
                    REQUIRE(object->calcPreferredSize(Size(prefSize.width, Size::componentNone())).height ==
                            prefSize.height);
                }

                SECTION("unconditionalWidth/2")
                {
                    REQUIRE(object->calcPreferredSize(Size(prefSize.width / 2, Size::componentNone())).height >=
                            prefSize.height);
                }

                SECTION("zero")
                {
                    REQUIRE(object->calcPreferredSize(Size(0, Size::componentNone())).height >= prefSize.height);
                }
            }

            SECTION("with constrained height plausible")
            {
                Size prefSize = object->calcPreferredSize();

                SECTION("unconditionalHeight")
                REQUIRE(object->calcPreferredSize(Size(Size::componentNone(), prefSize.height)).width ==
                        prefSize.width);

                SECTION("unconditionalHeight/2")
                REQUIRE(object->calcPreferredSize(Size(Size::componentNone(), prefSize.height / 2)).width >=
                        prefSize.width);

                SECTION("zero")
                REQUIRE(object->calcPreferredSize(Size(Size::componentNone(), 0)).width >= prefSize.width);
            }

            SECTION("preferredSizeHint influence")
            {
                // some views (for example some top level windows) have an
                // intrinsic minimum size. The result of calcPreferredSize will
                // not go below that. So to ensure that we are in a position to
                // verify maxSize, we first have to ensure that we are above
                // that limit. We do that by artificially enlarging the
                // preferred size with a big padding.

                view->padding = (UiMargin(300));

                CONTINUE_SECTION_WHEN_IDLE(view, object, keepAliveDuringContinuations)
                {
                    Size prefSizeBefore = object->calcPreferredSize();

                    SECTION("bigger than preferred size")
                    {
                        view->preferredSizeHint = (prefSizeBefore + Size(1, 1));

                        Size prefSize = object->calcPreferredSize();

                        REQUIRE(prefSize == prefSizeBefore);
                    }

                    SECTION("same as preferred size")
                    {
                        view->preferredSizeHint = (prefSizeBefore);

                        Size prefSize = object->calcPreferredSize();

                        REQUIRE(prefSize == prefSizeBefore);
                    }

                    SECTION("width smaller than preferred width")
                    {
                        view->preferredSizeHint = (Size(prefSizeBefore.width - 1, Size::componentNone()));

                        Size prefSize = object->calcPreferredSize();

                        // it depends on the view whether or not the width hint
                        // has an effect
                        if (shouldPrefererredWidthHintHaveAnEffect<ViewType>()) {
                            REQUIRE(prefSize.width < prefSizeBefore.width);

                            // the height may have increase as a result of the
                            // width being reduced. It may also have shrunk (for
                            // example, if an image is shrunk as the result of
                            // the hint. So we cannot test anything here, other
                            // than that we get a height that is >0
                            REQUIRE(prefSize.height > 0);
                        } else
                            REQUIRE(prefSize == prefSizeBefore);
                    }

                    SECTION("height smaller than preferred height")
                    {
                        view->preferredSizeHint = (Size(Size::componentNone(), prefSizeBefore.height - 1));

                        Size prefSize = object->calcPreferredSize();

                        if (shouldPrefererredHeightHintHaveAnEffect<ViewType>()) {
                            REQUIRE(prefSize.height < prefSizeBefore.height);

                            REQUIRE(prefSize.width > 0);
                        } else
                            REQUIRE(prefSize == prefSizeBefore);
                    }
                };
            }
        }
    }
}
