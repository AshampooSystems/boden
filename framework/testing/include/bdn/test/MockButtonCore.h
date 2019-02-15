#pragma once

#include <bdn/test/MockViewCore.h>

#include <bdn/Button.h>
#include <bdn/IButtonCore.h>

namespace bdn
{
    namespace test
    {

        /** Implementation of a "fake" button core that does not actually show
           anything visible, but behaves otherwise like a normal button core.

            See MockUIProvider.
            */
        class MockButtonCore : public MockViewCore, virtual public IButtonCore
        {
          public:
            MockButtonCore(std::shared_ptr<Button> button) : MockViewCore(button) { _label = button->label; }

            /** Returns the current label of the button.*/
            String getLabel() const { return _label; }

            /** Returns the number of times that a new label has been set.*/
            int getLabelChangeCount() const { return _labelChangeCount; }

            void setLabel(const String &label) override
            {
                _label = label;
                _labelChangeCount++;
            }

            Size calcPreferredSize(const Size &availableSpace = Size::none()) const override
            {
                MockViewCore::calcPreferredSize(availableSpace);

                BDN_REQUIRE_IN_MAIN_THREAD();

                Size size = _getTextSize(_label);

                // add our padding
                std::shared_ptr<View> view = getOuterViewIfStillAttached();
                if (view != nullptr) {
                    if (view->padding.get())
                        size += uiMarginToDipMargin(*view->padding.get());
                }

                // add some space for the fake button border
                size += Margin(4, 5);

                // ignore available space. We have a fixed size.

                if (view != nullptr) {
                    // clip to min and max size
                    size.applyMinimum(view->preferredSizeMinimum);
                    size.applyMaximum(view->preferredSizeMaximum);
                }

                return size;
            }

            void generateClick()
            {
                std::shared_ptr<View> view = getOuterViewIfStillAttached();
                if (view != nullptr) {
                    ClickEvent evt(view);

                    std::dynamic_pointer_cast<Button>(view)->onClick().notify(evt);
                }
            }

          protected:
            String _label;
            int _labelChangeCount = 0;
        };
    }
}
