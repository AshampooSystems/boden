#pragma once

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

            See MockUIProvider.
            */
        class MockContainerViewCore : public MockViewCore
        {
          public:
            MockContainerViewCore(std::shared_ptr<ContainerView> view) : MockViewCore(view) {}

            Size calcPreferredSize(const Size &availableSpace = Size::none()) const override
            {
                MockViewCore::calcPreferredSize(availableSpace);

                // call the outer container's preferred size calculation
                std::shared_ptr<ContainerView> outerView =
                    std::dynamic_pointer_cast<ContainerView>(getOuterViewIfStillAttached());
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
                    std::shared_ptr<ContainerView> view =
                        std::dynamic_pointer_cast<ContainerView>(getOuterViewIfStillAttached());

                    std::shared_ptr<ViewLayout> layout = view->calcContainerLayout(view->size);
                    layout->applyTo(view);
                }
            }
        };
    }
}
