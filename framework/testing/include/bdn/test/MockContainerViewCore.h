#ifndef BDN_TEST_MockContainerViewCore_H_
#define BDN_TEST_MockContainerViewCore_H_

#include <bdn/test/MockViewCore.h>

#include <bdn/ContainerView.h>

#include <bdn/ProgrammingError.h>

namespace bdn
{
    namespace test
    {

        /** Implementation of a "fake" container view core that does not
           actually show anything visible, but behaves otherwise like a normal
           container view core.

            See MockUiProvider.
            */
        class MockContainerViewCore : public MockViewCore
        {
          public:
            MockContainerViewCore(ContainerView *view) : MockViewCore(view) {}

            Size calcPreferredSize(const Size &availableSpace = Size::none()) const override
            {
                MockViewCore::calcPreferredSize(availableSpace);

                // call the outer container's preferred size calculation
                P<ContainerView> outerView = cast<ContainerView>(getOuterViewIfStillAttached());
                if (outerView != nullptr)
                    return outerView->calcContainerPreferredSize(availableSpace);
                else
                    return Size(0, 0);
            }

            void layout() override
            {
                BDN_REQUIRE_IN_MAIN_THREAD();

                _layoutCount++;

                if (!_overrideLayoutFunc || !_overrideLayoutFunc()) {
                    P<ContainerView> view = cast<ContainerView>(getOuterViewIfStillAttached());

                    P<ViewLayout> layout = view->calcContainerLayout(view->size());
                    layout->applyTo(view);
                }
            }
        };
    }
}

#endif
